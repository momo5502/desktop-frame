#ifndef RC_INVOKED

#define _HAS_CXX17 1

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

#include <string>
#include <mutex>
#include <vector>
#include <functional>
#include <fstream>
#include <sstream>
#include <regex>
#include <atomic>
#include <set>
#include <filesystem>
#include <optional>

#define RAPIDJSON_NOEXCEPT
#define RAPIDJSON_ASSERT(cond) if (cond); else throw std::runtime_error("rapidjson assert fail");

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

using namespace std::literals;

#endif

// Resources
#define IDI_ICON_1 102
