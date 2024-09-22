#include "pch.h"
#include <cstdint>

#ifdef DOO_PLATFORM_LINUX
#include <libgen.h>
#endif

#include <format>
#include <nfd.hpp>
#include <shellapi.h>

#include "FileSystem.h"
#include "Utils.h"

namespace Doodle
{

std::filesystem::path FileSystem::GetWorkingDirectory()
{
    return std::filesystem::current_path();
}

void FileSystem::SetWorkingDirectory(std::filesystem::path path)
{
    std::filesystem::current_path(path);
}

bool FileSystem::CreateDirectory(const std::filesystem::path &directory)
{
    return std::filesystem::create_directories(directory);
}

bool FileSystem::CreateDirectory(const std::string &directory)
{
    return CreateDirectory(std::filesystem::path(directory));
}

bool FileSystem::Move(const std::filesystem::path &oldFilepath, const std::filesystem::path &newFilepath)
{
    if (FileSystem::Exists(newFilepath))
        return false;

    std::filesystem::rename(oldFilepath, newFilepath);
    return true;
}

bool FileSystem::Copy(const std::filesystem::path &oldFilepath, const std::filesystem::path &newFilepath)
{
    if (FileSystem::Exists(newFilepath))
        return false;

    std::filesystem::copy(oldFilepath, newFilepath);
    return true;
}

bool FileSystem::MoveFile(const std::filesystem::path &filepath, const std::filesystem::path &dest)
{
    return Move(filepath, dest / filepath.filename());
}

bool FileSystem::CopyFile(const std::filesystem::path &filepath, const std::filesystem::path &dest)
{
    return Copy(filepath, dest / filepath.filename());
}

bool FileSystem::Rename(const std::filesystem::path &oldFilepath, const std::filesystem::path &newFilepath)
{
    return Move(oldFilepath, newFilepath);
}

bool FileSystem::RenameFilename(const std::filesystem::path &oldFilepath, const std::string &newName)
{
    std::filesystem::path newPath =
        oldFilepath.parent_path() / std::filesystem::path(newName + oldFilepath.extension().string());
    return Rename(oldFilepath, newPath);
}

bool FileSystem::Exists(const std::filesystem::path &filepath)
{
    return std::filesystem::exists(filepath);
}

bool FileSystem::Exists(const std::string &filepath)
{
    return std::filesystem::exists(std::filesystem::path(filepath));
}

bool FileSystem::DeleteFile(const std::filesystem::path &filepath)
{
    if (!FileSystem::Exists(filepath))
        return false;

    if (std::filesystem::is_directory(filepath))
        return std::filesystem::remove_all(filepath) > 0;
    return std::filesystem::remove(filepath);
}

bool FileSystem::IsDirectory(const std::filesystem::path &filepath)
{
    return std::filesystem::is_directory(filepath);
}

FileStatus FileSystem::TryOpenFile(const std::filesystem::path &filepath)
{
    HANDLE fileHandle = CreateFile(filepath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (fileHandle == INVALID_HANDLE_VALUE)
    {
        DWORD errorCode = GetLastError();
        if (errorCode == ERROR_FILE_NOT_FOUND || errorCode == ERROR_PATH_NOT_FOUND)
            return FileStatus::Invalid;
        if (errorCode == ERROR_SHARING_VIOLATION)
            return FileStatus::Locked;

        return FileStatus::OtherError;
    }

    CloseHandle(fileHandle);
    return FileStatus::Success;
}

FileStatus FileSystem::TryOpenFileAndWait(const std::filesystem::path &filepath, uint64_t waitMs)
{
    FileStatus fileStatus = TryOpenFile(filepath);
    if (fileStatus == FileStatus::Locked)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<uint64_t>(waitMs)));
        return TryOpenFile(filepath);
    }
    return fileStatus;
}

// returns true <=> fileA was last modified more recently than fileB
bool FileSystem::IsNewer(const std::filesystem::path &fileA, const std::filesystem::path &fileB)
{
    return std::filesystem::last_write_time(fileA) > std::filesystem::last_write_time(fileB);
}

bool FileSystem::ShowFileInExplorer(const std::filesystem::path &path)
{
    auto absolutePath = std::filesystem::canonical(path);
    if (!Exists(absolutePath))
        return false;

#ifdef DOO_PLATFORM_WINDOWS
    std::string cmd = std::format("explorer.exe /select,\"{0}\"", absolutePath.string());
#elif defined(DOO_PLATFORM_LINUX)
    std::string cmd = std::format("xdg-open \"{0}\"", dirname(absolutePath.string().data()));
#endif
    system(cmd.c_str()); // NOLINT
    return true;
}

