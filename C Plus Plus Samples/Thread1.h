#ifndef THREAD1_H
#define THREAD1_H

#include <thread>
#include <chrono>
#include <random>
#include <iostream>
#include <exception>

void doSomething_Thread1(int num,char c)
{
	try
	{
		std::default_random_engine dre(42 * c);
		std::uniform_int_distribution<int> id(10, 1000);
		for (int i = 0; i < num; ++i)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(id(dre)));
			std::cout.put(c).flush();
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << "THREAD-EXCEPTION(thread" << std::this_thread::get_id() << ")" << ex.what() << std::endl;
	}
}

void UseThreadSample1()
{
	try
	{
		std::thread t1(doSomething_Thread1, 5, '.');
		std::cout << "- started fg thread " << t1.get_id() << std::endl;

		for (int i = 0; i < 5; ++i)
		{
			std::thread t(doSomething_Thread1, 10, 'a' + i);
			std::cout << "- detach started bg thread " << t.get_id() << std::endl;
			t.detach();
		}

		std::cin.get();
		std::cout << "- join fg thread " << t1.get_id() << std::endl;
		t1.join();  //join()为阻塞方法，意味着t1执行完后join()才会有返回值
	}
	catch (const std::exception& ex)
	{
		std::cerr << "EXCEPTION" << ex.what() << std::endl;
	}
}

#endif // !THREAD1_H
