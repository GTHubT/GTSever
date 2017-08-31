#include "stdafx.h"
#include "GT_Client.h"
#include "GTUtlity/GT_Util_GlogWrapper.h"
#include "GTUtlity/GT_Util_CfgHelper.h"


namespace GT {

	namespace CLIENT {
#define MAX_CLIENT_USED_TIME    1200
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
			std::string log_name_ = "GTClient.log";
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


		void GT_Client::StartTest(std::string test_mode) {
			GT_TRACE_FUNCTION;
            if (test_mode == "continuous") {
                GT_LOG_INFO("start test by Continuous mode");
                client_test_thread_ = std::thread(&GT_Client::StartTestByContinuous_, this, std::ref(stop_thread_));
                printf("start test by Continuous mode!\n");
            }
            else {
                GT_LOG_INFO("start test by unContinuous mode");
                client_test_thread_ = std::thread(&GT_Client::StartTestByUnContinuous_, this, std::ref(stop_thread_));
                printf("start test by unContinuous mode!\n");
            }
		}

		void GT_Client::StartTestByUnContinuous_(std::atomic_bool& end_thread) {
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
					GT_LOG_ERROR("connect error, error code = " << WSAGetLastError());
					CloseHandle((HANDLE)client_);
					continue;
				}

				GT_LOG_INFO("connect server success!");

				std::string msg = "hello GTServer!";
				int num = send(client_, msg.c_str(), msg.length(), 0);
				if (num == SOCKET_ERROR) {
					GT_LOG_ERROR("send data error, err code = "<< WSAGetLastError());
					closesocket(client_);
					//CloseHandle((HANDLE)client_); Do not use the CloseHandle function to close a socket. Instead, use the closesocket function, which releases all resources associated with the socket including the handle to the socket object. For more information, see Socket Closure.
				}
				closesocket(client_);
				//CloseHandle((HANDLE)client_);
				// CloseHandle only work on the object:
				//  Access token
				//  Communications device
				//	Console input
				//	Console screen buffer
				//	Event
				//	File
				//	File mapping
				//	I / O completion port
				//	Job
				//	Mailslot
				//	Memory resource notification
				//	Mutex
				//	Named pipe
				//	Pipe
				//	Process
				//	Semaphore
				//	Thread
				//	Transaction
				//	Waitable timer
			}
		}

        void GT_Client::StartTestByContinuous_(std::atomic_bool& end_thread) {
            while (!end_thread) {
                //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                sock_pack client_;
				client_.use_times_ = 0;
                client_.sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (client_.sock_ == INVALID_SOCKET) {
                    GT_LOG_ERROR("client socket init failed!");
                    closesocket(client_.sock_);
                    continue;
                }


                SOCKADDR_IN client_addr_;
                client_addr_.sin_family = AF_INET;
                client_addr_.sin_addr.S_un.S_addr = inet_addr(GT_READ_CFG_STRING("server_cfg", "server_address", "127.0.0.2").c_str());
                client_addr_.sin_port = htons(GT_READ_CFG_INT("server_cfg", "server_port", 5000));

                int ret = connect(client_.sock_, (sockaddr*)&client_addr_, sizeof(client_addr_));
                if (ret != 0) {
                    GT_LOG_ERROR("connect error, error code = " << WSAGetLastError());
                    closesocket(client_.sock_);
                    continue;
                }
                connect_sock_.push_back(client_);
                GT_LOG_INFO("connect server success!");

				for (auto iter = connect_sock_.begin(); iter < connect_sock_.end(); ) {
					if (iter->use_times_ > MAX_CLIENT_USED_TIME) {
						printf("client socket = %d \n", (int)iter->sock_);
						printf("remove the socket that used too much time!\n");
						closesocket(iter->sock_);
						iter = connect_sock_.erase(iter);
					}
					else
					{
						char index[10];
						_ltoa_s(iter->use_times_, index, 10);
						std::string msg = "hello GTServer, this msg index = " + std::string(index);
						printf("client socket = %d, send data = %s \n", (int)iter->sock_, msg.c_str());
						int num = send(iter->sock_, msg.c_str(), msg.length(), 0);
						if (num == SOCKET_ERROR) {
							GT_LOG_ERROR("send data error, err code = " << WSAGetLastError());
							closesocket(client_.sock_);
							iter = connect_sock_.erase(iter);
							continue;
						}
						iter->use_times_++;
						iter++;
					}
				}
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
