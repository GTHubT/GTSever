// GTTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "GT_Util_GlogWrapper.h"
#include "GTServer/GT_Definition.h"
#include "GT_Util_FileHelper.h"
#include "GT_Util_OSInfo.h"
#include "GT_Util_CfgHelper.h"
#include <vector>
#include <thread>
#include <deque>

using namespace GT::UTIL;
void thredfunc() {
	while (1);
}

void testmove() {
	std::vector<std::thread> threadpool_;
	std::deque<std::thread> threadque_;
	std::thread th_1(thredfunc);
	GT_LOG_ERROR("thread id = " << th_1.get_id());
	threadque_.push_back(std::move(th_1));
	std::thread & s = threadque_.back();
	GT_LOG_ERROR("after queue move thread id = " << s.get_id());
	threadpool_.push_back(std::move(threadque_.back()));
	GT_LOG_ERROR("after vector move thread id = " << threadpool_[0].get_id());
	GT_LOG_ERROR("before pop que size = " << threadque_.size());
	//threadque_.erase(threadque_.);
	GT_LOG_ERROR("after pop que size = " << threadque_.size());
	threadpool_[0].detach();
}

void testreadfile() {
	std::string path = GT::UTIL::GT_Util_OSInfo::GetCurrentFolder();
    std::string content = GT::UTIL::GT_Util_FileHelper::ReadFileContent(path + "\\GTServer.cfg");
	GT_Util_CfgHelper::LoadCfg(path + "\\GTServer.cfg");
	std::string serverip = GT_READ_CFG_STRING("server_cfg", "server_address", "10.204.16.28");
    GT_LOG_ERROR(content.c_str() << "size = " << content.size());
	GT_LOG_ERROR("SERVER IP = " << serverip.c_str());
}

int main()
{
	
	GT::UTIL::GT_Util_GlogWrapper gt = GT::UTIL::GT_Util_GlogWrapper::GetInstance();
	gt.GT_LogInitialize("test.log", GT_LOG_LEVEL_WARNING, 1);
	GT_TRACE_FUNCTION;
	GT_LOG_INFO("this is a test " << 1);
	GT_LOG_ERROR("this is a test " << 2);
	GT_LOG_WARN("this is a test " << 3);
    testreadfile();
	testmove();
	char c = 'c';
	std::cin >> c;
    return 0;
}
