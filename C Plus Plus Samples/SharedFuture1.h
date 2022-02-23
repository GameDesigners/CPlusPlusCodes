#ifndef SHADREFUNTRUE1_H
#define SHADREFUNTRUE1_H

#include <future>
#include <thread>
#include <chrono>
#include <random>
#include <iostream>
#include <exception>

int queryNumber()
{
	std::cout << "read number:";
	int num;
	std::cin >> num;

	if (!std::cin)
	{
		throw std::runtime_error("no number read");
	}
	return num;
}

void doSomething_sharefuture(char c, std::shared_future<int> f)
{
	try
	{
		int num = f.get();
		for (int i = 0; i < num; ++i)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(100));
			std::cout.put(c).flush();
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << "EXCEPTION in thread" << std::this_thread::get_id() << ":" << ex.what() << std::endl;
	}
}

void UseSharedFuture1()
{
	try
	{
		std::shared_future<int> f = std::async(queryNumber);

		auto f1 = std::async(std::launch::async, doSomething_sharefuture, '.', f);
		auto f2 = std::async(std::launch::async, doSomething_sharefuture, '+', f);
		auto f3 = std::async(std::launch::async, doSomething_sharefuture, '*', f);

		f1.get();
		f2.get();
		f3.get();
	}
	catch (std::exception& ex)
	{
		std::cerr << "Exception:" << ex.what() << std::endl;
	}
	std::cout << "\ndone" << std::endl;
}

#endif // !SHADREFUNTRUE1_H
