#include "ThreadPool.h"	
ThreadPool::ThreadPool(size_t s) : size(s), endwork(false){
	for (size_t i = 0; i < size; i++) {
		myvec.emplace_back([this]() {
			while (true) {//поток будет жить пока сам его не закрою или пока программа не закроется
				std::function<void()>task;//Тут будет задача
				{
					std::unique_lock<std::mutex>myLock(this->mymtx);
					cv.wait(myLock, [this]() {
						return !this->que.empty() || this->endwork == true;
						});
					if (this->que.empty() && endwork == true) {
						return;
				}
					task = std::move(this->que.front());//Закинул задачу из очереди в функцию таск
					this->que.pop();
				}
				task();
			}
			});
}
}
void ThreadPool::add(std::function<void()>task) {
	{
	std::lock_guard<std::mutex>myLock(this->mymtx);
	this->que.push(std::move(task));
	}
	cv.notify_one();//Один из фоновых потоков проснется и заберет задачу
}
ThreadPool::~ThreadPool() {
	{
		std::lock_guard<std::mutex>myLock(this->mymtx);
		this->endwork = true;
	}
	cv.notify_all();
	for (std::thread& worker : this->myvec) {
		if (worker.joinable()) {
			worker.join();
		}
}
}