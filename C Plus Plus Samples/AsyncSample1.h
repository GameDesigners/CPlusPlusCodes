#ifndef ASYNCSAMPLE1_H
#define ASYNCSAMPLE1_H

#include <future>
#include <thread>
#include <chrono>
#include <random>
#include <iostream>
#include <exception>

int doSomething(char c)
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

int func1()
{
	return doSomething('.');
}

int func2()
{
	return doSomething('+');
}

void UseAsyncSample1()
{
	std::cout << "starting func1() in background" << " and func2 in foreground:" << std::endl;

	/*
	 *  解析：
	 *  1.async()尝试将其所获得的函数立刻异步启动于一个分离线程内。在例子中，func1在分离线程中被启动，不会阻塞UseAsyncSample1()所在的主线程。
	 *  2.async()返回 future<>对象。返回值也许是一个返回值，也许是一个异常。若启动的函数无返回值，则会使std::future<void>
	 *  3.async()返回 future<>对象是必须存在的，async()是尝试启动，若启动不成功，则需要这个future object才能强迫启动。
	 */
	std::future<int> result1(std::async(func1));
	
	int result2 = func2();


	/*
	 * future<>().get调用时，会发生的可能性有：
	 * 1.如果func1（）被async（）启动于一个分离线程中并且已结束，你会立刻获得其结果。
	 * 2.如果func1（）被启动但尚未结束，get（）会引发停滞（block）待func1（）结束后获得结果
	 * 3.如果func1（）尚未启动，会被强迫启动如同一个同步调用；get（）会引发停滞直至产生结果。
	 */
	int result = result1.get() + result2;

	std::cout << "\nresult of func1() + func2():" << result << std::endl;
}


#endif // !ASYNCSAMPLE1_H
