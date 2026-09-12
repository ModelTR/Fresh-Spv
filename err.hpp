#pragma once

#include <algorithm>
#include <cassert>
#include <string>
#include <utility>
#include <vector>

#include <fmt/base.h>
#include <fmt/format.h>
#include <tl/expected.hpp>
#include <spdlog/spdlog.h>

template <class T> using vec = std::vector<T>;

using str = std::string;

class Err {
private:
	bool printable_ = true;
	vec<str> logged_conts_;
	vec<spdlog::level::level_enum> levels_;

public:
	explicit Err(str &&mesg, spdlog::level::level_enum lvl);

	Err(const Err &other) = default;

	Err(Err &&other) = default;

	template <class... Args>
	auto add_err(spdlog::format_string_t<Args...> fmt, Args &&...args)
		-> Err & {
		logged_conts_.push_back(
			std::move(fmt::format(fmt, std::forward<Args>(args)...)));
		levels_.push_back(spdlog::level::err);
		return *this;
	}

	template <class... Args>
	auto add_info(spdlog::format_string_t<Args...> fmt, Args &&...args)
		-> Err & {
		logged_conts_.push_back(
			std::move(fmt::format(fmt, std::forward<Args>(args)...)));
		levels_.push_back(spdlog::level::info);
		return *this;
	}

	template <class... Args>
	auto add_warning(spdlog::format_string_t<Args...> fmt, Args &&...args)
		-> Err & {
		logged_conts_.push_back(
			std::move(fmt::format(fmt, std::forward<Args>(args)...)));
		levels_.push_back(spdlog::level::warn);
		return *this;
	}

	template <class... Args>
	auto add_critical(spdlog::format_string_t<Args...> fmt, Args &&...args)
		-> Err & {
		logged_conts_.push_back(
			std::move(fmt::format(fmt, std::forward<Args>(args)...)));
		levels_.push_back(spdlog::level::critical);
		return *this;
	}

	template <class... Args>
	auto add_debug(spdlog::format_string_t<Args...> fmt, Args &&...args)
		-> Err & {
		logged_conts_.push_back(
			std::move(fmt::format(fmt, std::forward<Args>(args)...)));
		levels_.push_back(spdlog::level::debug);
		return *this;
	}

	/// @brief spdlog::level::off as a default represent print all level
	auto print(spdlog::level::level_enum lvl_beprinted = spdlog::level::off)
		-> void;

	template <class Fn>
	friend auto operator|(Err &err, Fn &&func) -> decltype(auto) {
		return std::forward<Fn>(func)(err);
	}

	template <class Fn>
	friend auto operator|(Err &&err, Fn &&func) -> decltype(auto) {
		return std::forward<Fn>(func)(err);
	}
};

template <class... Args>
auto add_err(spdlog::format_string_t<Args...> fmt, Args &&...args) {
	return [=](Err &e) -> Err & {
		e.add_err(fmt, std::forward<Args>(args)...);
		return e;
	};
}

template <class... Args>
auto add_info(spdlog::format_string_t<Args...> fmt, Args &&...args) {
	return [=](Err &e) -> Err & {
		e.add_info(fmt, std::forward<Args>(args)...);
		return e;
	};
}

template <class... Args>
auto add_warning(spdlog::format_string_t<Args...> fmt, Args &&...args) {
	return [=](Err &e) -> Err & {
		e.add_warning(fmt, std::forward<Args>(args)...);
		return e;
	};
}

template <class... Args>
auto add_critical(spdlog::format_string_t<Args...> fmt, Args &&...args) {
	return [=](Err &e) -> Err & {
		e.add_warning(fmt, std::forward<Args>(args)...);
		return e;
	};
}

template <class... Args>
auto add_debug(spdlog::format_string_t<Args...> fmt, Args &&...args) {
	return [=](Err &e) -> Err & {
		e.add_debug(fmt, std::forward<Args>(args)...);
		return e;
	};
}

inline auto print() {
	return [](Err &e) -> void { e.print(); };
}

namespace util {

template <class T> using res = tl::expected<T, Err>;

using err = tl::unexpected<Err>;

template <class... Args>
auto make_err(spdlog::format_string_t<Args...> fmt, Args &&...args) -> err {
	return tl::unexpected(
		Err{fmt::format(fmt, std::forward<Args>(args)...), spdlog::level::err});
}

} // namespace util
