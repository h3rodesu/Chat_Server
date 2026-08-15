#include<iostream>
#include<string>
#include<thread>
#include<mutex>
#include<WinSock2.h>
#include<WS2tcpip.h>
#pragma comment(lib,"WS2_32.lib")
//Сделать класс для recv отдельный
class Receive {
public:
	int recBytes;
	char rxBuffer[1024];
	std::string receive(SOCKET smth) {
		recBytes = recv(smth, rxBuffer, sizeof(rxBuffer) - 1, 0);
		if (recBytes > 0) {
			rxBuffer[recBytes] = '\0';
			std::cout << rxBuffer << std::endl;//Добавил т.к. не было вывода
			return rxBuffer;//Да,возвр.значение стринг а вернет чар
		}
		else {
			std::cout << "Error of receive" << WSAGetLastError() << std::endl;
			return "";//Вместо чар вернет пустоту при ошибке
		}

	}
	~Receive() {}
};
int main() {
	std::mutex mtx;
	Receive read;
	//Подготовка-инициализация
	WSADATA wsa;//Адрес
	WSAStartup(MAKEWORD(2, 2), &wsa);//Инициализация
	//Создание сокета
	SOCKET UserSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (UserSocket == INVALID_SOCKET) {//не уверен что нужна проверка т.к. она по идее выполняется на сервере
		std::cerr << WSAGetLastError() << std::endl;
		closesocket(UserSocket);
		return 1;
	}
	sockaddr_in clientADDR;//Хранит адрес сервера,здесь настраивается адрес СЕРВЕРА
	clientADDR.sin_family = AF_INET;
	clientADDR.sin_port = htons(8080);
	if (inet_pton(AF_INET, "127.0.0.1", &clientADDR.sin_addr) <= 0) {
		std::cerr << "ERROR!NON VALID IP!" << std::endl;
	}
	else {
		std::cout << "inet_pton completed the task" << std::endl;
	}
	if (connect(UserSocket, (sockaddr*)&clientADDR, sizeof(clientADDR)) < 0) {
		std::cerr << "WARNING!CONNECTION ERROR!" << WSAGetLastError() << std::endl;

	}
	else {
		std::cout << "Successful connection!" << std::endl;
		read.receive(UserSocket);//Принимается строка welcome
		std::string nickname;
		std::cin >> std::ws;
		std::getline(std::cin, nickname);
		//nickname += "\n";
		send(UserSocket, nickname.c_str(), (int)nickname.size(), 0);
	}
	std::thread learn([UserSocket, &read] {
		while (true) {
			std::string incmsg = read.receive(UserSocket);
			if (!incmsg.empty()) {
			std::cout << incmsg << std::endl;
			}
			else {
				std::cout << "Lost connection with server...disconnect" << std::endl;
				break;
			}
		}
		});
	learn.detach();//нужно отвязать поток
	while (true) {
		std::string message;
		std::getline(std::cin, message);
		send(UserSocket, message.c_str(), (int)message.size(), 0);
	}
};
