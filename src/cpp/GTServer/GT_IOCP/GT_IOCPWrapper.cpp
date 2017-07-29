#include "GT_IOCPWrapper.h"
#include "GT_Definition.h"
#include "GTUtlity/GT_Util_OSInfo.h"
#include "GTUtlity/GT_Util_GlogWrapper.h"
#include "GTUtlity/GT_Util_CfgHelper.h"

#include <stdio.h>

using namespace GT::UTIL;
namespace GT {

    namespace NET {
        
#ifndef		GTSERVER_RESOURCE_MANAGER
#define		GTSERVER_RESOURCE_MANAGER		GT_Resource_Manager::GetInstance()
#endif

        GT_IOCPWrapper::GT_IOCPWrapper() :
            is_inited_(false),
            is_read_callback_setted_(false),
            is_write_callback_setted_(false),
            completionkey_ioevent_manager_enable_(false)
        {
			paccpetex_func_ = nullptr;
            listen_socket_ptr_ = nullptr;
            completion_port_ = INVALID_HANDLE_VALUE;
			pgetacceptex_sockaddrs_func_ = nullptr;
			accept_socket_completion_key_ = nullptr;
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

                completionkey_ioevent_manager_enable_ = GTSERVER_RESOURCE_MANAGER.Initialize();

                bool ret = InitializeListenSocket_();
                if (!ret) {
                    GT_LOG_ERROR("init listen socket failed!");
                    break;
                }

				ret = GetAcceptEXFuncAddress_();
				if (!ret) {
					GT_LOG_ERROR("Get acceptex func address failed!");
					break;
				}

				ret = GetAcceptExSockAddrsFuncAddress_();
				if (!ret) {
					GT_LOG_ERROR("Get acceptexsockaddrsfunc address failed!");
					break;
				}

                completion_port_ = CreateNewIoCompletionPort_();
                if (INVALID_HANDLE_VALUE == completion_port_) {
                    GT_LOG_ERROR("create new IOCP port failed!");
                    break;
                }

				/* create completion key for accept socket */
				accept_socket_completion_key_ = GTSERVER_RESOURCE_MANAGER.CreateNewSocketContext(listen_socket_ptr_);
				GTSERVER_RESOURCE_MANAGER.SetSocketContexAddr(accept_socket_completion_key_, serveraddr_);

                ret = BindSocketToCompletionPort(listen_socket_ptr_, (ULONG_PTR)accept_socket_completion_key_.get());
                if (!ret) {
                    GT_LOG_ERROR("bind listen socket to completion port failed!");
                    break;
                }

                PrePostAcceptEvent_();

                ret = true;
            } while (0);

			ret ? is_inited_ = ret : WSACleanup();
            return ret;
        }


		bool GT_IOCPWrapper::GetAcceptEXFuncAddress_() {
			GUID GuidAcceptEx = WSAID_ACCEPTEX;  
			DWORD dwBytes = 0;

			WSAIoctl(
				*listen_socket_ptr_,
				SIO_GET_EXTENSION_FUNCTION_POINTER,
				&GuidAcceptEx,
				sizeof(GuidAcceptEx),
				&paccpetex_func_,
				sizeof(paccpetex_func_),
				&dwBytes,
				NULL,
				NULL);

			return nullptr == paccpetex_func_ ? false : true;
		}

		bool GT_IOCPWrapper::GetAcceptExSockAddrsFuncAddress_() {
			GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
			DWORD dwBytes = 0;

			WSAIoctl(
				*listen_socket_ptr_,
				SIO_GET_EXTENSION_FUNCTION_POINTER,
				&GuidGetAcceptExSockAddrs,
				sizeof(GuidGetAcceptExSockAddrs),
				&pgetacceptex_sockaddrs_func_,
				sizeof(pgetacceptex_sockaddrs_func_),
				&dwBytes,
				NULL,
				NULL);

			return pgetacceptex_sockaddrs_func_ == nullptr ? false : true;
		}

