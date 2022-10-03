#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <thread>
#include <string>
#include "C:\Users\User\source\repos\OS_Vkr\ServerProccess.h"

using namespace std;



int main()
{
	setlocale(0, "Rus");
	cout << "Консоль сервера для первого процесса , добро пожаловать !" << endl;
	cout << "-------------------------------------------------------------------" << endl;
	ServerProccess* serverProccessOne = new ServerProccess("\\\\.\\pipe\\StatusServer1", "\\\\.\\pipe\\ProccessOne", { SM_CMOUSEBUTTONS, SM_MOUSEWHEELPRESENT }, { "SM_CMOUSEBUTTONS", "SM_MOUSEWHEELPRESENT" });
	serverProccessOne->startServer();
	delete serverProccessOne;
	system("pause");

}
