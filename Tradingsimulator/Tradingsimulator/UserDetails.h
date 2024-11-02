#pragma once
#include<fstream>
#include<string>

using namespace std;

struct USER
{
	string username;
	string password;
};

struct LOGINDETAILS
{
	string loginUername;
	string loginPassword;
};

struct FORGOTPASSWORDDETAILS
{
	string forgotPasswordUsername;
	string newPassword;
};

extern fstream accountsInfo;
