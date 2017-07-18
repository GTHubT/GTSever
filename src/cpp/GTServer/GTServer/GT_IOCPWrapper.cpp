#include "GT_IOCPWrapper.h"
#include "GT_Definition.h"
#include "GTUtlity/GT_Util_OSInfo.h"
#include "GTUtlity/GT_Util_GlogWrapper.h"
#include "GTUtlity/GT_Util_CfgHelper.h"

#include <stdio.h>

using namespace GT::UTIL;
namespace GT {

    namespace NET {
        
        GT_IOCPWrapper::GT_IOCPWrapper() :
            is_inited_(false),
            is_read_callback_setted_(false),
            is_write_callback_setted_(false),
            socket_pool_enable_(false)
        {
            listen_socket_ = INVALID_SOCKET;
            completion_port_ = INVALID_HANDLE_VALUE;
        }

        bool GT_IOCPWrapper::Initialize() {
			GT_TRACE_FUNCTION;
            bool ret = false;
            do {
                WORD version;
                WSADATA wsadata;
                int err = -1;
                version = MAKEWORD(2, 2);
                err = WSAStartup(version, &wsadata);
                if (err != 0) {
					GT_LOG_ERROR("socket environment init failed!");
                    break;
                }

                if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2) {
                    GT_LOG_ERROR("WINSOCK dll do not support version 2.2!");
                    break;
                }

                socket_pool_enable_ = GT_SocketPool::GetInstance().Initilize();

                bool ret = InitializeListenSocket_();
                if (!ret) {
                    GT_LOG_ERROR("init listen socket failed!");
                    break;
                }
                
                completion_port_ = CreateNewIoCompletionPort_();
                if (INVALID_HANDLE_VALUE == completion_port_) {
                    GT_LOG_ERROR("create new IOCP port failed!");
                    break;
                }

                //ret = BindSocketToCompletionPort(listen_socket_);
                if (!ret) {
                    GT_LOG_ERROR("bind listen socket to completion port failed!");
                    break;
                }

                PostAcceptEvent_();

                ret = true;
            } while (0);

			ret ? is_inited_ = ret : WSACleanup();
            return ret;
        }


		bool GT_IOCPWrapper::InitializeListenSocket_() {
            listen_socket_ = (GT_READ_CFG_BOOL("server_cfg", "enable_tcp_mode", 1) ? CreateOverlappedSocket_(AF_INET, SOCK_STREAM, IPPROTO_TCP) : CreateOverlappedSocket_(AF_INET, SOCK_DGRAM, IPPROTO_UDP));
            if (GT_READ_CFG_BOOL("server_cfg", "enable_tcp_mode", 1) && listen_socket_ != INVALID_SOCKET) {

                // create socket addr
                serveraddr_.sin_family   = AF_INET;
                serveraddr_.sin_port     = htons(GT_READ_CFG_INT("server_cfg","server_port",5555));
                serveraddr_.sin_addr.S_un.S_addr = inet_addr(GT_READ_CFG_STRING("server_cfg", "server_address", "127.0.0.2").c_str());

                // bind socket to server IP
                if (!bind(listen_socket_, (SOCKADDR*)(&serveraddr_), sizeof(SOCKADDR_IN))) {
                    int err = WSAGetLastError();
                    GT_LOG_ERROR("bind to local failed error code = " << err);
                    return false;
                }

                // set listen num
                if (!listen(listen_socket_, SOMAXCONN)) {
                    int err = WSAGetLastError();
                    GT_LOG_ERROR("bind to local failed error code = " << err);
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
			GT_TRACE_FUNCTION;
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

        void GT_IOCPWrapper::SetReadCompleteEventCallBack(Read_Complete_Event_Callback read_func) {
            read_func_ = read_func;
            is_read_callback_setted_ = true;
        }

        void GT_IOCPWrapper::SetWriteCompleteEventCallBack(Write_Complete_Event_Callback write_func) {
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
