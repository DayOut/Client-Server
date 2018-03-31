// Client.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>


// ��� ������������� ������
LPCWSTR NAME_PIPE = L"\\\\.\\pipe\\MyPipeName";

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, ".866");
	WCHAR tmp[MAX_PATH] = { 0 };
	std::wcout << L"������� ���� � �������: ";
	std::wcin.get(tmp, MAX_PATH - 1);

	std::wstring mywstring(tmp);
	std::wstring concatted_stdstr = L"\\\\" + mywstring + L"\\pipe\\MyPipeName";
	LPCWSTR NAME_PIPE = concatted_stdstr.c_str();

	// �������� ����������� � ������� ������������ ������ 2 �������
	if (!WaitNamedPipe(NAME_PIPE, 2000))
	{
		std::wcout << L"������ WaitNamedPipe = " << GetLastError() << std::endl;
		std::wcin.get();
		return 0;
	}

	// �������� ������������ ������
	HANDLE hFile = CreateFile(NAME_PIPE,						// ��� ������
		GENERIC_READ | GENERIC_WRITE,	// �� ������ � ������
		NULL,							// ����������� ������
		NULL,							// ������
		OPEN_EXISTING,					// ������������ �����
		NULL,							// ��������
		NULL);							// ��� �������
										// ��������
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::wcout << L"������ CreateFile = " << GetLastError() << std::endl;
		std::wcin.get();
		return 0;
	}

	while (true)
	{
		std::wcin.clear();
		// ������� ������ ����� ��� �������
		WCHAR buf[MAX_PATH] = { 0 };
		std::wcout << L"������� ������ ��� �������: ";
		std::wcin.getline(buf, MAX_PATH - 1);
		std::wcin >> buf;
		DWORD write = NULL;
		// �������� �������
		if (!WriteFile(hFile,
			buf,
			wcslen(buf) * sizeof(WCHAR),
			&write,
			NULL))
		{
			std::wcout << L"������ WriteFile = " << GetLastError() << std::endl;
			std::wcin.get();
			return 0;
		}

		// ����� ������ �� �������
		memset(buf, 0, sizeof(buf));
		DWORD read = NULL;
		if (!ReadFile(hFile,
			buf,
			sizeof(buf) / sizeof(WCHAR),
			&read,
			NULL))
		{
			std::wcout << L"������ ReadFile = " << GetLastError() << std::endl;
			std::wcin.get();
			return 0;
		}
		buf[read] = '\0';

		// ����� ������
		std::wcout << L"����� �� ������� = " << buf << std::endl;
	}
	// �������� ������
	CloseHandle(hFile);
	std::wcin.get();

	return 0;
}

