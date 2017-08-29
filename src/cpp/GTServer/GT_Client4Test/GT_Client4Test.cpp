// GT_Client4Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GT_Client.h"
#include "GTUtlity/GT_Util_OSInfo.h"
#include "GTUtlity/GT_Util_CfgHelper.h"

#include <iostream>


int main()
{

	GT::CLIENT::GT_Client client;

	std::string cfg_path = GT::UTIL::GT_Util_OSInfo::GetCurrentFolder() + "GTServer.cfg";
	bool ret = client.InitializeLog(cfg_path);
	if (!ret) {
		std::cout << "init log failed!" << std::endl;
	}
	ret = client.InitializeClient();
	if (!ret) {
		std::cout << "init client failed!" << std::endl;
		return 0;
	}

	std::string client_test_mode = GT_READ_CFG_STRING("client_test_mode","model","uncontinuous");
	client.StartTest(client_test_mode);

	while (true) {
		char c = '0';
		std::cout << "do you mean exit test? (y/n)" << std::endl;
		std::cin >> c;
		if (c == 'y' || c == 'Y') {
			std::cout << "now exiting test..." << std::endl;
			client.StopTest();
			break;
		}
	}
    return 0;
}

