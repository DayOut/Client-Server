// Server.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>



using namespace std;

// ��� ������������ ������
LPCWSTR NAME_PIPE = L"\\\\.\\pipe\\MyPipeName";

// �������, ������� ����� � ��������� ������ ����������� �������
DWORD WINAPI Thread(LPVOID);

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, ".866");

	wcout << L"Server started... \n";

	// � ����� ����� ��������
	while (true)
	{
		wcout << L"Created named Pipe... \n";
		// ������� ����������� �����
		HANDLE hPipe = CreateNamedPipe(NAME_PIPE,					// ��� ������
			PIPE_ACCESS_DUPLEX,			// ������������� ������-������
			PIPE_TYPE_BYTE | PIPE_WAIT,	// ��������� �������� ������, ����������
			PIPE_UNLIMITED_INSTANCES,	// ������������ ���������� ��������
			NULL,						// ������ ��������� ������						
			NULL,						// ������ �������� ������
			NULL,						// ����� ��������
			NULL);						// �������� ������
										// ��������
		if (hPipe == INVALID_HANDLE_VALUE)
		{
			wcout << L"������ CreateNamedPipe = " << GetLastError() << endl;
			wcin.get();
			return 0;
		}

		wcout << L"Waiting for client... \n";
		// �������� ����������� �������
		if (ConnectNamedPipe(hPipe,		// ���������� ������ 
			NULL))	// ����������
		{
			wcout << L"Client connected... \n";
			// ������ ������ � �������� ��� � ���������� ���������� ������������ ������
			HANDLE hThread = CreateThread(NULL,		// ������ �� ���������
				NULL,		// ������ �����
				Thread,		// ��������� �� �������
				hPipe,		// ���������� ������������ ������
				NULL,		// ������ �����
				NULL);		// ID �� ���������
							// ��������
			if (!hThread)
			{
				wcout << L"������ CreateThread = " << GetLastError() << endl;
				wcin.get();
				return 0;
			}
			else
				// ��������� ���������� ������
				CloseHandle(hThread);
		}
	}

	wcin.get();
	return 0;
}

// ������� ���������� � ��������� ������, ������������� ������������� �������
DWORD WINAPI Thread(LPVOID param)
{
	// ���������� ����������� ������������ ������
	HANDLE hPipe = (HANDLE)param;

	// ������ �� ������� ������
	DWORD read = NULL;
	WCHAR buf[MAX_PATH] = { 0 };
	while (hPipe)
	{
		wcout << L"Trying to receive data from client... \n";

		if (!ReadFile(hPipe, buf, MAX_PATH, &read, NULL))
		{
			wcout << L"������ ReadFile = " << GetLastError() << endl;
			wcin.get();
		}
		else
		{
			// ����� ���������� ������ �� �������
			buf[read] = '\0';
			wcout << buf << endl;

			// ����� � ���������� ������
			wcout << L"������� �����: ";
			memset(buf, 0, sizeof(buf));
			wcin.getline(buf, MAX_PATH);
			fflush(stdin);
			DWORD write = NULL;
			// �������� ������� ������
			if (!WriteFile(hPipe, buf, wcslen(buf) * sizeof(WCHAR), &write, NULL))
			{
				wcout << L"������ WriteFile = " << GetLastError() << endl;
				wcin.get();
				break;
			}
		}
	}

	// �������� ����������� ������
	CloseHandle(hPipe);

	return 0;
}