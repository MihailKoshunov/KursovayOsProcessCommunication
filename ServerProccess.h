#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <vector>
#include <thread>
#include <future>
#include <fstream>
#include <string>
#include <winbase.h>
#include <sstream>
#include <winuser.h>
#include <iostream>

using namespace std;


class ServerProccess {


	//Закрытые переменные
	private:
		/* Именованный канал которые отсылает клиенту то что данный сервер работает  */
		HANDLE pipeSenderServerLive;
		/* Читает запрос и отправляет ответ клиенту */
		HANDLE pipeReaderSenderProccess;
		
		/* Отслеживает статус сервака */
		wchar_t* namePipeServerStatus;

		/* Отслеживает сообщения от клиента и возвращает ответ */
		wchar_t* namePipeProccess;

		/* Процессы которые ему нужны для данного процесса */
		vector<int> proccessNeed;
		vector<string> proccessNeedText;

		/* Буфер для хранения команд о статусе сервака */
		char bufferStatus[1024];
		/* Буфер для хранений сообщений со стороны клиента */
		char bufferWriteSend[1024];
		
		/*Команда от клиента */
		string textCommand;
		

	/*  */
	private:

		void sendServerStatusLive() {

			//const wchar_t* wid = wstring(namePipeServerStatus.begin(), namePipeServerStatus.end()).c_str();
			this->pipeSenderServerLive = CreateNamedPipe(namePipeServerStatus, PIPE_ACCESS_DUPLEX,
				PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 0, 0, NMPWAIT_USE_DEFAULT_WAIT, NULL);
			DWORD dwwr;
			cout << "Ждём подключения" << endl;
			if (pipeSenderServerLive != INVALID_HANDLE_VALUE){ 
				cout << "Подключение произошло успешно !" << endl;
				while (true) {			
					BOOL isConnected = ConnectNamedPipe(pipeSenderServerLive,NULL);
					if (isConnected != FALSE) {
						strcpy_s(bufferStatus,"server_live");
						WriteFile(pipeSenderServerLive,&bufferStatus,sizeof(bufferStatus),&dwwr,NULL);
					}
					DisconnectNamedPipe(pipeSenderServerLive);

				}

			}


		}


		void createPipeProccess() {
			this->pipeReaderSenderProccess = CreateNamedPipe(this->namePipeProccess, PIPE_ACCESS_DUPLEX,
				PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
				PIPE_UNLIMITED_INSTANCES, 0, 0, NMPWAIT_USE_DEFAULT_WAIT, NULL);
			DWORD dwwr;
			while (true) {
				try {
					if (ConnectNamedPipe(this->pipeReaderSenderProccess,NULL) != FALSE) {

						if (GetLastError() != ERROR_PIPE_CONNECTED)
						{
							/*  Читает что есть в файле, в buffer находится текст */
							ReadFile(this->pipeReaderSenderProccess, &bufferWriteSend, sizeof(bufferWriteSend), &dwwr, NULL);
							string text = bufferWriteSend;
							if (text == "send_proccess_one") {
								infoProccessOne(dwwr);
							}
							else if (text == "send_proccess_two") {
								infoProccessTwo(dwwr);
							}
						}
					}
					/* Отсоединяемся и закрываем */
					DisconnectNamedPipe(pipeReaderSenderProccess);
				}
				catch (const std::exception& ex)
				{
					cout << "Ошибка: " << ex.what() << endl;
				}
			}

		}

		void infoProccessOne(DWORD dwwr) {
			string text = "";
			cout << "Исходная информация первого процесса от сервера : " << endl;
				for (int i = 0; i < proccessNeed.size(); i++)
				{
					text = proccessNeedText[i] + " ----> "+ to_string(GetSystemMetrics(proccessNeed[i]));
					const char * c = text.c_str();
					strcpy_s(bufferWriteSend, c);
					cout << bufferWriteSend << endl;
					/* Записывает это всё в файл */
					WriteFile(pipeReaderSenderProccess, &bufferWriteSend, sizeof(bufferWriteSend), &dwwr, NULL);
				}
			cout << "Сообщение первого процесса отправлена к клиенту !" << endl;
		}

		void infoProccessTwo(DWORD dwwr) {
			MEMORYSTATUSEX theStatus;
			theStatus.dwLength = sizeof(theStatus);		
			stringstream text_for_send;
			GlobalMemoryStatusEx(&theStatus);
			cout << "Исходная информация второго процесса от сервера : " << endl; 
			text_for_send << "Размер файла подкачки в байтах " << theStatus.ullTotalPageFile / 1024 / 1024 << " MB" << "\n" 
			<< "Количество свободных байтов файла подкачки " << theStatus.ullAvailPageFile / 1024 / 1024 << " MB";
			string text = text_for_send.str();
			const char * c = text.c_str();
			strcpy_s(bufferWriteSend, c);
			cout << bufferWriteSend << endl;
			WriteFile(pipeReaderSenderProccess, &bufferWriteSend, sizeof(bufferWriteSend), &dwwr, NULL);
			cout << "Сообщение второго процесса отправлена к клиенту !" << endl;
		}





	/* Запуск серверка */
	public:

		ServerProccess(const char* namePipeServerStatus, const char* namePipeProccess,vector<int> proccessNeed, vector<string> proccessNeedText) {

			this->namePipeServerStatus = new wchar_t[strlen(namePipeServerStatus) + 1];
			this->namePipeProccess = new wchar_t[strlen(namePipeProccess) + 1];

			mbstowcs_s(NULL, this->namePipeServerStatus, strlen(namePipeServerStatus) + 1, namePipeServerStatus, strlen(namePipeServerStatus) + 1);
			mbstowcs_s(NULL, this->namePipeProccess, strlen(namePipeProccess) + 1, namePipeProccess, strlen(namePipeProccess) + 1);

			this->proccessNeed = proccessNeed;
			this->proccessNeedText = proccessNeedText;

		}

		ServerProccess(const char* namePipeServerStatus, const char* namePipeProccess) {

			this->namePipeServerStatus = new wchar_t[strlen(namePipeServerStatus) + 1];
			this->namePipeProccess = new wchar_t[strlen(namePipeProccess) + 1];

			mbstowcs_s(NULL, this->namePipeServerStatus, strlen(namePipeServerStatus) + 1, namePipeServerStatus, strlen(namePipeServerStatus) + 1);
			mbstowcs_s(NULL, this->namePipeProccess, strlen(namePipeProccess) + 1, namePipeProccess, strlen(namePipeProccess) + 1);

		}


		void startServer() {
			thread eventSenderLiveStatus(&ServerProccess::sendServerStatusLive,this);
			thread messageFromClient(&ServerProccess::createPipeProccess,this);
			eventSenderLiveStatus.join();
			messageFromClient.join();

		}








};
