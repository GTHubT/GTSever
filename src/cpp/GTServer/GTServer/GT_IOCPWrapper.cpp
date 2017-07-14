#include "GT_IOCPWrapper.h"
#include "GT_Definition.h"
#include "GTUtlity/GT_Util_OSInfo.h"
#include "GT_Cfg.h"

#include <stdio.h>

#ifndef GLOG_NO_ABBREVIATED_SEVERITIES
#define GLOG_NO_ABBREVIATED_SEVERITIES
#endif
#include <glog/logging.h>

namespace GT {

    namespace NET {
        
        GT_IOCPWrapper::GT_IOCPWrapper() :
            is_inited_(false),
            is_read_callback_setted_(false),
            is_write_callback_setted_(false)
        {
            listen_socket_ = INVALID_SOCKET;
            completion_port_ = INVALID_HANDLE_VALUE;
        }

        bool GT_IOCPWrapper::Initialize() {
            bool ret = false;
            do {
                // init socket environment
                WORD version;
                WSADATA wsadata;
                int err = -1;
                version = MAKEWORD(2, 2);
                err = WSAStartup(version, &wsadata);
                if (err != 0) {
                    printf("socket environment init failed! \n");
                    break;
                }

                // check WINSOCK dll support version 2.2 
                if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2) {
                    printf("WINSOCK dll do not support version 2.2 ! \n");
                    break;
                }

                // init listen socket
                bool ret = InitializeListenSocket_();
                if (!ret) {
                    printf("init listen socket failed! \n");
                    break;
                }

                // init completion port
                completion_port_ = CreateNewIoCompletionPort_();
                if (INVALID_HANDLE_VALUE == completion_port_) {
                    printf("create new IOCP port failed! \n");
                    break;
                }

                // bind listen socket to completion port
                //ret = BindSocketToCompletionPort(listen_socket_);
                if (!ret) {
                    printf("bind listen socket to completion port failed! \n");
                    break;
                }

                ret = true;
            } while (0);

            if (!ret) {
                WSACleanup();
                printf("IOCP init failed! \n");
            } else {
                is_inited_ = true;
            }
            return ret;
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
        
        HANDLE GT_IOCPWrapper::CreateNewIoCompletionPort_() {            
            return CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
        }

        bool GT_IOCPWrapper::BindSocketToCompletionPort(SOCKET s, ULONG_PTR completionkey) {
            HANDLE temp_port = CreateIoCompletionPort((HANDLE)s, completion_port_, completionkey, 0);
            return temp_port == completion_port_;
        }

        void GT_IOCPWrapper::StartService() {
            // create thread pool
            std::function<void()> threadfunc = std::bind(&GT_IOCPWrapper::GetCompletionPortStatus, this);
            thread_pool_.Start(GT::UTIL::GT_Util_OSInfo::GetCPUNum() * 2, threadfunc);
        }

        GT_IOCPWrapper& GT_IOCPWrapper::GetInstance() {
            static GT_IOCPWrapper iocpinstance;
            return iocpinstance;
        }


        bool GT_IOCPWrapper::StopService() {
            return true;
        }

        void GT_IOCPWrapper::SetReadEventCallBack(Read_Ready_Event_Callback read_func) {
            read_func_ = read_func;
            is_read_callback_setted_ = true;
        }

        void GT_IOCPWrapper::SetWriteEventCallBack(Write_Ready_Event_Callback write_func) {
            write_func_ = write_func;
            is_write_callback_setted_ = true;
        }

		SOCKET GT_IOCPWrapper::CreateOverlappedSocket_(int af, int type, int protocl) {
			return WSASocket(af, type, protocl, nullptr, 0, WSA_FLAG_OVERLAPPED);
		}


        void GT_IOCPWrapper::ProcessAcceptEvent_() {

        }


        void GT_IOCPWrapper::PostReadEvent_() {

        }


        void GT_IOCPWrapper::PostWriteEvent_() {

        }

        void GT_IOCPWrapper::PostAcceptEvent_() {

        }

		void GT_IOCPWrapper::GetCompletionPortStatus() {

		}

    }
}
