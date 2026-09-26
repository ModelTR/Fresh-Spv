#pragma once

#include "type_alias.hpp"
#include <source_location>
#include <spdlog/spdlog.h>

namespace util {

class Recorder {
private:
	str label_;
	str recorded_msg_;
	steady_time_point start_time_;
	std::source_location start_loc_;
	bool recordable_ = true;

public:
	Recorder(const str &lb = "", const src_loc &loc = src_loc::current());

	Recorder(const Recorder &) = delete;
	auto operator=(const Recorder &) -> Recorder & = delete;

	~Recorder();

	auto record(const src_loc &loc = src_loc::current()) -> void;
	auto print(const src_loc &loc = src_loc::current()) -> void {
		if (recorded_msg_.empty()) {
			this->record(loc);
		}
		spdlog::info(recorded_msg_);
	}
};

} // namespace util