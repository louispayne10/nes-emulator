#pragma once

#include <fmt/format.h>

#include <cassert>

inline void vdebug_message(const char* filename, int line_number, fmt::string_view format, fmt::format_args args)
{
    fmt::print(stderr, "{}: {}: ", filename, line_number);
    fmt::vprint(stderr, format, args);
    fmt::print(stderr, "\n");
}

template <typename S, typename... Args>
void debug_message(const char* filename, int line_number, const S& format, Args&&... args)
{
    vdebug_message(filename, line_number, format, fmt::make_format_args(args...));
}

#define DEBUG_LOG(format, ...) debug_message(__FILE__, __LINE__, FMT_STRING(format), __VA_ARGS__)

template <typename... Args>
void info_message(fmt::format_string<Args...> fmt_string, Args&&... args)
{
    fmt::print(stderr, fmt_string, std::forward<Args>(args)...);
    fmt::print(stderr, "\n");
}

#define NOT_IMPLEMENTED() assert(false && "Not implemented yet")
