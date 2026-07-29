#include"ChatServer.h"
#include <iostream>

int main() {
//setlocale(LC_ALL, "Russian");
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	int port;
	std::cout << "Enter your port" << std::endl;
	std::cin >> port;
	ChatServer myServer(port);
	myServer.init();
	if (!myServer.init()) {//Если возниклас ошибка с инициализицей
		std::cerr << "Ошибка инициализации,не удалось запустить сервер" << std::endl;
			system("Pause");
			return 1;
	}
	myServer.start();//Запуск бесконечного цикла
	system("Pause");
	WSACleanup();
	std::cout << "Сетевая бибилотека удалена из ОЗУ" << std::endl;
	return 0;
}