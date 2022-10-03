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
	cout << "Консоль сервера для второго процесса , добро пожаловать !" << endl;
	cout << "-------------------------------------------------------------------" << endl;
	ServerProccess* serverProccessTwo = new ServerProccess("\\\\.\\pipe\\StatusServer2", "\\\\.\\pipe\\ProccessTwo");
	serverProccessTwo->startServer();
	delete serverProccessTwo;
	system("pause");
}

