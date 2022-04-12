#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

#define timer_start() ctm::start()
#define timer_stop() ctm::stop()
#define timer_result_hours() ctm::result<std::chrono::hours>()
#define timer_result_minutes() ctm::result<std::chrono::minutes>()
#define timer_result_seconds() ctm::result<std::chrono::seconds>()
#define timer_result_milliseconds() ctm::result<std::chrono::milliseconds>()
#define timer_result_microseconds() ctm::result<std::chrono::microseconds>()
#define timer_result_nanoseconds() ctm::result<std::chrono::nanoseconds>()

class ctm final
{
public:
	ctm() = delete;

	inline static void start() noexcept
	{
		start_time = std::chrono::steady_clock::now();
	}

	inline static void stop() noexcept
	{
		interval = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start_time);
	}
	
	template <typename T>
	inline static uint64_t result()
	{
		return std::chrono::duration_cast<T>(interval).count();
	}

private:
	inline static std::chrono::nanoseconds interval;
	inline static std::chrono::time_point<std::chrono::steady_clock> start_time;

};

#endif // TIMER_HPP
