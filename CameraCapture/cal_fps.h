#include <iostream>
#include <numeric>
#include <opencv2/opencv.hpp>

// Note: FpsCounterLazy class must be used in a loop and call in this way "FpsCounterLazy count_here;"
// do NOT new it since it will create the object in manual managed storage
class FpsCounterLazy
{
public:
	std::chrono::high_resolution_clock::time_point tp_start;
	std::chrono::high_resolution_clock::time_point tp_stop;

	inline FpsCounterLazy()
	{
		tp_start = std::chrono::high_resolution_clock::now();
	}
	inline ~FpsCounterLazy()
	{
		tp_stop = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double, std::nano> dur_nanos = tp_stop - tp_start;
		double fps = std::chrono::nanoseconds(1000000000) / dur_nanos;
		std::cout << "fps=" << std::setprecision(3) << std::fixed << fps << std::endl;
	}
};

// Instantiate FpsCounter class (either use default sample size or provide your own), then just call aio()
class FpsCounter
{
public:
	std::chrono::high_resolution_clock::time_point tp_start;
	std::chrono::high_resolution_clock::time_point tp_stop;
	std::deque<double> durnations; // Note: the std::deque::resize() keeps from the front to n, so we need to use the deque backwards
	static constexpr double one_sec_in_ns = 1000000000.0;
	size_t durnation_queue_size;
	double fps, fps_avg;

	FpsCounter(size_t avg_sample_size = 10)
	{
		// Set output format
		std::cout << std::setprecision(3) << std::fixed;
		// Set sample size
		durnation_queue_size = avg_sample_size;
	}

	inline void count()
	{
		// Set time base then return for the first run
		if (tp_start.time_since_epoch().count() == 0)
		{
			tp_start = std::chrono::high_resolution_clock::now();
			return;
		}
		tp_stop = std::chrono::high_resolution_clock::now();
		durnations.push_front((tp_stop - tp_start).count());
		tp_start = std::chrono::high_resolution_clock::now();
	}

	inline void cal_single()
	{
		fps = one_sec_in_ns / durnations.front();
	}

	inline void cal_avg()
	{
		if (durnations.size() > durnation_queue_size) durnations.resize(durnation_queue_size);
		fps_avg = one_sec_in_ns / (std::accumulate(durnations.begin(), durnations.end(), 0.0, std::plus<double>()) / durnations.size());
	}

	inline void print()
	{
		std::cout
			<< std::setprecision(3) << std::fixed // Set print format
			<< "fps=" << fps
			<< " "
			<< "fps_avg=" << fps_avg
			<< " "
			<< "diff=" << fps - fps_avg
			<< std::endl;
	}

	inline void aio()
	{
		count();
		//cal_single();
		cal_avg();
		//print();
	}
};