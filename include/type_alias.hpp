#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <source_location>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include <chrono>

namespace fs = std::filesystem;

namespace rng = std::ranges;

namespace vws = std::ranges::views;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u63 = uint64_t;

template <class T> using vec = std::vector<T>;

using u8vec = vec<u8>;

using cu8vec = vec<const u8>;

using sizes = vec<size_t>;

using str = std::string;

using str_view = std::string_view;

using paths = vec<fs::path>;

template <size_t Extent = std::dynamic_extent>
using u8span = std::span<u8, Extent>;

template <size_t Extent = std::dynamic_extent>
using cu8span = std::span<const u8, Extent>;

using src_loc = std::source_location;

using steady_clock = std::chrono::steady_clock;
using steady_time_point = std::chrono::time_point<steady_clock>;
