#ifndef _TIMER
#define _TIMER

#include<thread>
#include<chrono>
#include<functional>

class Timer
{
private :
	std::thread td;
	bool alive{ false };
	long long repeat_cnt{ Timer::Infinite };
	long long left_cnt{ Timer::Infinite };
	std::chrono::milliseconds interval{ 0 };
	std::function<void(void)> func{ nullptr };

	void thread_func(void)
	{
		if (repeat_cnt == Infinite)
			while (alive)
				sleep_and_run();
		else
			while (left_cnt--)
				sleep_and_run();
	}

	void sleep_and_run(void)
	{
		std::this_thread::sleep_for(interval);

		if (alive)
			get_function()();
	}

public : 
	static const long long Infinite;

	Timer() {}

	Timer(const std::function<void(void)> &f) : func{ f } {}

	Timer(const std::function<void(void)> &f, const long long i, const long long repeat = Infinite)
		: func{ f }, interval{ i }, repeat_cnt{ repeat } {}

	void start(bool async = true)
	{
		if (alive)
			return;

		alive = true;
		left_cnt = repeat_cnt;

		if (async)
			td = std::thread{ thread_func, this };
	}

	void stop(void)
	{
		alive = false;
		td.join();
	}

	void set_function(const std::function<void(void)> &f)
	{
		func = f;
	}

	void set_repeat_count(const long long r)
	{
		if (alive)
			return;
		repeat_cnt = r;
	}

	void set_interval(const long long i)
	{
		if (alive)
			return;
		interval = std::chrono::milliseconds{ i };
	}

	const std::function<void(void)>& get_function(void) const
	{
		return func;
	}
};

const long long Timer::Infinite = -1L;

#endif // !_TIMER
