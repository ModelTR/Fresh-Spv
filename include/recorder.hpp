#pragma once

#include <source_location>
#include "type_alias.hpp"

namespace util {

class Recorder {
private:
	str label_;
	steady_time_point start_time_;
	std::source_location start_loc_;
	bool recordable_ = true;

public:
	Recorder(const str &lb = "", const src_loc &loc = src_loc::current());

	Recorder(const Recorder &) = delete;
	auto operator=(const Recorder &) -> Recorder & = delete;

	~Recorder();
	auto record(const src_loc &loc = src_loc::current()) -> void;
};

} // namespace util