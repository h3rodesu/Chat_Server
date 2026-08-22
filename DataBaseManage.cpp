#include "DataBaseManage.h"
#pragma warning(disable : 4996)//Чтоюы работал exec_params
//DataBase(const std::string data) :conn(data){}
bool DataBase::signin(const std::string& login, const std::string& password) {
	pqxx::connection connect(this->conn);//Отдельный коннект (если работа бует в разных потоках-для каждого потока свой коннект)
	pqxx::work tx(connect);
	//Нужно проверить логин и пароль 
	pqxx::result res = tx.exec_params("SELECT 1 FROM users WHERE log = $1 AND pass=digest($2,'sha256')", login, password);
	if (!res.empty()) {
		return true;
	}
	//тут клиент выведет что  неправильный логин или пароль
	return false;
}
bool DataBase::registration(const std::string& login, const std::string& password) {
	pqxx::connection connect(this->conn);
	pqxx::work tx(connect);
	pqxx::result logfind = tx.exec_params("SELECT 1 FROM users WHERE log =$1", login);//Поиск логина(юзер заходил)
	if (logfind.empty()) {//Если логин не найден
		tx.exec_params("INSERT INTO users(log,pass) VALUES($1,digest($2,'sha256'))", login, password);
		tx.commit();
		return true;
	}
		return false;
}
void DataBase::changelog(std::string oldnick, std::string newnick) {
	pqxx::connection connect(this->conn);
	pqxx::work tx(connect);
	tx.exec_params("UPDATE users SET log=$1 WHERE log=$2", newnick, oldnick);//позже заменить exec_params
	tx.commit();
}