#pragma once

#include <algorithm>
#include <cassert>
#include <source_location>
#include <spdlog/common.h>
#include <utility>
#include <vector>

#include <fmt/base.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <tl/expected.hpp>

#include "type_alias.hpp"

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
		e.add_critical(fmt, std::forward<Args>(args)...);
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

/// @note specialized fmt::formatter<std::source_location> need two
/// FMT_BEGIN_NAMESPACE and FMT_END_NAMESPACE to warpper

FMT_BEGIN_NAMESPACE

template <> struct fmt::formatter<std::source_location> {
	constexpr auto parse(fmt::format_parse_context &ctx) { return ctx.begin(); }

	template <class FormatContext>
	auto format(const std::source_location &loc, FormatContext &ctx) const {
		return fmt::format_to(ctx.out(), "{} : {}", loc.file_name(),
							  loc.function_name());
	}
};

FMT_END_NAMESPACE

namespace util {

template <class T> using res = tl::expected<T, Err>;

using err = tl::unexpected<Err>;

template <spdlog::level::level_enum LVL, class... Args>
auto make_impl(spdlog::format_string_t<Args...> fmt, Args &&...args) {
	return tl::unexpected(
		Err{fmt::format(fmt, std::forward<Args>(args)...), LVL});
}

template <class... Args>
auto make_err(spdlog::format_string_t<Args...> fmt, Args &&...args) -> err {
	return make_impl<spdlog::level::err>(fmt, std::forward<Args>(args)...);
}

template <class... Args>
auto make_info(spdlog::format_string_t<Args...> fmt, Args &&...args) -> err {
	return make_impl<spdlog::level::info>(fmt, std::forward<Args>(args)...);
}

template <class... Args>
auto make_critical(spdlog::format_string_t<Args...> fmt, Args &&...args)
	-> err {
	return make_impl<spdlog::level::critical>(fmt, std::forward<Args>(args)...);
}

template <class T = std::source_location>
auto get_fn_name(fmt::format_string<T> fmt = "Fn[\033[34m{}\033[0m]",
				 T &&loc = std::source_location::current()) -> str {
	return fmt::format(fmt, std::forward<T>(loc));
}

} // namespace util
