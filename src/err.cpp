#include "err.hpp"

#include <ranges>
#include <tuple>
#include <cstddef>

namespace vws = std::views;

Err::Err(str &&mesg, spdlog::level::level_enum lvl) {
	logged_conts_.push_back(std::move(mesg));
	levels_.push_back(lvl);
}


auto Err::print(spdlog::level::level_enum lvl_beprinted) -> void {
	assert(logged_conts_.size() == levels_.size());

		/// @note use "uz" std::size_t literal replace size_t{0} from C++23
		/// @todo use std::views::zip instead from C++23
		/// @brief use vws::filter to get the corresponding logged content with
		/// its level
		auto zipped = vws::iota(size_t{0}, logged_conts_.size()) |
					  vws::transform([&](const size_t &idx) {
						  return std::tie(logged_conts_[idx], levels_[idx]);
					  }) |
					  vws::filter([&](const auto &tied) {
						  if (lvl_beprinted == spdlog::level::off) {
							  return true;
						  } else {
							  return std::get<1>(tied) == lvl_beprinted;
						  }
					  });
		for (const auto &[cont, lvl] : zipped) {
			spdlog::log(lvl, cont);
		}
}