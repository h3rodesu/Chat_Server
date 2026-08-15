#include"HttpRequest.h"
void Pars::parse(const char* buffer, size_t size) {
	for (size_t i = 0; i < size; i++) {
		char c = buffer[i];
		switch (current_stat) {
		case Status::COMMAND:
			if (c == '|') {
				if (command == "MSG") {
					current_stat = Status::MSG;
				}
				else if (command == "QUIT") {
					current_stat = Status::QUIT;
				}
				else if (command == "CHANGE_NICK") {
					current_stat = Status::CHANGE_NICK;
				}
			}
			else if (c == '\n' || c == '\r') {
				message = command;
				command = "MSG";
				current_stat = Status::COMPLETE;
				return;
			}
			else {
				command.push_back(c);
			}
			break;
		case Status::MSG:
		case Status::CHANGE_NICK:
		case Status::QUIT:
			if (c == '\r') {
				//скип
			}
			else if (c == '\n') {
				current_stat = Status::COMPLETE;
			}
			else {
				message.push_back(c);
			}
			break;
		case Status::COMPLETE:
			return;
		}
	}
	 if (current_stat == Status::COMMAND && !command.empty()) {//Чтобы можно было писать сообщения без MSG|	
		message = command;
		command = "MSG";
		current_stat = Status::MSG;
			}
}