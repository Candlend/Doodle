#include <atomic>
#include <chrono>
#include <filesystem>
#include <functional>
#include <string>
#include <thread>
#include <unordered_map>

namespace Doodle
{

enum class FileEventType
{
    Created,
    Modified,
    Erased
};

class FileWatcher
{
public:
    FileWatcher(std::filesystem::path pathToWatch,
                std::chrono::duration<int, std::milli> delay = std::chrono::milliseconds(500))
        : m_pathToWatch{pathToWatch}, m_delay{delay}, m_running{false}
    {
    }

    ~FileWatcher()
    {
        Stop(); // 确保在析构时停止监控
    }

    void Start(const std::function<void(std::filesystem::path, FileEventType)> &action)
    {
        CheckForChanges(action);
        m_running = true;
        m_thread = std::thread([this, action]() {
            while (m_running)
            {
                std::this_thread::sleep_for(m_delay);
                CheckForChanges(action);
            }
        });
    }

    void Stop()
    {
        m_running = false;
        if (m_thread.joinable())
        {
            m_thread.join(); // 等待线程结束
        }
    }

private:
    std::unordered_map<std::filesystem::path, std::filesystem::file_time_type> m_paths;
    std::atomic<bool> m_running;
    std::thread m_thread;
    std::filesystem::path m_pathToWatch;            // 监视的路径
    std::chrono::duration<int, std::milli> m_delay; // 检查间隔时间

    void CheckForChanges(const std::function<void(std::filesystem::path, FileEventType)> &action)
    {
        auto it = m_paths.begin();
        while (it != m_paths.end())
        {
            if (!std::filesystem::exists(it->first))
            {
                action(it->first, FileEventType::Erased);
                it = m_paths.erase(it);
            }
            else
            {
                it++;
            }
        }

        // 检查文件是否被创建或修改
        for (const auto &file : std::filesystem::recursive_directory_iterator(m_pathToWatch))
        {
            auto currentFileLastWriteTime = std::filesystem::last_write_time(file);

            // 文件创建
            if (!Contains(file.path()))
            {
                m_paths[file.path()] = currentFileLastWriteTime;
                action(file.path(), FileEventType::Created);
                // 文件修改
            }
            else
            {
                if (m_paths[file.path()] != currentFileLastWriteTime)
                {
                    m_paths[file.path()] = currentFileLastWriteTime;
                    action(file.path(), FileEventType::Modified);
                }
            }
        }
    }

    bool Contains(const std::filesystem::path &key)
    {
        auto el = m_paths.find(key);
        return el != m_paths.end();
    }
};

} // namespace Doodle
