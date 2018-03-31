// Server.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <ctime>

#include "PipeServer.h"

#define CLIENT_DISCONNECT 109
#define MAX_CONNECTED_USERS 3 
#define PIPE_NAME		L"\\\\.\\pipe\\pipe_example"


// Имя именованного канала
LPCWSTR NAME_PIPE = L"\\\\.\\pipe\\MyPipeName";

// Функция, которая будет в отдельном потоке обслуживать клиента
DWORD WINAPI Thread(LPVOID);



int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "rus");

	std::ifstream file;
	char *FName = new char[MAX_PATH];
	bool serverMode = false;
	std::vector<User<char>> vec;

	std::cout << "\nSTATUS MSG: Сервер запущен. \n\nВведите имя файла с логинами и паролями: ";
	std::cin >> FName;

	file.open(FName);
	if (!file)
	{
		std::cout << "Ошибка чтения файла с именем " << FName << "!" << std::endl;
	}
	else
	{
		std::cout << "Выберите режим работы: \n0) Обычный \n1) Противодействие взлому\n Вариант: ";
		std::cin >> serverMode;


		int users = 0;
		// В цикле прием клиентов
		while (users < MAX_CONNECTED_USERS)
		{
			std::cout << "\nSTATUS MSG: Создан новый pipe \n";
			// Создаем именованный канал
			HANDLE hPipe = CreateNamedPipe(NAME_PIPE,					// Имя канала
				PIPE_ACCESS_DUPLEX,			// Одновременная запись-чтение
				PIPE_TYPE_BYTE | PIPE_WAIT,	// Потоковая передача данных, синхронная
				PIPE_UNLIMITED_INSTANCES,	// Максимальное количество клиентов
				NULL,						// Размер выходного буфера						
				NULL,						// Размер входного буфера
				NULL,						// Время ожидания
				NULL);						// Атрибуты защиты
											// Проверка
			if (hPipe == INVALID_HANDLE_VALUE)
			{
				std::cout << "\nSTATUS MSG: Ошибка CreateNamedPipe = " << GetLastError() << std::endl;
				std::cin.get();
				return 0;
			}

			std::cout << "\nSTATUS MSG: Ожидание подключения клиента... \n";
			// Ожидание подключения клиента
			if (ConnectNamedPipe(hPipe,		// Дескриптор канала 
				NULL))	// Синхронная
			{
				
				// Запуск потока и передача ему в параметрах дескриптор именованного канала
				HANDLE hThread = CreateThread(NULL,		// Защита по умолчанию
					NULL,		// Размер стека
					Thread,		// Указатель на функцию
					hPipe,		// Дескриптор именованного канала
					NULL,		// Запуск сразу
					NULL);		// ID не требуется

								// Проверка
				if (!hThread)
				{
					std::cout << "\nSTATUS MSG: Ошибка CreateThread = " << GetLastError() << std::endl;
					std::cin.get();
					return 0;
				}
				else
					// Закрываем дескриптор потока
					CloseHandle(hThread);
				users++;
			}
		}
	}


	std::cout << "\nSTATUS MSG: Сервер завершил свою работу. Выключение...";
	system("pause");
	return 0;
}

// Функция работающая в отдельном потоке, обслуживающая подключенного клиента
/*
	логика этой фукции:
		выдает клиенту случайный номер (чтобы в консоли было проще отслеживать с каким клиентом работаешь)
		считывает данные от клиента ("логин/пароль")
		запрашивает у админа ответ (надо сделать проверку на логин и пароль из файла, т.е. автоматизировать это)
		если логин и пароль верны - выходит из вечного цикла запроса логина и пароля
*/
DWORD WINAPI Thread(LPVOID param)
{
	srand(time(NULL));
	int rand_number = rand() % 10; // выдает клиенту случайный номер (чтобы в консоли было проще отслеживать с каким клиентом работаешь)

	HANDLE hPipe = (HANDLE)param;


	DWORD read = NULL; 
	char *buf = new char[MAX_PATH];

	while (hPipe)
	{
		std::cout << "Ожидание данных от клиента ("<<rand_number<<")... \n";

		if (!ReadFile(hPipe, buf, MAX_PATH, &read, NULL))
		{
			if (GetLastError() == CLIENT_DISCONNECT)
			{
				std::cout << "Клиент(" << rand_number << ") отключился " << std::endl;
				CloseHandle(hPipe);
				hPipe = NULL;
				break;
			}
			else
			{
				std::cout << "Ошибка ReadFile(" << rand_number << ") = " << GetLastError() << std::endl;
				break;
			}
		}
		else
		{
			// Вывод полученных данные на консоль
			buf[read] = '\0';
			std::cout << buf << std::endl;

			// Прием с клавиатуры ответа
			std::cout << "Введите ответ(" << rand_number << "): ";
			memset(buf, 0, sizeof(buf));
			std::cin >> buf;
			fflush(stdin);
			DWORD write = NULL;
			// Передача клиенту ответа
			if (!WriteFile(hPipe, buf, strlen(buf) * sizeof(char), &write, NULL))
			{
				if (GetLastError() == CLIENT_DISCONNECT)
				{
					std::cout << "Клиент(" << rand_number << ") отключился " << std::endl;
					break;
				}
				else
				{
					std::cout << "Ошибка WriteFile(" << rand_number << ") = " << GetLastError() << std::endl;
					break;
				}
			}
		}
	}

	// Закрытие дескриптора канала
	if (hPipe)
	{
		CloseHandle(hPipe);
	}
	

	return 0;
}