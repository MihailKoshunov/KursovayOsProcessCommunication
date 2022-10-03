#define STRICT
#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <thread>
#include <future>
#include <fstream>
#include <string>
#include <winbase.h>
#include <winuser.h>
#include <vector>

using namespace std;


class Client
{
	/*Используется для проверки функционирования первого сервера */
protected: HANDLE ClientPipeFileProcessOne = NULL;
		   /* Используется для проверки функционирования второго сервера*/
protected: HANDLE ClientPipeFileProcessTwo = NULL;

		   /*Отправляет сообщение к первому серверу */
protected: HANDLE ClientPipeSendToServerProcessOne = NULL;

		   /* Отправляет сообщение к второму серверу */
protected: HANDLE ClientPipeSendToServerProcessTwo = NULL;
		   /* Проверка подключения */
protected: int CheckServerConnection[2];

public:

#pragma Проверка на подключение к первому серверу
	void CheckServerOne()
	{
		string text = "";
		char buffer[1024];
		DWORD nBytesRead;
		int i = 0;
		while (true)
		{
			this_thread::sleep_for(chrono::milliseconds(1000));
			/*Создаётся файлик*/
			ClientPipeFileProcessOne =
				CreateFile(L"\\\\.\\pipe\\ProccessOne",
					GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			/*Если можно прочитать файл то он удовлетворяет условию */
			if (ClientPipeFileProcessOne != INVALID_HANDLE_VALUE)
			{
				/* Очищает буферы указанного файла и вызывает запись
				всех буферизованных данных в файл. */
				BOOL FlushFileBufferProcess1 =
					FlushFileBuffers(ClientPipeFileProcessOne);
				if (FlushFileBufferProcess1 != FALSE)
				{
					/* Читает файл */
					if (ReadFile(ClientPipeFileProcessOne, &buffer,
						sizeof(buffer), &nBytesRead, NULL) != 0)
					{
						text = buffer;
						if (text == "server_live")
						{
							if (i == 0)
							{
								cout << "Подключение первого сервера произошла успешно !" << endl;
									i = 1;
							}
							else if (i == -1)
							{
								cout << "Соединение восстановлено с первым сервером !" << endl;
									i = 1;
							}
							CheckServerConnection[0] = 1;
						}
					}
				}
				CloseHandle(ClientPipeFileProcessOne);
				this_thread::sleep_for(chrono::milliseconds(30000));
			}
			else
			{
				i = -1;
				if (i == -1)
				{
					cout << "Соединение с первым сервером прервано"
						<< endl;
				}
				CheckServerConnection[0] = -1;
				this_thread::sleep_for(chrono::milliseconds(30000));
			}
		}
	}

#pragma Проверка на подключение ко второму серверу
	void CheckServerTwo()
	{
		string text = "";
		int i = 0;
		char buffer[1024];
		DWORD nBytesRead;
		while (true)
		{

			this_thread::sleep_for(chrono::milliseconds(1200));
			/* Создаёт файл */
			ClientPipeFileProcessTwo =
				CreateFile(L"\\\\.\\pipe\\ServerProccess2",
					GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			/* Если есть доступ */
			if (ClientPipeFileProcessTwo != INVALID_HANDLE_VALUE)
			{
				/* Очищает буферы указанного файла и вызывает запись
всех буферизованных данных в файл. */
				BOOL FlushFileBufferProcess2 =
					FlushFileBuffers(ClientPipeFileProcessTwo);
				if (FlushFileBufferProcess2 != FALSE)
				{
					/* Читает файл */
					if (ReadFile(ClientPipeFileProcessTwo, &buffer,
						sizeof(buffer), &nBytesRead, NULL) != 0)
					{
						text = buffer;
						if (text == "server_live")
						{
							if (i == 0)
							{
								cout << "Подключение второго сервера произошла успешно !" 
									<< endl;
								i = 1;
							}
							else if (i == -1)
							{
								i = 1;
								cout << "Соединение восстановлено со вторым сервером !" << endl;
							}
							CheckServerConnection[1] = 1;
						}
					}
				}
				CloseHandle(ClientPipeFileProcessTwo);
				this_thread::sleep_for(chrono::milliseconds(30000));
			}
			else {
				i = -1;
				if (i == -1)
				{
					cout << "Соединение со вторым сервером прервано !" << endl;
				}
				CheckServerConnection[1] = -1;
				this_thread::sleep_for(chrono::milliseconds(30000));
			}
		}
	}

#pragma Отображает данные которые клиент получает от первого сервака (процесса)
	void ReadMessageAnswerFromProcessOne()
	{
		string text = "";
		char buffer[1024];
		DWORD nBytesRead;
		/* Очищает буферы указанного файла и вызывает запись всех
буферизованных данных в файл. */
		BOOL FlushFileBufferProcessOne =
			FlushFileBuffers(ClientPipeSendToServerProcessOne);
		cout << endl;
		if (FlushFileBufferProcessOne != FALSE)
		{
			cout << "Данные от первого процесса " << endl;
			/* Читает файл */
			while (ReadFile(ClientPipeSendToServerProcessOne, &buffer,
				sizeof(buffer), &nBytesRead, NULL) != 0)
			{
				text = buffer;
				cout << text << endl;
				this_thread::sleep_for(chrono::milliseconds(3000));
			}
			cout << "Данные переданы успешно !" << endl;
		}
		DisconnectNamedPipe(ClientPipeSendToServerProcessOne);
		CloseHandle(ClientPipeSendToServerProcessOne);
		MenuClient();
	}

#pragma Отправка команды на сервер который связан с первым процессом
	void SendToServerOneProcess()
	{
		string text = "";
		char buffer[1024];
		DWORD nBytesRead;
		if (CheckServerConnection[0] == 1)
		{
			/* Создаёт файл */
			ClientPipeSendToServerProcessOne =
				CreateFile(L"\\\\.\\pipe\\StatusServer1",
					GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (ClientPipeSendToServerProcessOne != INVALID_HANDLE_VALUE)
			{
				strcpy_s(buffer, "send_to_server_info_process_one");
				/* Записывает файл */
				WriteFile(ClientPipeSendToServerProcessOne, &buffer,
					sizeof(buffer), &nBytesRead, NULL);
				this_thread::sleep_for(chrono::milliseconds(3000));
				ReadMessageAnswerFromProcessOne();
			}
		}
		else
		{
			cout << "Невозможно получить данные из первого сервера"
				<< endl;
			MenuClient();
		}
	}

#pragma Отправка команды на сервер который связан с вторым процессом
	void ReadMessageAnswerFromProcessTwo()
	{
		string text = "";
		char buffer[1024];
		DWORD nBytesRead;
		BOOL FlushFileBufferProcessTwo =
			FlushFileBuffers(ClientPipeSendToServerProcessTwo);
		cout << endl;
		if (FlushFileBufferProcessTwo != FALSE)
		{
			cout << "Данные от второго процесса " << endl;
			while (ReadFile(ClientPipeSendToServerProcessTwo, &buffer,
				sizeof(buffer), &nBytesRead, NULL) != 0)
			{
				text = buffer;
				cout << text << endl;
				this_thread::sleep_for(chrono::milliseconds(3000));
			}
			cout << "Данные переданы успешно !" << endl;
		}
		DisconnectNamedPipe(ClientPipeSendToServerProcessTwo);
		CloseHandle(ClientPipeSendToServerProcessTwo);
		MenuClient();
	}

#pragma Отправка команды на сервер который связан со вторым процессом
	void SendToServerTwoProcess()
	{
		string text = "";
		char buffer[1024];
		DWORD nBytesRead;
		/* Если есть подключение */
		if (CheckServerConnection[1] == 1)
		{
			/* Создаёт файл */
			ClientPipeSendToServerProcessTwo =
				CreateFile(L"\\\\.\\pipe\\ServerProccessTwoSend",
					GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (ClientPipeSendToServerProcessTwo != INVALID_HANDLE_VALUE)
			{
				strcpy_s(buffer, "send_to_server_info_process_two");
				/* Записывает файл */
				WriteFile(ClientPipeSendToServerProcessTwo, &buffer,
					sizeof(buffer), &nBytesRead, NULL);
				this_thread::sleep_for(chrono::milliseconds(3000));
				ReadMessageAnswerFromProcessTwo();
			}
		}
		else
		{
			cout << "Невозможно получить данные из второго сервера" << endl;
			MenuClient();
		}
	}


#pragma Меню клиента
	void MenuClient()
	{
		this_thread::sleep_for(chrono::milliseconds(2000));
		int ChouseOperation = 0;
		cout << "" << endl;
		cout << "Добро пожаловать в клиентское приложение " << endl;
		cout << "Получить информацию от первого сервера - 1" << endl;
		cout << "Получить информацию от второго сервера - 2" << endl;
		cout << "Выйти из приложения - 3 " << endl;
		cout << "Выберите операцию: ";
		cin >> ChouseOperation;
		switch (ChouseOperation)
		{

		case 1:
			/* Если есть подключение */
			if (CheckServerConnection[0] == 1)
			{
				SendToServerOneProcess();
			}
			else
			{
				cout << "Невозможно получить данные из первого сервера"
					<< endl;
				MenuClient();
			}
			break;

		case 2:
			/* Если есть подключение */
			if (CheckServerConnection[1] == 1)
			{
				SendToServerTwoProcess();
			}
			else
			{
				cout << "Невозможно получить данные из второго сервера"
					<< endl;
				MenuClient();
			}
			break;

		case 3:
			exit(0);
			break;

		default:
			cout << "Недопустимая операция !" << endl;
			cout << endl;
			MenuClient();
			break;
		}
	}

#pragma Начала работы программы 
	void ThreadForClient()
	{
		thread ServerOne(&Client::CheckServerOne, this);
		//thread ServerTwo(&Client::CheckServerTwo, this);
		thread Client(&Client::MenuClient, this);
		ServerOne.join();
		//ServerTwo.join();
		Client.join();
	}

};


int main()
{

	setlocale(0, "Rus");
	Client WorkClient;
	WorkClient.ThreadForClient();
	system("pause");

}


