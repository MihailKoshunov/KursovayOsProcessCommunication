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


	//�������� ����������
	private:
		/* ����������� ����� ������� �������� ������� �� ��� ������ ������ ��������  */
		HANDLE pipeSenderServerLive;
		/* ������ ������ � ���������� ����� ������� */
		HANDLE pipeReaderSenderProccess;
		
		/* ����������� ������ ������� */
		wchar_t* namePipeServerStatus;

		/* ����������� ��������� �� ������� � ���������� ����� */
		wchar_t* namePipeProccess;

		/* �������� ������� ��� ����� ��� ������� �������� */
		vector<int> proccessNeed;
		vector<string> proccessNeedText;

		/* ����� ��� �������� ������ � ������� ������� */
		char bufferStatus[1024];
		/* ����� ��� �������� ��������� �� ������� ������� */
		char bufferWriteSend[1024];
		
		/*������� �� ������� */
		string textCommand;
		

	/*  */
	private:

		void sendServerStatusLive() {

			//const wchar_t* wid = wstring(namePipeServerStatus.begin(), namePipeServerStatus.end()).c_str();
			this->pipeSenderServerLive = CreateNamedPipe(namePipeServerStatus, PIPE_ACCESS_DUPLEX,
				PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 0, 0, NMPWAIT_USE_DEFAULT_WAIT, NULL);
			DWORD dwwr;
			cout << "��� �����������" << endl;
			if (pipeSenderServerLive != INVALID_HANDLE_VALUE){ 
				cout << "����������� ��������� ������� !" << endl;
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
							/*  ������ ��� ���� � �����, � buffer ��������� ����� */
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
					/* ������������� � ��������� */
					DisconnectNamedPipe(pipeReaderSenderProccess);
				}
				catch (const std::exception& ex)
				{
					cout << "������: " << ex.what() << endl;
				}
			}

		}

		void infoProccessOne(DWORD dwwr) {
			string text = "";
			cout << "�������� ���������� ������� �������� �� ������� : " << endl;
				for (int i = 0; i < proccessNeed.size(); i++)
				{
					text = proccessNeedText[i] + " ----> "+ to_string(GetSystemMetrics(proccessNeed[i]));
					const char * c = text.c_str();
					strcpy_s(bufferWriteSend, c);
					cout << bufferWriteSend << endl;
					/* ���������� ��� �� � ���� */
					WriteFile(pipeReaderSenderProccess, &bufferWriteSend, sizeof(bufferWriteSend), &dwwr, NULL);
				}
			cout << "��������� ������� �������� ���������� � ������� !" << endl;
		}

		void infoProccessTwo(DWORD dwwr) {
			MEMORYSTATUSEX theStatus;
			theStatus.dwLength = sizeof(theStatus);		
			stringstream text_for_send;
			GlobalMemoryStatusEx(&theStatus);
			cout << "�������� ���������� ������� �������� �� ������� : " << endl; 
			text_for_send << "������ ����� �������� � ������ " << theStatus.ullTotalPageFile / 1024 / 1024 << " MB" << "\n" 
			<< "���������� ��������� ������ ����� �������� " << theStatus.ullAvailPageFile / 1024 / 1024 << " MB";
			string text = text_for_send.str();
			const char * c = text.c_str();
			strcpy_s(bufferWriteSend, c);
			cout << bufferWriteSend << endl;
			WriteFile(pipeReaderSenderProccess, &bufferWriteSend, sizeof(bufferWriteSend), &dwwr, NULL);
			cout << "��������� ������� �������� ���������� � ������� !" << endl;
		}





	/* ������ �������� */
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
