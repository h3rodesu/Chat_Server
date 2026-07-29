#pragma once
#include "SafeSocket.h"
#include<thread>
#include<mutex>
#include <iostream>
#include <condition_variable>
#include<map>
#include<string>
#include"ThreadPool.h"
class ChatServer {
private:
	SafeSocket mysocket;
	std::mutex mtx;
	std::map<SOCKET,std::string> Map;
	int myport;//Порт-просто число
	ThreadPool pool;
public:
	ChatServer(int port);
	bool init();//Инициализация winsock,bind и listen
void start();//цикл для ацептов,главный поток-хостес,получил клиентский сокет-завернул в лямбду и передал в фоновый поток,ТОЛЬКО ОЖИДАНЕИ И ПРИЕМ КЛИЕНТОВ
	void handlClient(std::shared_ptr<SafeSocket>mySocket);//ПРиём сокетов клиентов через мув чтобы закинуть в поток пула,фоновй поток внутри пула
	~ChatServer();
};