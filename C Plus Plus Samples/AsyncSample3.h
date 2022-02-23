#ifndef ASYNCSAMPLE3_H
#define ASYNCSAMPLE3_H

#include <future>
#include <thread>
#include <chrono>
#include <random>
#include <iostream>
#include <exception>

int doSomething3(char c)
{
	std::default_random_engine dre(c);
	std::uniform_int_distribution<int> id(10, 1000);

	for (int i = 0; i < 10; ++i)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(id(dre)));
		std::cout.put(c).flush();
	}
	return c;
}

void UseAsyncSample3()
{
	std::cout << "starting 2 operations asynchronously" << std::endl;

	auto f1 = std::async([] {doSomething3('.'); });
	auto f2 = std::async([] {doSomething3('+'); });

	if (f1.wait_for(std::chrono::seconds(0)) != std::future_status::deferred ||
		f2.wait_for(std::chrono::seconds(0)) != std::future_status::deferred)
	{
		while (f1.wait_for(std::chrono::seconds(0)) != std::future_status::ready ||
			   f2.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
		{
			std::this_thread::yield();
		}
	}

	std::cout.put('\n').flush();

	try
	{
		f1.get();
		f2.get();
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Exception:" << ex.what() << std::endl;
	}

	std::cout << "\ndone" << std::endl;
}


#endif // !ASYNCSAMPLE2_H3