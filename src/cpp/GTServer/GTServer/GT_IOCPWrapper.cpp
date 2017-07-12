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
            completion_port_ = CreateNewIoCompletionPort_();
            if (INVALID_HANDLE_VALUE == completion_port_) {
                printf("create new IOCP port failed! \n");
                return ret;
            }
            
            return true;
        }

		bool GT_IOCPWrapper::InitializeListenSocket_() {
            listen_socket_ = (TCP_MODE_ENABLE ? CreateOverlappedSocket_(AF_INET, SOCK_STREAM, IPPROTO_TCP) : CreateOverlappedSocket_(AF_INET, SOCK_DGRAM, IPPROTO_UDP));
            if (TCP_MODE_ENABLE && listen_socket_ != INVALID_SOCKET) {

                // create socket addr
                serveraddr.sin_family   = AF_INET;
                serveraddr.sin_port     = htons(BIND_PORT);
                serveraddr.sin_addr.S_un.S_addr = inet_addr(BIND_ADDRESS);

                // bind socket to server IP
                if (!bind(listen_socket_, (SOCKADDR*)(&serveraddr), sizeof(SOCKADDR_IN))) {
                    int err = WSAGetLastError();
                    printf("bind to local failed error code = %d \n", err);
                    return false;
                }

                // set listen num
                if (!listen(listen_socket_, SOMAXCONN)) {
                    int err = WSAGetLastError();
                    printf("bind to local failed error code = %d \n", err);
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
			listen_socket_      = INVALID_SOCKET;
            completion_port_    = INVALID_HANDLE_VALUE;
		}

        HANDLE GT_IOCPWrapper::CreateNewIoCompletionPort_() {            
            return CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
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
