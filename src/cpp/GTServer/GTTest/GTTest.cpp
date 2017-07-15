// GTTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include "GT_Util_GlogWrapper.h"
#include "GTServer/GT_Definition.h"

void GT_WriteLog(std::ostream& os) {
	std::string str = "ERROR";
	printf("%s", str.c_str());
	//LOG(INFO) << os;
}

int main()
{
	GT::UTIL::GT_Util_GlogWrapper gt = GT::UTIL::GT_Util_GlogWrapper::GetInstance();
	gt.GT_LogInitialize("test", GT_LOG_LEVEL_ALL);
	int i = 0;
	while (i < 1) {
		i++;
		//GT_WriteLog("TEST FOR TEST TEST");
		//std::basic_ostream<std::ostream> os;
		std::ostream str(NULL);
		str<< "this is test = " << 1 << " value";
		std::ostringstream sst;
		sst << "this is test = " << 1 << " value";
		std::string st = sst.str();
		LOG(ERROR) << "this is test = " << 1 << " value";

	}
	char c = 'c';
	std::cin >> c;
    return 0;
}

