// Client.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>



// ��� ������������� ������
LPCWSTR NAME_PIPE = L"\\\\.\\pipe\\MyPipeName";

//� ���������� ��� ���� ������� � ��������� ����, ��, ��� ���������, "� ��� ������" (��������)
bool tryToAuth(HANDLE hFile)
{
	/*
		������:
			+	�������� �����
			+	�������� ������
			+	������� � ������ ���� "�����/������"
			+	��������� ��� ������
			+	��������� ������ �� �������
			+	���� 1 - �����
			+	���� 0 - ����
	*/

	char * login	= new char [50];
	char * password = new char[50];

	//char *buf = new char[MAX_PATH];

	std::cout << "�����: ";
	std::cin >> login;
	std::cout << "������: ";
	std::cin >> password;

	char buf[100];   

	//����� ������ "�����/������"
	strcpy_s(buf, login); 
	strcat_s(buf, "/");
	strcat_s(buf, password);

	DWORD write = NULL;
	// �������� �������
	if (!WriteFile(hFile,
					buf,
					sizeof(buf) * sizeof(char),
					&write,
					NULL))
	{
		std::cout << "������ WriteFile = " << GetLastError() << std::endl;
		std::cin.get();
		return 0;
	}

	// ����� ������ �� �������
	memset(buf, 0, sizeof(buf)); 
	DWORD read = NULL;
	if (!ReadFile(hFile,
		buf,
		sizeof(buf) / sizeof(char),
		&read,
		NULL))
	{
		std::cout << "������ ReadFile = " << GetLastError() << std::endl;
		std::cin.get();
		return 0;
	}
	buf[read] = '\0'; // �������� �����, ��������� ��������� �������� ������ ������ ������

	std::cout << "����� �� ������� = " << buf << std::endl;
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
	std::cout << "������� ���� � �������: ";
	std::wcin >> tmp;
	

	std::wstring mywstring(tmp);
	std::wstring concatted_stdstr = L"\\\\" + mywstring + L"\\pipe\\MyPipeName";

	LPCWSTR NAME_PIPE = concatted_stdstr.c_str();

	// �������� ����������� � ������� ������������ ������ 2 �������
	if (!WaitNamedPipe(NAME_PIPE, 2000))
	{
		std::cout << "������ WaitNamedPipe = " << GetLastError() << std::endl;
		std::cin.get();
		return 0;
	}

	// �������� ������������ ������
	HANDLE hFile = CreateFile(NAME_PIPE,						
								GENERIC_READ | GENERIC_WRITE,
								NULL,							
								NULL,	// ������ (����� � ���, ��� ��� � ��� ������ � ���� �������� �� ����� 
										// ������, �� � ��������� �� IP, �� ����, �������� ���������� � �� ���� �� ������ �������)
								OPEN_EXISTING,					
								NULL,							
								NULL);							

	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cout << "������ CreateFile = " << GetLastError() << std::endl;
		std::cin.get();
		return 0;
	}

	while (true)
	{
		std::cout << "\n����������� �� �������... \n";
		if (tryToAuth(hFile))
		{
			std::cout << "����������� �������!";
			break;
		}
		else
		{
			std::cout << "�������� ����� ��� ������!";
		}
		
	}

	system("pause");

	// �������� ������
	CloseHandle(hFile);

	return 0;
}

