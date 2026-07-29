#include "ChatServer.h"
#include<memory>	
ChatServer::ChatServer(int s): myport(s),mysocket(INVALID_SOCKET),pool(4){}//4 потока
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
		SOCKET ptimeclientSocket;//Голый сокет без дескриптора

		ptimeclientSocket = accept(mysocket.get(), (sockaddr*)&clientAddr, (&clientAddrSize));//Появление дескриптора
		if (ptimeclientSocket == INVALID_SOCKET|| ptimeclientSocket==0) {
			continue;//в случае ошибки возврат в начало while к новым подключеяим без ошибок
		}
		SafeSocket clientSocket(ptimeclientSocket);//Если ошибки выше нет то перенос сокета с дескриптором в объект класса SafeSocket чтобы работал принцип RAII
		std::cout << "Клиент на сервере" << std::endl;
		auto unclientsocket = std::make_shared<SafeSocket>(std::move(clientSocket));//function отказывался принимать лямбду с некопируемым сокетом так что пришлось идти на крайние меры
		pool.add([this,unclientsocket=std::move(unclientsocket)]() mutable/*чтобы в лямбде не было const*/ noexcept{//Закинуть сокет клиента в очередь как задачу
			this->handlClient(unclientsocket);//достали сокет из указателя через разименование
			});
	}
}
void ChatServer::handlClient(std::shared_ptr<SafeSocket>mySocket) {//Фоновый поток для чтения через метод recv
	while (true) {//Работает только когда есть клиентсокет,активное ожидание исключено
		char Rxbuffer[1024];
		int recBytes = recv(mySocket->get(), Rxbuffer, sizeof(Rxbuffer) - 1, 0);//-1 под нуль терминатор,0 т.к. 0 флагов
		if (recBytes > 0) {
			Rxbuffer[recBytes] = '\0';//огранчение строки
			std::cout << "Получены байты " << Rxbuffer<< std::endl;
		}
		else if (recBytes == 0) {//Если ПРИНЯТО 0 байтов-клиент отключился
			std::cout << "Клиент отключился " << std::endl;
			break;
		}
		else {
			std::cerr << WSAGetLastError() << std::endl;
			break;
		}
	}
}
ChatServer::~ChatServer() {
	std::cout << "[WINSOCK] Сетевая библиотека удалена " << std::endl;
}
