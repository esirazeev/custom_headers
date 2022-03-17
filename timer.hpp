#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

#define TIMER_START() Timer::Start()
#define TIMER_STOP() Timer::Stop()
#define TIMER_STOP_GAP() Timer::StopGap()
#define TIMER_RESET() Timer::Reset();
#define TIMER_RESULT_HOURS() Timer::GetResultHours()
#define TIMER_RESULT_MINUTES() Timer::GetResultMinutes()
#define TIMER_RESULT_SECONDS() Timer::GetResultSeconds()
#define TIMER_RESULT_MILLISECONDS() Timer::GetResultMilliseconds()
#define TIMER_RESULT_MICROSECONDS() Timer::GetResultMicroseconds()
#define TIMER_RESULT_NANOSECONDS() Timer::GetResultNanoseconds()

class Timer final
{
public:
	Timer() = delete;

	inline static void Start();
	inline static void Stop();
	inline static void StopGap();
	inline static void Reset();
	inline static int64_t GetResultHours();
	inline static int64_t GetResultMinutes();
	inline static int64_t GetResultSeconds();
	inline static int64_t GetResultMilliseconds();
	inline static int64_t GetResultMicroseconds();
	inline static int64_t GetResultNanoseconds();

private:
	inline static bool gap_timer_running = false;
	inline static std::chrono::nanoseconds interval;
	inline static std::chrono::time_point<std::chrono::steady_clock> start;

	template <typename T>
	inline static auto GetTimeInterval();
};

template <typename T>
inline auto Timer::GetTimeInterval()
{
	return std::chrono::duration_cast<T>(interval).count();
}

inline void Timer::Start()
{
	start = std::chrono::steady_clock::now();
}

inline void Timer::Stop()
{
	gap_timer_running = false;

	interval = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start);
}

inline void Timer::StopGap()
{
	if (!gap_timer_running)
	{
		interval = {};
	}
	gap_timer_running = true;

	interval += std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start);
}

inline void Timer::Reset()
{
	interval = {};
}

inline int64_t Timer::GetResultHours()
{
	return GetTimeInterval<std::chrono::hours>();
}

inline int64_t Timer::GetResultMinutes()
{
	return GetTimeInterval<std::chrono::minutes>();
}

inline int64_t Timer::GetResultSeconds()
{
	return GetTimeInterval<std::chrono::seconds>();
}

inline int64_t Timer::GetResultMilliseconds()
{
	return GetTimeInterval<std::chrono::milliseconds>();
}

inline int64_t Timer::GetResultMicroseconds()
{
	return GetTimeInterval<std::chrono::microseconds>();
}

inline int64_t Timer::GetResultNanoseconds()
{
	return GetTimeInterval<std::chrono::nanoseconds>();
}

#endif // TIMER_HPP
