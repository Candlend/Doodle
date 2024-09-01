#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Log.h"

#ifdef DOO_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#ifdef DOO_BUILD_PYTHON
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11; // NOLINT
#endif