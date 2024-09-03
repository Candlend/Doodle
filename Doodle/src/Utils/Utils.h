#pragma once

#include "pch.h"
#include <chrono>

DOO_API std::string NormalizePath(const std::string &path);
DOO_API std::string FormatTimePoint(const std::chrono::time_point<std::chrono::system_clock> &tp);
DOO_API void PrintBinary(const void *ptr, size_t size);