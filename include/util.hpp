#pragma once

#include "spdlog/spdlog.h"

#include "err.hpp"

namespace util {

template <class T, class... Args>
auto expect(const T &value, spdlog::format_string_t<Args...> fmt,
			Args &&...args) -> util::res<T> {
	if (!value) {
		return util::make_err(fmt, std::forward<Args>(args)...);
	}
	return value;
}

template <class T, class... Args>
auto require(const T &value, spdlog::format_string_t<Args...> fmt,
			 Args &&...args) -> util::res<T> {
	if (!value) {
		return util::make_critical(fmt, std::forward<Args>(args)...);
	}
	return value;
}
} // namespace util