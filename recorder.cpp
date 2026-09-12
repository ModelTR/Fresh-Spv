#include "recorder.hpp"

#include <spdlog/spdlog.h>

util::Recorder::Recorder(const str &lb, const src_loc &loc)
	: label_(lb), start_time_(steady_clock::now()), start_loc_(loc) {}

util::Recorder::~Recorder() {
	if (recordable_) {
		spdlog::info("Fn: \"{}\" [{} to ...] takes {}ms\n",
					 (label_.size() != 0 ? label_ : start_loc_.function_name()),
					 start_loc_.line(),
					 std::chrono::duration_cast<std::chrono::milliseconds>(
						 (steady_clock::now() - start_time_))
						 .count());
	}
}

auto util::Recorder::record(const src_loc &loc) -> void {
	recordable_ = false;
	spdlog::info("Fn: \"{}\" [{} to {}] takes {}ms\n",
				 (label_.size() != 0 ? label_ : start_loc_.function_name()),
				 start_loc_.line(), loc.line(),
				 std::chrono::duration_cast<std::chrono::milliseconds>(
					 (steady_clock::now() - start_time_))
					 .count());
}