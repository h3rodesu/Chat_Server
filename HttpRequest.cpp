#include"HttpRequest.h"
void Pars::parse(const char* buffer, size_t size) {
	for (size_t i=0; i < size; i++) {
		char c = buffer[i];
		switch (current_stat) {
		case Status::METHOD:
			if (c == ' ') {
				current_stat = Status::PATH;
			}
			else {
				method.push_back(c);
			}
			break;
		case Status::PATH:
			if (c == ' ') {
				current_stat = Status::VERSION;
			}
			else {
				path.push_back(c);
			}
			break;
		case Status::VERSION:
			if (c == '\r') {
				//тут скип т.е. игнор т.к. версия последняя в строке и ждем \n
			}
			else if (c == '\n') {
				current_stat = Status::FIRST_STRING_DONE;
			}
			else {
				version.push_back(c);
			}
			break;
		case Status::FIRST_STRING_DONE:
			if (c == '\r') {
				//тут так же игнор,ничего не происходит,ждем \n
		}
			else if (c == '\n') {
				current_stat = Status::COMPLETE;
			}
			else {
				key.push_back(c);//Чтобы добавилась первая буква
				current_stat = Status::HEAD_KEY;
			}
			break;
		case Status::HEAD_KEY:
			if (c == ':') {
				current_stat = Status::HEAD_VALUE;
			}
			else {
				key.push_back(c);
			}
			break;
		case Status::HEAD_VALUE:
			if (c == '\r') {
				//игнор+скип
			}
			else if (c == '\n') {
				if (!value.empty() && value[0] == ' ') {//т.е. если первый символ в value-пробел
					value.erase(0, 1);//Удалить один символ в нулевом индексе
				}
				headers[key] = value;//Просто запись в мапу 
				key.clear();//Чистим временные переменные
				value.clear();//т.к. будут еще значения
				current_stat = Status::FIRST_STRING_DONE;
			}
			else {
				value.push_back(c);
			}
			break;
		case Status::COMPLETE:
			return;
		}
	}
}