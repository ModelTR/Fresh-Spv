#pragma once

#include <chrono>
#include <source_location>
#include <string>

namespace util {

class Recorder {
private:
    using steady_clock = std::chrono::steady_clock;
	using steady_time_point =
		std::chrono::time_point<steady_clock>;
	using str = std::string;
	using src_loc = std::source_location;

	str label_;
	steady_time_point start_time_;
	std::source_location start_loc_;
	bool recordable_ = true;

public:
	Recorder(const str &lb = "", const src_loc &loc = src_loc::current());
	~Recorder();
	auto record(const src_loc &loc = src_loc::current()) -> void;
};

} // namespace util