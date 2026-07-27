#pragma once
#include "SafeSocket.h"
#include<thread>
#include<mutex>
#include <iostream>
#include <condition_variable>
#include<map>
#include<string>
class ChatServer {
private:
	SafeSocket mysocket;
	std::mutex mtx;
	std::map<SOCKET,std::string> Map;
	int myport;//Порт-просто число
public:
	ChatServer(int port);
	bool init();//Инициализация winsock,bind и listen
void start();//цикл для ацептов
	void handlClient(SafeSocket clientSocket);//ПРиём сокетов клиентов через мув чтобы закинуть в поток пула
	~ChatServer();
};