// Client.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>



// Имя именнованного канала
LPCWSTR NAME_PIPE = L"\\\\.\\pipe\\MyPipeName";

//в дальнейшем это надо вынести в отдельный файл, но, как говорится, "и так сойдет" (временно)
bool tryToAuth(HANDLE hFile)
{
	/*
		должны:
			+	спросить логин
			+	спросить пароль
			+	слепить в строку вида "логин/пароль"
			+	отправить эту строку
			+	дождаться ответа от сервера
			+	если 1 - успех
			+	если 0 - лажа
	*/

	char * login	= new char [50];
	char * password = new char[50];

	//char *buf = new char[MAX_PATH];

	std::cout << "Логин: ";
	std::cin >> login;
	std::cout << "Пароль: ";
	std::cin >> password;

	char buf[100];   

	//лепим строку "логин/пароль"
	strcpy_s(buf, login); 
	strcat_s(buf, "/");
	strcat_s(buf, password);

	DWORD write = NULL;
	// Передача серверу
	if (!WriteFile(hFile,
					buf,
					sizeof(buf) * sizeof(char),
					&write,
					NULL))
	{
		std::cout << "Ошибка WriteFile = " << GetLastError() << std::endl;
		std::cin.get();
		return 0;
	}

	// Прием ответа от сервера
	memset(buf, 0, sizeof(buf)); 
	DWORD read = NULL;
	if (!ReadFile(hFile,
		buf,
		sizeof(buf) / sizeof(char),
		&read,
		NULL))
	{
		std::cout << "Ошибка ReadFile = " << GetLastError() << std::endl;
		std::cin.get();
		return 0;
	}
	buf[read] = '\0'; // полезная штука, последним считанным символом делает концом строки

	std::cout << "Ответ от сервера = " << buf << std::endl;
	char tmp = buf[0];

	if (tmp == '0')
	{
		return false;
	}
	else
	{
		return true;
	}

}



int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "rus");
	WCHAR tmp[MAX_PATH] = {0};
	std::cout << "Введите путь к серверу: ";
	std::wcin >> tmp;
	

	std::wstring mywstring(tmp);
	std::wstring concatted_stdstr = L"\\\\" + mywstring + L"\\pipe\\MyPipeName";

	LPCWSTR NAME_PIPE = concatted_stdstr.c_str();

	// Ожидание подключения к серверу именованного канала 2 секунды
	if (!WaitNamedPipe(NAME_PIPE, 2000))
	{
		std::cout << "Ошибка WaitNamedPipe = " << GetLastError() << std::endl;
		std::cin.get();
		return 0;
	}

	// Открытие именованного канала
	HANDLE hFile = CreateFile(NAME_PIPE,						
								GENERIC_READ | GENERIC_WRITE,
								NULL,							
								NULL,	// Защита (фигня в том, что оно и без защиты у меня работает на одной 
										// машине, но с указанием ее IP, то есть, возможно отработает и по сети на разных машинах)
								OPEN_EXISTING,					
								NULL,							
								NULL);							

	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cout << "Ошибка CreateFile = " << GetLastError() << std::endl;
		std::cin.get();
		return 0;
	}

	while (true)
	{
		std::cout << "\nАвторизация на сервере... \n";
		if (tryToAuth(hFile))
		{
			std::cout << "Авторизация успешна!";
			break;
		}
		else
		{
			std::cout << "Неверный логин или пароль!";
		}
		
	}

	system("pause");

	// Закрытие канала
	CloseHandle(hFile);

	return 0;
}

