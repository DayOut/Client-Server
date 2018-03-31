// Client.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>


// Имя именнованного канала
LPCWSTR NAME_PIPE = L"\\\\.\\pipe\\MyPipeName";

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, ".866");
	WCHAR tmp[MAX_PATH] = { 0 };
	std::wcout << L"Введите путь к серверу: ";
	std::wcin.get(tmp, MAX_PATH - 1);

	std::wstring mywstring(tmp);
	std::wstring concatted_stdstr = L"\\\\" + mywstring + L"\\pipe\\MyPipeName";
	LPCWSTR NAME_PIPE = concatted_stdstr.c_str();

	// Ожидание подключения к серверу именованного канала 2 секунды
	if (!WaitNamedPipe(NAME_PIPE, 2000))
	{
		std::wcout << L"Ошибка WaitNamedPipe = " << GetLastError() << std::endl;
		std::wcin.get();
		return 0;
	}

	// Открытие именованного канала
	HANDLE hFile = CreateFile(NAME_PIPE,						// Имя канала
		GENERIC_READ | GENERIC_WRITE,	// На чтение и запись
		NULL,							// Монопольный доступ
		NULL,							// Защита
		OPEN_EXISTING,					// Существующий файла
		NULL,							// Атрибуты
		NULL);							// Без шаблона
										// Проверка
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::wcout << L"Ошибка CreateFile = " << GetLastError() << std::endl;
		std::wcin.get();
		return 0;
	}

	while (true)
	{
		std::wcin.clear();
		// Просьба ввести текст для сервера
		WCHAR buf[MAX_PATH] = { 0 };
		std::wcout << L"Введите строку для сервера: ";
		std::wcin.getline(buf, MAX_PATH - 1);
		std::wcin >> buf;
		DWORD write = NULL;
		// Передача серверу
		if (!WriteFile(hFile,
			buf,
			wcslen(buf) * sizeof(WCHAR),
			&write,
			NULL))
		{
			std::wcout << L"Ошибка WriteFile = " << GetLastError() << std::endl;
			std::wcin.get();
			return 0;
		}

		// Прием ответа от сервера
		memset(buf, 0, sizeof(buf));
		DWORD read = NULL;
		if (!ReadFile(hFile,
			buf,
			sizeof(buf) / sizeof(WCHAR),
			&read,
			NULL))
		{
			std::wcout << L"Ошибка ReadFile = " << GetLastError() << std::endl;
			std::wcin.get();
			return 0;
		}
		buf[read] = '\0';

		// Вывод ответа
		std::wcout << L"Ответ от сервера = " << buf << std::endl;
	}
	// Закрытие канала
	CloseHandle(hFile);
	std::wcin.get();

	return 0;
}

