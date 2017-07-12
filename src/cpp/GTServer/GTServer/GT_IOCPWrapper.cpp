#include "GT_IOCPWrapper.h"
#include "GT_Definition.h"

#include <stdio.h>

namespace GT {

    namespace NET {

        bool GT_IOCPWrapper::Initialize() {

			// init socket environment
			WORD version;
			WSADATA wsadata;
			int err = -1;
			version = MAKEWORD(2, 2);
			err = WSAStartup(version, &wsadata);
			if (err != 0) {
				printf("socket environment init failed! \n");
				WSACleanup();
				return false;
			}

			// check WINSOCK dll support version 2.2 
			if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2) {
				printf("WINSOCK dll do not support version 2.2 ! \n");
				WSACleanup();
				return false;
			}

			// init listen socket
			bool ret = InitializeListenSocket_();
			if (!ret) {
				printf("init listen socket failed! \n");
				return ret;
			}

			// init completion port


            return true;
        }

		bool GT_IOCPWrapper::InitializeListenSocket_() {
			if (TCP_MODE_ENABLE) {
				listen_socket_ = CreateOverlappedSocket_(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (INVALID_SOCKET == listen_socket_) {
					printf("create overlapped listen socket failed! \n");
					return false;
				}
			}
			else {
				listen_socket_ = CreateOverlappedSocket_(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
				if (INVALID_SOCKET == listen_socket_) {
					printf("create overlapped listen socket failed! \n");
					return false;
				}
			}
			return true;
		}

        bool GT_IOCPWrapper::Finalize() {
            return true;
        }

        GT_IOCPWrapper& GT_IOCPWrapper::GetInstance() {
            static GT_IOCPWrapper iocpinstance;
            return iocpinstance;
        }

		GT_IOCPWrapper::GT_IOCPWrapper() :is_inited_(false) {
			listen_socket_ = INVALID_SOCKET;
		}

        bool GT_IOCPWrapper::CreateNewIoCompletionPort() {
            return true;
        }

        bool GT_IOCPWrapper::BindSocketToCompletionPort(SOCKET s, ULONG_PTR completionkey) {
            return true;
        }

		void GT_IOCPWrapper::GetCompletionPortStatus(Ready_Event_Callback callback) {

		}

		SOCKET GT_IOCPWrapper::CreateOverlappedSocket_(int af, int type, int protocl) {
			return WSASocket(af, type, protocl, nullptr, 0, WSA_FLAG_OVERLAPPED);
		}

    }
}