		bool GT_IOCPWrapper::InitializeListenSocket_() {
            listen_socket_ptr_ = (GT_READ_CFG_BOOL("server_cfg", "enable_tcp_mode", 1) ? CreateOverlappedSocket_(AF_INET, SOCK_STREAM, IPPROTO_TCP) : CreateOverlappedSocket_(AF_INET, SOCK_DGRAM, IPPROTO_UDP));
            if (GT_READ_CFG_BOOL("server_cfg", "enable_tcp_mode", 1) && *listen_socket_ptr_ != INVALID_SOCKET) {

                // create socket addr
                serveraddr_.sin_family   = AF_INET;
                serveraddr_.sin_port     = htons(GT_READ_CFG_INT("server_cfg","server_port",5555));
                serveraddr_.sin_addr.S_un.S_addr = inet_addr(GT_READ_CFG_STRING("server_cfg", "server_address", "127.0.0.2").c_str());

                // bind socket to server IP
                if (!bind(*listen_socket_ptr_, (SOCKADDR*)(&serveraddr_), sizeof(SOCKADDR_IN))) {
                    int err = WSAGetLastError();
                    GT_LOG_ERROR("bind to local failed error code = " << err);
                    return false;
                }

                // set listen num
                if (!listen(*listen_socket_ptr_, SOMAXCONN)) {
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

        bool GT_IOCPWrapper::BindSocketToCompletionPort(SOCKET_SHAREPTR sock_ptr, ULONG_PTR sockcontext_completionkey) {
            HANDLE temp_port = CreateIoCompletionPort((HANDLE)(*sock_ptr), completion_port_, sockcontext_completionkey, 0);
            return temp_port == completion_port_;
        }

        void GT_IOCPWrapper::StartService(std::function<void(IO_EVENT_TYPE, SOCKETCONTEXT_SHAREPTR, IO_BUFFER_PTR)>& call_back_func_) {
			GT_TRACE_FUNCTION;
            /* create thread pool */
            std::function<void()> threadfunc = std::bind(&GT_IOCPWrapper::GetCompletionPortEventStatus, this, std::ref(call_back_func_));
            thread_pool_.Start(std::thread::hardware_concurrency() * 2, threadfunc);
        }

        GT_IOCPWrapper& GT_IOCPWrapper::GetInstance() {
            static GT_IOCPWrapper iocpinstance;
            return iocpinstance;
        }


        bool GT_IOCPWrapper::StopService() {
			GT_LOG_INFO("Now stopping service...");
			thread_pool_.Stop();
            return true;
        }

		SOCKET_SHAREPTR GT_IOCPWrapper::CreateOverlappedSocket_(int af, int type, int protocl) {
			return SOCKET_SHAREPTR(new (SOCKET)(WSASocket(af, type, protocl, nullptr, 0, WSA_FLAG_OVERLAPPED)));
		}

        void GT_IOCPWrapper::ProcessAcceptEvent_(IO_BUFFER_PTR io_context) {
            GT_LOG_DEBUG("Process Accept Event!");

			SOCKETCONTEXT_SHAREPTR completion_key = GTSERVER_RESOURCE_MANAGER.CreateNewSocketContext(io_context->GetClientSocketPtr());
			IO_BUFFER_PTR overlappe_ptr = GTSERVER_RESOURCE_MANAGER.GetIOContextBuffer();
			overlappe_ptr->SetIOBufferEventType(IO_EVENT_READ);
			overlappe_ptr->SetIOBufferSocket(io_context->GetClientSocketPtr());
			PostReadRequestEvent_(completion_key);

        }

        void GT_IOCPWrapper::PostReadRequestEvent_(SOCKETCONTEXT_SHAREPTR completion_key_) {
            GT_LOG_DEBUG("Post Read Request Event!");
            DWORD bytes_recved_ = 0;
            DWORD flag = 0;
            IO_BUFFER_PTR temp_io_ptr = GTSERVER_RESOURCE_MANAGER.GetIOContextBuffer();
            int ret = WSARecv(*(completion_key_->GetContextSocketPtr()), &temp_io_ptr->GetWsaBuf(), 1, &bytes_recved_, &flag, (LPOVERLAPPED)temp_io_ptr.get(), nullptr);
            if (ret == SOCKET_ERROR && (WSA_IO_PENDING != GetLastError())) {
                GT_LOG_ERROR("Send Socket recv event failed!");
            }
        }

        void GT_IOCPWrapper::PostWriteRequestEvent(SOCKETCONTEXT_SHAREPTR completion_key_, IO_BUFFER_PTR io_event_) {
            GT_LOG_DEBUG("Post Write Event Request!");
            DWORD transfersize = 0;
            int ret = WSASend(*(completion_key_->GetContextSocketPtr()), &io_event_->GetWsaBuf(), io_event_->GetBufferSize(), &transfersize, 0, (LPOVERLAPPED)io_event_.get(), nullptr);
            if (ret == SOCKET_ERROR && (WSA_IO_PENDING != GetLastError())) {
                GT_LOG_ERROR("Send Socket write event failed!");
            }
        }

        void GT_IOCPWrapper::PrePostAcceptEvent_() {
            /* post accept event for listen socket and post num determined by thread num */
            GT_LOG_DEBUG("Post Accept Event for listen socket!");
            for (int i = 0; i < std::thread::hardware_concurrency() * 2; i++) {
                PostAnotherAcceptEvent_();
            }
        }
        
        void GT_IOCPWrapper::PostAnotherAcceptEvent_() {
            GT_LOG_DEBUG("Post Another Accept Event for listen socket!");
            IO_BUFFER_PTR temp_ptr = GTSERVER_RESOURCE_MANAGER.GetIOContextBuffer();
            temp_ptr->SetIOBufferEventType(IO_EVENT_ACCEPT);
            temp_ptr->SetIOBufferSocket(GTSERVER_RESOURCE_MANAGER.GetCachedSocket());
            accept_socket_completion_key_->AddIOContext2Cache(temp_ptr);
            if (temp_ptr->GetClientSocketPtr() != nullptr) {

                bool ret = paccpetex_func_(*listen_socket_ptr_,
                    *(temp_ptr->GetClientSocketPtr()),
                    temp_ptr->GetBufferAddr(),
                    temp_ptr->GetBufferSize() - ((sizeof(sockaddr_in) + 16) * 2),
                    (sizeof(sockaddr_in) + 16),
                    (sizeof(sockaddr_in) + 16),
                    nullptr,
                    (LPOVERLAPPED)temp_ptr.get());

                if (ret == false && ERROR_IO_PENDING != GetLastError()) {
                    GT_LOG_ERROR("Post Accept Event Failed!");
                }
            }
        }

		void GT_IOCPWrapper::GetCompletionPortEventStatus(std::function<void(IO_EVENT_TYPE, SOCKETCONTEXT_SHAREPTR, IO_BUFFER_PTR)>& call_back_func_) {
            GT_LOG_DEBUG("Get completion port status...");
            DWORD Nnumofbytestransfered = 0;
            SOCKETCONTEXT_SHAREPTR completion_key = nullptr;
            IO_BUFFER_PTR overlapped_ptr = nullptr;
            bool ret = GetQueuedCompletionStatus(completion_port_, &Nnumofbytestransfered, (PULONG_PTR)completion_key.get(), (LPOVERLAPPED*)overlapped_ptr.get(), INFINITE);

            if (ret && Nnumofbytestransfered == 0 && overlapped_ptr->GetIOEventType() == IO_EVENT_ACCEPT) {
                GT_LOG_DEBUG("Get Accept Event!");
                ProcessAcceptEvent_(overlapped_ptr);
                PostAnotherAcceptEvent_();
            }
            else if (ret && overlapped_ptr->GetIOEventType() == IO_EVENT_READ) {
                GT_LOG_DEBUG("Get read event!");
                call_back_func_(IO_EVENT_READ, completion_key, overlapped_ptr);
				completion_key->ResetTimer();
				PostReadRequestEvent_(completion_key);
            }
            else if (ret && overlapped_ptr->GetIOEventType() == IO_EVENT_WRITE) {
                GT_LOG_DEBUG("Get write event!");
				completion_key->ResetTimer();
                call_back_func_(IO_EVENT_READ, completion_key, overlapped_ptr);
            }
            else if (Nnumofbytestransfered == 0) /* client exit */
            {
                GT_LOG_DEBUG("client exit!");
                GTSERVER_RESOURCE_MANAGER.ReleaseCompletionKey(completion_key);
                PostReadRequestEvent_(completion_key);
            }
            else {
                GT_LOG_DEBUG("Unkonwn Event!");
            }

			GTSERVER_RESOURCE_MANAGER.ReleaseIOBuffer(overlapped_ptr);
		}
    }
}
