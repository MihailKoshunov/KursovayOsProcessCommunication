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
	/*������������ ��� �������� ���������������� ������� ������� */
protected: HANDLE ClientPipeFileProcessOne = NULL;
		   /* ������������ ��� �������� ���������������� ������� �������*/
protected: HANDLE ClientPipeFileProcessTwo = NULL;

		   /*���������� ��������� � ������� ������� */
protected: HANDLE ClientPipeSendToServerProcessOne = NULL;

		   /* ���������� ��������� � ������� ������� */
protected: HANDLE ClientPipeSendToServerProcessTwo = NULL;
		   /* �������� ����������� */
protected: int CheckServerConnection[2];

public:

#pragma �������� �� ����������� � ������� �������
	void CheckServerOne()
	{
		string text = "";
		char buffer[1024];
		DWORD nBytesRead;
		int i = 0;
		while (true)
		{
			this_thread::sleep_for(chrono::milliseconds(1000));
			/*�������� ������*/
			ClientPipeFileProcessOne =
				CreateFile(L"\\\\.\\pipe\\ProccessOne",
					GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			/*���� ����� ��������� ���� �� �� ������������� ������� */
			if (ClientPipeFileProcessOne != INVALID_HANDLE_VALUE)
			{
				/* ������� ������ ���������� ����� � �������� ������
				���� �������������� ������ � ����. */
				BOOL FlushFileBufferProcess1 =
					FlushFileBuffers(ClientPipeFileProcessOne);
				if (FlushFileBufferProcess1 != FALSE)
				{
					/* ������ ���� */
					if (ReadFile(ClientPipeFileProcessOne, &buffer,
						sizeof(buffer), &nBytesRead, NULL) != 0)
					{
						text = buffer;
						if (text == "server_live")
						{
							if (i == 0)
							{
								cout << "����������� ������� ������� ��������� ������� !" << endl;
									i = 1;
							}
							else if (i == -1)
							{
								cout << "���������� ������������� � ������ �������� !" << endl;
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
					cout << "���������� � ������ �������� ��������"
						<< endl;
				}
				CheckServerConnection[0] = -1;
				this_thread::sleep_for(chrono::milliseconds(30000));
			}
		}
	}

#pragma �������� �� ����������� �� ������� �������
	void CheckServerTwo()
	{
		string text = "";
		int i = 0;
		char buffer[1024];
		DWORD nBytesRead;
		while (true)
		{

			this_thread::sleep_for(chrono::milliseconds(1200));
			/* ������ ���� */
			ClientPipeFileProcessTwo =
				CreateFile(L"\\\\.\\pipe\\ServerProccess2",
					GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			/* ���� ���� ������ */
			if (ClientPipeFileProcessTwo != INVALID_HANDLE_VALUE)
			{
				/* ������� ������ ���������� ����� � �������� ������
���� �������������� ������ � ����. */
				BOOL FlushFileBufferProcess2 =
					FlushFileBuffers(ClientPipeFileProcessTwo);
				if (FlushFileBufferProcess2 != FALSE)
				{
					/* ������ ���� */
					if (ReadFile(ClientPipeFileProcessTwo, &buffer,
						sizeof(buffer), &nBytesRead, NULL) != 0)
					{
						text = buffer;
						if (text == "server_live")
						{
							if (i == 0)
							{
								cout << "����������� ������� ������� ��������� ������� !" 
									<< endl;
								i = 1;
							}
							else if (i == -1)
							{
								i = 1;
								cout << "���������� ������������� �� ������ �������� !" << endl;
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
					cout << "���������� �� ������ �������� �������� !" << endl;
				}
				CheckServerConnection[1] = -1;
				this_thread::sleep_for(chrono::milliseconds(30000));
			}
		}
	}

#pragma ���������� ������ ������� ������ �������� �� ������� ������� (��������)
	void ReadMessageAnswerFromProcessOne()
	{
		string text = "";
		char buffer[1024];
		DWORD nBytesRead;
		/* ������� ������ ���������� ����� � �������� ������ ����
�������������� ������ � ����. */
		BOOL FlushFileBufferProcessOne =
			FlushFileBuffers(ClientPipeSendToServerProcessOne);
		cout << endl;
		if (FlushFileBufferProcessOne != FALSE)
		{
			cout << "������ �� ������� �������� " << endl;
			/* ������ ���� */
			while (ReadFile(ClientPipeSendToServerProcessOne, &buffer,
				sizeof(buffer), &nBytesRead, NULL) != 0)
			{
				text = buffer;
				cout << text << endl;
				this_thread::sleep_for(chrono::milliseconds(3000));
			}
			cout << "������ �������� ������� !" << endl;
		}
		DisconnectNamedPipe(ClientPipeSendToServerProcessOne);
		CloseHandle(ClientPipeSendToServerProcessOne);
		MenuClient();
	}

#pragma �������� ������� �� ������ ������� ������ � ������ ���������
	void SendToServerOneProcess()
	{
		string text = "";
		char buffer[1024];
		DWORD nBytesRead;
		if (CheckServerConnection[0] == 1)
		{
			/* ������ ���� */
			ClientPipeSendToServerProcessOne =
				CreateFile(L"\\\\.\\pipe\\StatusServer1",
					GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (ClientPipeSendToServerProcessOne != INVALID_HANDLE_VALUE)
			{
				strcpy_s(buffer, "send_to_server_info_process_one");
				/* ���������� ���� */
				WriteFile(ClientPipeSendToServerProcessOne, &buffer,
					sizeof(buffer), &nBytesRead, NULL);
				this_thread::sleep_for(chrono::milliseconds(3000));
				ReadMessageAnswerFromProcessOne();
			}
		}
		else
		{
			cout << "���������� �������� ������ �� ������� �������"
				<< endl;
			MenuClient();
		}
	}

#pragma �������� ������� �� ������ ������� ������ � ������ ���������
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
			cout << "������ �� ������� �������� " << endl;
			while (ReadFile(ClientPipeSendToServerProcessTwo, &buffer,
				sizeof(buffer), &nBytesRead, NULL) != 0)
			{
				text = buffer;
				cout << text << endl;
				this_thread::sleep_for(chrono::milliseconds(3000));
			}
			cout << "������ �������� ������� !" << endl;
		}
		DisconnectNamedPipe(ClientPipeSendToServerProcessTwo);
		CloseHandle(ClientPipeSendToServerProcessTwo);
		MenuClient();
	}

#pragma �������� ������� �� ������ ������� ������ �� ������ ���������
	void SendToServerTwoProcess()
	{
		string text = "";
		char buffer[1024];
		DWORD nBytesRead;
		/* ���� ���� ����������� */
		if (CheckServerConnection[1] == 1)
		{
			/* ������ ���� */
			ClientPipeSendToServerProcessTwo =
				CreateFile(L"\\\\.\\pipe\\ServerProccessTwoSend",
					GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (ClientPipeSendToServerProcessTwo != INVALID_HANDLE_VALUE)
			{
				strcpy_s(buffer, "send_to_server_info_process_two");
				/* ���������� ���� */
				WriteFile(ClientPipeSendToServerProcessTwo, &buffer,
					sizeof(buffer), &nBytesRead, NULL);
				this_thread::sleep_for(chrono::milliseconds(3000));
				ReadMessageAnswerFromProcessTwo();
			}
		}
		else
		{
			cout << "���������� �������� ������ �� ������� �������" << endl;
			MenuClient();
		}
	}


#pragma ���� �������
	void MenuClient()
	{
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
			/* ���� ���� ����������� */
			if (CheckServerConnection[0] == 1)
			{
				SendToServerOneProcess();
			}
			else
			{
				cout << "���������� �������� ������ �� ������� �������"
					<< endl;
				MenuClient();
			}
			break;

		case 2:
			/* ���� ���� ����������� */
			if (CheckServerConnection[1] == 1)
			{
				SendToServerTwoProcess();
			}
			else
			{
				cout << "���������� �������� ������ �� ������� �������"
					<< endl;
				MenuClient();
			}
			break;

		case 3:
			exit(0);
			break;

		default:
			cout << "������������ �������� !" << endl;
			cout << endl;
			MenuClient();
			break;
		}
	}

#pragma ������ ������ ��������� 
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


