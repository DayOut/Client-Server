// Server.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>



using namespace std;

// Имя именованного канала
LPCWSTR NAME_PIPE = L"\\\\.\\pipe\\MyPipeName";

// Функция, которая будет в отдельном потоке обслуживать клиента
DWORD WINAPI Thread(LPVOID);

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, ".866");

	wcout << L"Server started... \n";

	// В цикле прием клиентов
	while (true)
	{
		wcout << L"Created named Pipe... \n";
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
			wcout << L"Ошибка CreateNamedPipe = " << GetLastError() << endl;
			wcin.get();
			return 0;
		}

		wcout << L"Waiting for client... \n";
		// Ожидание подключения клиента
		if (ConnectNamedPipe(hPipe,		// Дескриптор канала 
			NULL))	// Синхронная
		{
			wcout << L"Client connected... \n";
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
				wcout << L"Ошибка CreateThread = " << GetLastError() << endl;
				wcin.get();
				return 0;
			}
			else
				// Закрываем дескриптор потока
				CloseHandle(hThread);
		}
	}

	wcin.get();
	return 0;
}

// Функция работающая в отдельном потоке, обслуживающая подключенного клиента
DWORD WINAPI Thread(LPVOID param)
{
	// Сохранение дескриптора именованного канала
	HANDLE hPipe = (HANDLE)param;

	// Читаем от клиента данные
	DWORD read = NULL;
	WCHAR buf[MAX_PATH] = { 0 };
	while (hPipe)
	{
		wcout << L"Trying to receive data from client... \n";

		if (!ReadFile(hPipe, buf, MAX_PATH, &read, NULL))
		{
			wcout << L"Ошибка ReadFile = " << GetLastError() << endl;
			wcin.get();
		}
		else
		{
			// Вывод полученных данные на консоль
			buf[read] = '\0';
			wcout << buf << endl;

			// Прием с клавиатуры ответа
			wcout << L"Введите ответ: ";
			memset(buf, 0, sizeof(buf));
			wcin.getline(buf, MAX_PATH);
			fflush(stdin);
			DWORD write = NULL;
			// Передача клиенту ответа
			if (!WriteFile(hPipe, buf, wcslen(buf) * sizeof(WCHAR), &write, NULL))
			{
				wcout << L"Ошибка WriteFile = " << GetLastError() << endl;
				wcin.get();
				break;
			}
		}
	}

	// Закрытие дескриптора канала
	CloseHandle(hPipe);

	return 0;
}