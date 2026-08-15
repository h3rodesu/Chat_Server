#pragma once
#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#pragma comment(lib,"WS2_32.lib")

class SafeSocket {
private:
	SOCKET mysocket;
public:
	SafeSocket(SOCKET s) : mysocket(s) {}
	SafeSocket(const SafeSocket&) = delete;//Запрет на создание объекта как копии
	SafeSocket& operator=(const SafeSocket&) = delete;//Запрет на копирование данных из одного
	//Объекта в другой
	//Присваивание
	SafeSocket(SafeSocket&& other)noexcept { 
			mysocket = other.mysocket;
			other.mysocket = INVALID_SOCKET;
		}
		//Перемещение
		SafeSocket&operator=(SafeSocket&&other) noexcept{//Оператор перемещения
			if (this != &other) {//Проверка не равен ли объект сам себе
				if (mysocket != INVALID_SOCKET) {
					closesocket(mysocket);//Если старый объект не пустой закрыть его
				}
					mysocket = other.mysocket;//Перекидываем данные
					other.mysocket = INVALID_SOCKET;//Зануление
				}
				return	*this;
			}
//Деструктор(соблюдение принципа RAII)
		~SafeSocket() {
			if (mysocket != INVALID_SOCKET) {
		closesocket(mysocket);
		this->mysocket = INVALID_SOCKET;
		std::cout << "Сокет автоматически удалён" << std::endl;
			}
}
		//Геттер чтобы можно было вызывать необходимы функции
		SOCKET get() const{
			return mysocket;
		}
};