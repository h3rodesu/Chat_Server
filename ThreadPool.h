#pragma once
#include<thread>
#include<mutex>
#include<condition_variable>
#include<queue>
#include<functional>
#include<vector>
class ThreadPool {
private:
	std::mutex mymtx;
	std::condition_variable cv;
	bool endwork;
	std::vector<std::thread>myvec;//Вектор потоков
	std::queue<std::function<void()>>que;//Каждая задача-функция
	size_t size;
public:
	ThreadPool(size_t s);//Принимает кол-во потоков
	void add(std::function<void()>task);//Приниамает сокет через move-семантику
	~ThreadPool();
};