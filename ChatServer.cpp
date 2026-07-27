#include "ChatServer.h"
ChatServer::ChatServer(int s): myport(s),mysocket(INVALID_SOCKET){}
bool ChatServer::init() {
	WSADATA wsa;//Тут адрес 
	int result = WSAStartup(MAKEWORD(2, 2), &wsa);//Инициализация из сети
	if (result != 0) {//т.е. проблемы с запуском
		std::cerr << "WARNING! WINSOCK НЕ ЗАПУЩЕН" << std::endl;
		return false;//тк тип bool единица была бы равна тру
	}
	else {
		std::cout << "[WINSOCK] Успешно запущен" << std::endl;
	}
	//Дескриптор сокета
	SOCKET listensocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listensocket == INVALID_SOCKET) {
		std::cerr << WSAGetLastError() << std::endl;
		WSACleanup();
		return false;
	}
	mysocket = SafeSocket(listensocket);//Перенос сокета
//Заполнение структуры адреса и привязка сокета к порту 
	sockaddr_in serverADDR{};
	serverADDR.sin_family = AF_INET;//протокол IPv4
	serverADDR.sin_port = htons(myport);//Порт 
	serverADDR.sin_addr.s_addr = INADDR_ANY;//Слушает любой вхлдящий айпи адрес
	if(bind(mysocket.get(),(sockaddr*)&serverADDR,sizeof(serverADDR))==SOCKET_ERROR){//Проверка(если порт недоступен)
		std::cerr <<"[SERVER] ОШИБКА"<<myport<<" Занят другой программой " << WSAGetLastError() << std::endl;
		WSACleanup();
		return false;
	}
	else {
		std::cout << "Привязка к порту " << myport << " прошла успешно" << std::endl;
	}
	if (listen(mysocket.get(), SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "[SERVER] ОШИБКА LISTEN" << WSAGetLastError() << std::endl;
	}
	else {
		std::cout << "[SERVER] ШЛЮЗ ОТКРЫТ" << std::endl;
	}
	return true;//bool тип требует вернуть значение
}
void ChatServer::start() {//Работа с клиентом
	while (true) {
		sockaddr_in clientAddr{};//{}чтобы не было системного мусора
		int clientAddrSize = sizeof(clientAddr);
		SOCKET rawclientSocket;//Голый сокет без дескриптора

		rawclientSocket = accept(mysocket.get(), (sockaddr*)&clientAddr, (&clientAddrSize));//Появление дескриптора
		if (rawclientSocket == INVALID_SOCKET) {
			continue;//в случае ошибки возврат в начало while к новым подключеяим без ошибок
		}
		SafeSocket clientSocket(rawclientSocket);//Если ошибки выше нет то перенос сокета с дескриптором в объект класса SafeSocket чтобы работал принцип RAII
		std::cout << "Клиент на сервере" << std::endl;

		char rxBuffer[1024];//Буфер приёма
		int BytesRec = recv(clientSocket.get(), rxBuffer, sizeof(rxBuffer) - 1, 0);
		if (BytesRec > 0) {
			rxBuffer[BytesRec] = '\0';
			std::cout << "Приняты байты " << rxBuffer << std::endl;
		}
		else if (BytesRec == 0) {//0 байт поступает только если клиент отключился
			std::cout << "Клиент отключился " << std::endl;
		}
		else {
			std::cerr << WSAGetLastError() << std::endl;
		}
	}
}
ChatServer::~ChatServer() {
	WSACleanup();
	std::cout << "[WINSOCK] Сетевая библиотека удалена " << std::endl;
}
