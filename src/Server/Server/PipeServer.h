#pragma once

#include <windows.h>
#include <vector>
#include <fstream>
#include <string>


#include "User.h"

template <typename T>
class CPipeServer
{

	std::vector<User<T>> db_users;
	int maxLenLog, maxLenPass;

public:
	bool readFromDB(std::ifstream &file)
	{
		if (file >> maxLenLog >> maxLenPass)
		{
			T *tmpLogin = new T[maxLenLog];
			T *tmpPassword = new T[maxLenPass];
			while (file >> tmpLogin >> tmpPassword)
			{
				db_users.push_back({ tmpLogin, tmpPassword });
			}
			delete[] tmpLogin;
			delete[] tmpPassword;
			return true;
		}

		return false;
	}
};