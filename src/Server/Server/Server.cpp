// Server.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <ctime>

#include "PipeServer.h"

#define CLIENT_DISCONNECT 109
#define MAX_CONNECTED_USERS 3 
#define PIPE_NAME		L"\\\\.\\pipe\\pipe_example"


// ��� ������������ ������
LPCWSTR NAME_PIPE = L"\\\\.\\pipe\\MyPipeName";

// �������, ������� ����� � ��������� ������ ����������� �������
DWORD WINAPI Thread(LPVOID);



int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "rus");

	std::ifstream file;
	char *FName = new char[MAX_PATH];
	bool serverMode = false;
	std::vector<User<char>> vec;

	std::cout << "\nSTATUS MSG: ������ �������. \n\n������� ��� ����� � �������� � ��������: ";
	std::cin >> FName;

	file.open(FName);
	if (!file)
	{
		std::cout << "������ ������ ����� � ������ " << FName << "!" << std::endl;
	}
	else
	{
		std::cout << "�������� ����� ������: \n0) ������� \n1) ��������������� ������\n �������: ";
		std::cin >> serverMode;


		int users = 0;
		// � ����� ����� ��������
		while (users < MAX_CONNECTED_USERS)
		{
			std::cout << "\nSTATUS MSG: ������ ����� pipe \n";
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
				std::cout << "\nSTATUS MSG: ������ CreateNamedPipe = " << GetLastError() << std::endl;
				std::cin.get();
				return 0;
			}

			std::cout << "\nSTATUS MSG: �������� ����������� �������... \n";
			// �������� ����������� �������
			if (ConnectNamedPipe(hPipe,		// ���������� ������ 
				NULL))	// ����������
			{
				
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
					std::cout << "\nSTATUS MSG: ������ CreateThread = " << GetLastError() << std::endl;
					std::cin.get();
					return 0;
				}
				else
					// ��������� ���������� ������
					CloseHandle(hThread);
				users++;
			}
		}
	}


	std::cout << "\nSTATUS MSG: ������ �������� ���� ������. ����������...";
	system("pause");
	return 0;
}

// ������� ���������� � ��������� ������, ������������� ������������� �������
/*
	������ ���� ������:
		������ ������� ��������� ����� (����� � ������� ���� ����� ����������� � ����� �������� ���������)
		��������� ������ �� ������� ("�����/������")
		����������� � ������ ����� (���� ������� �������� �� ����� � ������ �� �����, �.�. ���������������� ���)
		���� ����� � ������ ����� - ������� �� ������� ����� ������� ������ � ������
*/
DWORD WINAPI Thread(LPVOID param)
{
	srand(time(NULL));
	int rand_number = rand() % 10; // ������ ������� ��������� ����� (����� � ������� ���� ����� ����������� � ����� �������� ���������)

	HANDLE hPipe = (HANDLE)param;


	DWORD read = NULL; 
	char *buf = new char[MAX_PATH];

	while (hPipe)
	{
		std::cout << "�������� ������ �� ������� ("<<rand_number<<")... \n";

		if (!ReadFile(hPipe, buf, MAX_PATH, &read, NULL))
		{
			if (GetLastError() == CLIENT_DISCONNECT)
			{
				std::cout << "������(" << rand_number << ") ���������� " << std::endl;
				CloseHandle(hPipe);
				hPipe = NULL;
				break;
			}
			else
			{
				std::cout << "������ ReadFile(" << rand_number << ") = " << GetLastError() << std::endl;
				break;
			}
		}
		else
		{
			// ����� ���������� ������ �� �������
			buf[read] = '\0';
			std::cout << buf << std::endl;

			// ����� � ���������� ������
			std::cout << "������� �����(" << rand_number << "): ";
			memset(buf, 0, sizeof(buf));
			std::cin >> buf;
			fflush(stdin);
			DWORD write = NULL;
			// �������� ������� ������
			if (!WriteFile(hPipe, buf, strlen(buf) * sizeof(char), &write, NULL))
			{
				if (GetLastError() == CLIENT_DISCONNECT)
				{
					std::cout << "������(" << rand_number << ") ���������� " << std::endl;
					break;
				}
				else
				{
					std::cout << "������ WriteFile(" << rand_number << ") = " << GetLastError() << std::endl;
					break;
				}
			}
		}
	}

	// �������� ����������� ������
	if (hPipe)
	{
		CloseHandle(hPipe);
	}
	

	return 0;
}