bool FileSystem::OpenDirectoryInExplorer(const std::filesystem::path &path)
{
#ifdef DOO_PLATFORM_WINDOWS
    auto absolutePath = std::filesystem::canonical(path);
    if (!Exists(absolutePath))
        return false;

    ShellExecute(NULL, L"explore", absolutePath.wstring().c_str(), NULL, NULL, SW_SHOWNORMAL);
    return true;
#elif defined(DOO_PLATFORM_LINUX)
    return ShowFileInExplorer(path);
#endif
}

bool FileSystem::OpenExternally(const std::filesystem::path &path)
{
    auto absolutePath = std::filesystem::canonical(path);
    if (!Exists(absolutePath))
        return false;

#ifdef DOO_PLATFORM_WINDOWS
    ShellExecute(NULL, L"open", absolutePath.wstring().c_str(), NULL, NULL, SW_SHOWNORMAL);
    return true;
#elif defined(DOO_PLATFORM_LINUX)
    std::string cmd = std::format("xdg-open \"{0}\"", absolutePath.string().data());
    system(cmd.c_str());
    return true;
#endif
}

std::filesystem::path FileSystem::GetUniqueFileName(const std::filesystem::path &filepath)
{
    if (!FileSystem::Exists(filepath))
        return filepath;

    int counter = 0;
    auto checkID = [&counter, filepath](auto checkID) -> std::filesystem::path {
        ++counter;
        const std::string COUNTER_STR = [&counter] {
            if (counter < 10)
                return "0" + std::to_string(counter);
            else
                return std::to_string(counter);
        }(); // Pad with 0 if < 10;

        std::string newFileName = std::format("{} ({})", RemoveExtension(filepath.filename().string()), COUNTER_STR);

        if (filepath.has_extension())
            newFileName = std::format("{}{}", newFileName, filepath.extension().string());

        if (std::filesystem::exists(filepath.parent_path() / newFileName))
            return checkID(checkID);
        else
            return filepath.parent_path() / newFileName;
    };

    return checkID(checkID);
}

uint64_t FileSystem::GetLastWriteTime(const std::filesystem::path &filepath)
{
    DOO_CORE_ASSERT(FileSystem::Exists(filepath), "FileSystem::GetLastWriteTime - file does not exist: {}",
                    filepath.string());

    if (TryOpenFileAndWait(filepath) == FileStatus::Success)
    {
        std::filesystem::file_time_type lastWriteTime = std::filesystem::last_write_time(filepath);
        return std::chrono::duration_cast<std::chrono::seconds>(lastWriteTime.time_since_epoch()).count();
    }

    DOO_CORE_ERROR("FileSystem::GetLastWriteTime - could not open file: {}", filepath.string());
    return 0;
}

std::filesystem::path FileSystem::OpenFileDialog(const std::initializer_list<FileDialogFilterItem> inFilters)
{
    NFD::UniquePath filePath;
    nfdresult_t result =
        NFD::OpenDialog(filePath, reinterpret_cast<const nfdfilteritem_t *>(inFilters.begin()), inFilters.size());

    switch (result)
    {
    case NFD_OKAY:
        return filePath.get();
    case NFD_CANCEL:
        return "";
    case NFD_ERROR: {
        DOO_CORE_ERROR("NFD-Extended threw an error: {}", NFD::GetError());
        return "";
    }
    }

    return "";
}

std::filesystem::path FileSystem::OpenFolderDialog(const char *initialFolder)
{
    NFD::UniquePath filePath;
    nfdresult_t result = NFD::PickFolder(filePath, initialFolder);

    switch (result)
    {
    case NFD_OKAY:
        return filePath.get();
    case NFD_CANCEL:
        return "";
    case NFD_ERROR: {
        DOO_CORE_ERROR("NFD-Extended threw an error: {}", NFD::GetError());
        return "";
    }
    }

    return "";
}

std::filesystem::path FileSystem::SaveFileDialog(const std::initializer_list<FileDialogFilterItem> inFilters)
{
    NFD::UniquePath filePath;
    nfdresult_t result =
        NFD::SaveDialog(filePath, reinterpret_cast<const nfdfilteritem_t *>(inFilters.begin()), inFilters.size());

    switch (result)
    {
    case NFD_OKAY:
        return filePath.get();
    case NFD_CANCEL:
        return "";
    case NFD_ERROR: {
        DOO_CORE_ERROR("NFD-Extended threw an error: {}", NFD::GetError());
        return "";
    }
    }

    return "";
}

} // namespace Doodle
