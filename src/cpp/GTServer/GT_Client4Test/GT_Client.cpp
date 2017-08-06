#include "stdafx.h"
#include "GT_Client.h"
#include "GTUtlity/GT_Util_GlogWrapper.h"
#include "GTUtlity/GT_Util_CfgHelper.h"

#include <chrono>
#include <WinSock2.h>

#pragma  comment(lib, "Ws2_32.lib")

namespace GT {

	namespace CLIENT {

		GT_Client::GT_Client()
		{
			stop_thread_ = false;
		}


		GT_Client::~GT_Client()
		{
		}


		bool GT_Client::InitializeLog(std::string cfg_path) {
			/* load config */
			GT::UTIL::GT_Util_CfgHelper::LoadCfg(cfg_path);

			/* init log */
			std::string log_name_ = GT_READ_CFG_STRING("log_control", "log_name", "GT");
			std::string log_level_str = GT_READ_CFG_STRING("log_control", "log_level", "off");
			GT_LOG_LEVEL log_level_ = LoglevelConvert(log_level_str);
			int max_logsize_ = GT_READ_CFG_INT("log_control", "max_log_size", 50);
			GT::UTIL::GT_Util_GlogWrapper::GetInstance().GT_LogInitialize(log_name_, log_level_, max_logsize_);

			return true;
		}


		bool GT_Client::InitializeClient() {
			WORD version;
			WSADATA wsadata;
			int err = -1;
			version = MAKEWORD(2, 2);
			err = WSAStartup(version, &wsadata);
			if (err != 0) {
				GT_LOG_ERROR("socket environment init failed!");
				return false;
			}

			if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2) {
				GT_LOG_ERROR("WINSOCK dll do not support version 2.2!");
				return false;
			}

			return true;
		}


		GT_LOG_LEVEL GT_Client::LoglevelConvert(std::string loglevel) {
			if (loglevel == "all") {
				return GT_LOG_LEVEL_ALL;
			}
			else if (loglevel == "off") {
				return GT_LOG_LEVEL_OFF;
			}
			else if (loglevel == "warn")
			{
				return GT_LOG_LEVEL_WARNING;
			}
			else if (loglevel == "error") {
				return GT_LOG_LEVEL_ERROR;
			}
			else {
				printf("unknown log level! \n");
				return GT_LOG_LEVEL_OFF;
			}
		}


		void GT_Client::StartTest() {
			GT_TRACE_FUNCTION;
			client_test_thread_ = std::thread(&GT_Client::TestFunc_, this, std::ref(stop_thread_));
			printf("client test thread started!\n");
		}

		void GT_Client::TestFunc_(std::atomic_bool& end_thread) {
			while(!end_thread){
				//std::this_thread::sleep_for(std::chrono::milliseconds(10000));

				SOCKET client_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (client_ == INVALID_SOCKET) {
					GT_LOG_ERROR("client socket init failed!");
					CloseHandle((HANDLE)client_);
					continue;
				}
				
				printf("client socket = %d \n" ,(int)client_);

				SOCKADDR_IN client_addr_;
				client_addr_.sin_family = AF_INET;
				client_addr_.sin_addr.S_un.S_addr = inet_addr(GT_READ_CFG_STRING("server_cfg", "server_address" ,"127.0.0.2").c_str());
				client_addr_.sin_port = htons(GT_READ_CFG_INT("server_cfg","server_port", 5000));

				int ret = connect(client_, (sockaddr*)&client_addr_, sizeof(client_addr_));
				if (ret != 0) {
					GT_LOG_ERROR("connect error, error code = " << GetLastError());
					CloseHandle((HANDLE)client_);
					continue;
				}

				GT_LOG_INFO("connect server success!");

				std::string msg = "hello IOCP!";
				int num = send(client_, msg.c_str(), msg.length(), 0);
				if (num == SOCKET_ERROR) {
					GT_LOG_ERROR("send data error, err code = "<< GetLastError());
					closesocket(client_);
					CloseHandle((HANDLE)client_);
				}
				closesocket(client_);
				CloseHandle((HANDLE)client_);
			}
		}


		void GT_Client::StopTest() {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("stopping client test...");
			stop_thread_ = true;
			if (client_test_thread_.joinable()) {
				client_test_thread_.join();
			}
			GT_LOG_INFO("client test thread stopped!");
			printf("client test thread stopped!\n");
		}

	}
}
