// GTTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "GT_Util_GlogWrapper.h"
#include "GTIOCP/GT_Definition.h"
#include "GT_Util_FileHelper.h"
#include "GT_Util_OSInfo.h"
#include "GT_Util_CfgHelper.h"
#include <vector>
#include <thread>
#include <deque>
#include <memory>
#include <set>
#include <unordered_map>
#include <windows.h>

using namespace GT::UTIL;
void thredfunc() {
	while (1);
}


static void CALLBACK worker_func(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_     PTP_WORK              Work) {
	printf("I am a thread worker!\n");
}


void test_windows_thread_pool() {
	auto pool = CreateThreadpoolWork(worker_func, NULL, NULL);
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

void test_stl_set() {
	std::set<std::shared_ptr<int>> int_set;
	int_set.insert(std::shared_ptr<int>(new int(1)));
	int_set.insert(std::shared_ptr<int>(new int(2)));
	int_set.insert(std::shared_ptr<int>(new int(3)));

	std::set<std::shared_ptr<int>>::iterator iter = int_set.begin();
	for (; iter != int_set.end();iter++) {
		printf("test the set by iterator, iterator value = %d, pointer address = %p \n", *((*iter).get()), (*iter).get());
		//iter = int_set.erase(iter);
	}

	for (auto iter : int_set) {
		printf("test the set by auto, iterator value = %d, pointer address = %p \n", *iter, iter.get());
	}

}

void test_map() {
    struct item {
        int i;
    };
    std::unordered_map<ULONG_PTR, item> u_map;
    std::map<ULONG_PTR, item> o_map;

    for (int i = 0; i < 10; i++) {
        item* it = new item;
        it->i = i;
        u_map.insert(std::make_pair((ULONG_PTR)it, *it));
        o_map.insert(std::make_pair((ULONG_PTR)it, *it));
        printf("i = %d, item addr = %p \n", i, it);
    }
    std::unordered_map<ULONG_PTR, item>::iterator it = u_map.begin();
    std::map<ULONG_PTR, item>::iterator iter = o_map.begin();

    for (int i = 0; i < 10; i++) {
        printf("i = %d, F item addr = %p \n", i, it->first);
        //printf("i = %d, F item addr = %p \n", i, iter->first);
        it++;
        iter++;
    }
    std::unordered_map<ULONG_PTR, item>::iterator it_ = u_map.begin();
    std::map<ULONG_PTR, item>::iterator iter_ = o_map.begin();
    it_ = u_map.erase(it_);

    for (int i = 0; i < 9; i++) {
        printf("i = %d, G item addr = %p \n", i, it_->first);
        //printf("i = %d, G item addr = %p \n", i, iter_->first);
        it_++;
        iter_++;
    }
}

int main()
{
	test_windows_thread_pool();
	PTP_CALLBACK_INSTANCE instance = nullptr;
	worker_func(instance, NULL, NULL);
   /* test_map();
	test_stl_set();
	GT::UTIL::GT_Util_GlogWrapper gt = GT::UTIL::GT_Util_GlogWrapper::GetInstance();
	gt.GT_LogInitialize("test.log", GT_LOG_LEVEL_WARNING, 1);
	GT_TRACE_FUNCTION;
	GT_LOG_INFO("this is a test " << 1);
	GT_LOG_ERROR("this is a test " << 2);
	GT_LOG_WARN("this is a test " << 3);
    testreadfile();
	testmove();*/
	char c = 'c';
	std::cin >> c;
    return 0;
}

