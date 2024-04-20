#pragma once

#include "pch.h"

std::string NormalizePath(const std::string &path) {
    std::string normalized = path;
    for (char &c : normalized) {
#ifdef DOO_PLATFORM_WINDOWS
        if (c == '/') {
            c = '\\';
        }
#else
        if (c == '\\') {
            c = '/';
        }
#endif
    }

    return normalized;
}