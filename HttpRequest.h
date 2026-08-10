#pragma once
#include<string>
#include<map>
#include<iostream>
enum class Status {
METHOD,
PATH,
VERSION,
FIRST_STRING_DONE,
HEAD_KEY,
HEAD_VALUE,
COMPLETE,
};
class Pars {
private:
	Status current_stat;//Текущее состояние 
	std::string key;//Временныая переменная дял ключа
	std::string value;//временная переменная для значения
public:
	Pars() :current_stat(Status::METHOD){}//По дефолту парсинг начинается с метода
	std::string method;
	std::string path;
	std::string version;
	std::map<std::string, std::string>headers;
	void parse(const char* buffer, size_t size);//Принимает массив символов
	bool isComplete() const	{//Проверка на завершение парсинга
		return current_stat==Status::COMPLETE;
}
	~Pars() {
		std::cout<<"[Parser] has been Deleted"<<std::endl;
}
};
