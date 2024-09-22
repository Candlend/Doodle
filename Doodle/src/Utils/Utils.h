#pragma once

#include "Core.h"
#include "pch.h"
#include <chrono>

DOO_API std::string NormalizePath(const std::string &path);
DOO_API std::string FormatTimePoint(const std::chrono::time_point<std::chrono::system_clock> &tp);
DOO_API void PrintBinary(const void *ptr, size_t size);
DOO_API std::string RemoveExtension(const std::string &filename);
DOO_API std::string GetDirectory(const std::string &path);