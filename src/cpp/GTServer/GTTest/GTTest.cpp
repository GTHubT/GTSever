// GTTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "GT_Util_GlogWrapper.h"
#include "GTServer/GT_Definition.h"
#include <vector>
#include <thread>
#include <deque>

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
int main()
{
	
	GT::UTIL::GT_Util_GlogWrapper gt = GT::UTIL::GT_Util_GlogWrapper::GetInstance();
	gt.GT_LogInitialize("test", GT_LOG_LEVEL_WARNING, 1);
	GT_TRACE_FUNCTION;
	GT_LOG_INFO("this is a test " << 1);
	GT_LOG_ERROR("this is a test " << 2);
	GT_LOG_WARN("this is a test " << 3);
	testmove();
	char c = 'c';
	std::cin >> c;
    return 0;
}

