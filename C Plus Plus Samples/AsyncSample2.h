#ifndef ASYNCSAMPLE2_H
#define ASYNCSAMPLE2_H
#include <future>
#include <list>
#include <iostream>
#include <exception>

void task1()
{
	std::list<int> v;
	while (true)
	{
		for (int i = 0; i < 1000000; ++i)
		{
			v.push_back(i);
		}
		std::cout.put('.').flush();
	}
}

void UseAsyncSample2()
{
	std::cout << "starting 2 tasks" << std::endl;
	std::cout << "- task1:process endless loop of memory consumption" << std::endl;
	std::cout << "- task2:wait for <return> and then for task1" << std::endl;

	auto f1 = std::async(task1);
	std::cin.get();  //read a character
	std::cout << "\nwait for the end of task1" << std::endl;

	try
	{
		f1.get();
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Exception:" << ex.what() << std::endl;
	}

}


#endif // !ASYNCSAMPLE2_H
