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


class Client {


	private:
		/* ������������ ��� ���� ��� ������ �������� �� ������ ��� ��� */
		HANDLE clientPipeStatusServer;

		/* �������� � ����� �� ��������� */
		HANDLE clientPipeSendMessageServerProccess;

		//��� ������������ ��������� � ������� ������� !
		char bufferPipeStatus[1024];

		//��������� ���������
		char bufferPipeSendMessToProcces[1024];

		vector<string> namesPipeStatusServer;
		vector<string> namePipeProccess;




		mutex muSectionStatus;

	private:

		void ServerStatus(const char *str,string nameServer) {
			wchar_t* namePipe = new wchar_t[strlen(str) + 1];
			mbstowcs_s(NULL, namePipe, strlen(str) + 1, str, strlen(str) + 1);
			int i = 0;
			DWORD nBytesRead;
			while (true) {
				muSectionStatus.lock();
				this->clientPipeStatusServer = CreateFile(namePipe,
					GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					try
					{
						BOOL flushBufferStatusServer = FlushFileBuffers(clientPipeStatusServer);
						if (ReadFile(clientPipeStatusServer, &bufferPipeStatus, sizeof(bufferPipeStatus), &nBytesRead, NULL) != 0) {
							string commandStatus = bufferPipeStatus;
							if (i == 0) {
								cout << nameServer << " ��������� !" << endl;
								i++;
							}
						}
					}
					catch (const std::exception& ex)
					{
						cout << "������: " << ex.what() << endl;
						cout << nameServer << "��������" << endl;
					}
				muSectionStatus.unlock();
				this_thread::sleep_for(chrono::milliseconds(75));
			}

		}




		void ServerCommandSender(string needInfo, const char *namePipeProccess) {
			DWORD nBytesRead;
			try {
				wchar_t* namePipe = new wchar_t[strlen(namePipeProccess) + 1];
				mbstowcs_s(NULL, namePipe, strlen(namePipeProccess) + 1, namePipeProccess, strlen(namePipeProccess) + 1);
				clientPipeSendMessageServerProccess = CreateFile(namePipe,
					GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (clientPipeSendMessageServerProccess != INVALID_HANDLE_VALUE)
					{
						strcpy_s(bufferPipeSendMessToProcces, needInfo.c_str());
						/* ���������� ���� */
						WriteFile(clientPipeSendMessageServerProccess, &bufferPipeSendMessToProcces,
						sizeof(bufferPipeSendMessToProcces), &nBytesRead, NULL);
						//GetMessageFromServer();
					}
			}
			catch (const std::exception& ex) {
				cout << "������: " << ex.what() << endl;
			}
			GetMessageFromServer(needInfo);
		}


		void GetMessageFromServer(string needInfo) {
			DWORD nBytesRead;
			BOOL FlushFileBufferProcess = FlushFileBuffers(this->clientPipeSendMessageServerProccess);
			cout << endl;
			if (FlushFileBufferProcess != FALSE) {
				if (needInfo == "send_proccess_one") {
					cout << "������ �� ������� �������: " << endl;
				}
				else if (needInfo == "send_proccess_two") {
					cout << "������ �� ������� �������: " << endl;

				}		
				while (ReadFile(this->clientPipeSendMessageServerProccess, &this->bufferPipeSendMessToProcces, sizeof(this->bufferPipeSendMessToProcces), &nBytesRead, NULL) != 0)
				{
					string text = this->bufferPipeSendMessToProcces;
					cout << text << endl;
					this_thread::sleep_for(chrono::milliseconds(3000));
				}
				cout << "����� ������" << endl;
			}
			DisconnectNamedPipe(this->clientPipeSendMessageServerProccess);
		}



		void Menu() {
			this_thread::sleep_for(chrono::milliseconds(2000));
			int ChouseOperation = 0;
			cout << "" << endl;
			cout << "����� ���������� � ���������� ���������� " << endl;
			cout << "�������� ���������� �� ������� ������� - 1" << endl;
			cout << "�������� ���������� �� ������� ������� - 2" << endl;
			cout << "����� �� ���������� - 3 " << endl;
			cout << "�������� ��������: ";
			cin >> ChouseOperation;
			switch (ChouseOperation)
			{

			case 1:
				ServerCommandSender("send_proccess_one", namePipeProccess[0].c_str());
				Menu();
				break;

			case 2:
				ServerCommandSender("send_proccess_two", namePipeProccess[1].c_str());
				Menu();
				break;

			case 3:
				exit(0);
				break;

			default:
				cout << "������������ �������� !" << endl;
				cout << endl;
				Menu();
				break;
			}
		}

	




	public:


		Client(vector<string> namesPipeStatusServer, vector<string> namePipeProccess) {
			this->namesPipeStatusServer = namesPipeStatusServer;
			this->namePipeProccess = namePipeProccess;
		}


		void installClient() {
			thread serverIsAliveOne(&Client::ServerStatus,this,namesPipeStatusServer[0].c_str(),"������ ����� ����");
			thread serverIsAliveTwo(&Client::ServerStatus, this, namesPipeStatusServer[1].c_str(),"������ ����� ���");
			thread mainThreadClient(&Client::Menu,this);
			serverIsAliveOne.join();
			serverIsAliveTwo.join();
		}


};



int main()
{
	setlocale(0, "Rus");

	vector <string> namesPipeAdressStatusServers = {"\\\\.\\pipe\\StatusServer1","\\\\.\\pipe\\StatusServer2" };
	vector <string> namesPipeAdress = { "\\\\.\\pipe\\ProccessOne", "\\\\.\\pipe\\ProccessTwo" };
	Client* currentUser = new Client(namesPipeAdressStatusServers, namesPipeAdress);
	currentUser->installClient();

	system("pause");

}
