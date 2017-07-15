// GTTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include "GT_Util_GlogWrapper.h"
#include "GTServer/GT_Definition.h"

//void GT_WriteLog(std::istringstream os) {
//	std::string str = os.str();
//	printf("%s", str.c_str());
//	LOG(INFO) << str;
//}


int main()
{
	GT::UTIL::GT_Util_GlogWrapper gt = GT::UTIL::GT_Util_GlogWrapper::GetInstance();
	gt.GT_LogInitialize("test", GT_LOG_LEVEL_ALL, 1);
	GT_TRACE_FUNCTION
	int i = 0;
	while (i < 5000) {
		i++;
		//GT_WriteLog("TEST FOR TEST TEST"<<"");
		std::ostringstream sst;
		sst << "this is test = " << 1 << " value";
		std::string st = sst.str();
		//LOG(ERROR) << "this is test = " << 1 << " value";
		GT_LOG_INFO("GT" << "GTTEST" << 1);
		GT_LOG_ERROR("I AM GT" << " NO." << 1);
		GT_LOG_WARN("I AM GT" << " NO." << 1);
		
	}
	char c = 'c';
	std::cin >> c;
    return 0;
}

