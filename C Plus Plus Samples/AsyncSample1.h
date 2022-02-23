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
	 *  ������
	 *  1.async()���Խ�������õĺ��������첽������һ�������߳��ڡ��������У�func1�ڷ����߳��б���������������UseAsyncSample1()���ڵ����̡߳�
	 *  2.async()���� future<>���󡣷���ֵҲ����һ������ֵ��Ҳ����һ���쳣���������ĺ����޷���ֵ�����ʹstd::future<void>
	 *  3.async()���� future<>�����Ǳ�����ڵģ�async()�ǳ������������������ɹ�������Ҫ���future object����ǿ��������
	 */
	std::future<int> result1(std::async(func1));
	
	int result2 = func2();


	/*
	 * future<>().get����ʱ���ᷢ���Ŀ������У�
	 * 1.���func1������async����������һ�������߳��в����ѽ�����������̻��������
	 * 2.���func1��������������δ������get����������ͣ�ͣ�block����func1�����������ý��
	 * 3.���func1������δ�������ᱻǿ��������ͬһ��ͬ�����ã�get����������ͣ��ֱ�����������
	 */
	int result = result1.get() + result2;

	std::cout << "\nresult of func1() + func2():" << result << std::endl;
}


#endif // !ASYNCSAMPLE1_H
