// GTTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "GT_Util_GlogWrapper.h"
#include "GTServer/GT_Definition.h"


int main()
{
	GT::UTIL::GT_Util_GlogWrapper gt = GT::UTIL::GT_Util_GlogWrapper::GetInstance();
	gt.GT_LogInitialize("test", GT_LOG_LEVEL_WARNING, 1);
	GT_TRACE_FUNCTION;
	GT_LOG_INFO("this is a test " << 1);
	GT_LOG_ERROR("this is a test " << 2);
	GT_LOG_WARN("this is a test " << 3);
	char c = 'c';
	std::cin >> c;
    return 0;
}

