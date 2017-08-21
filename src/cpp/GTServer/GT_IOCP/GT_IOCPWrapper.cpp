#include "GT_IOCPWrapper.h"
#include "GT_Definition.h"
#include "GTUtlity/GT_Util_OSInfo.h"
#include "GTUtlity/GT_Util_GlogWrapper.h"
#include "GTUtlity/GT_Util_CfgHelper.h"

#include <stdio.h>

long long client_num_processed = 0;
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
            is_resource_worker_started_(false),
            is_iocp_thread_pool_started_(false),
            is_need_continue_wait_completion_port_status_(true)
        {
			paccpetex_func_ = nullptr;
            listen_socket_ptr_ = nullptr;
            completion_port_ = INVALID_HANDLE_VALUE;
			pgetacceptex_sockaddrs_func_ = nullptr;
			accept_socket_completion_key_ = nullptr;
        }

        bool GT_IOCPWrapper::Initialize(std::function<void(IO_EVENT_TYPE, SOCKETCONTEXT_SHAREPTR, IO_BUFFER_PTR, long)>& call_back_func_) {
			GT_TRACE_FUNCTION;
			if (is_inited_)
				return true;

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

                ret = InitializeListenSocket_();
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

				is_need_continue_wait_completion_port_status_ = false;

				InitThreadPool_(call_back_func_);

                ret = GTSERVER_RESOURCE_MANAGER.Initialize(); /* when to get the thread id is proper time */
                if (!ret) {
                    GT_LOG_ERROR("init resource manager failed!");
                    break;
                }
                is_resource_worker_started_ = true;

				/* create completion key for accept socket */
				accept_socket_completion_key_ = GTSERVER_RESOURCE_MANAGER.GetListenSocketCompletionKey(listen_socket_ptr_);
				GTSERVER_RESOURCE_MANAGER.SetSocketContexAddr(accept_socket_completion_key_, serveraddr_);

                ret = BindSocketToCompletionPort(listen_socket_ptr_, (ULONG_PTR)accept_socket_completion_key_.get());
                if (!ret) {
                    GT_LOG_ERROR("bind listen socket to completion port failed!");
                    break;
                }

                PrePostAcceptEvent_();

                ret = true;
            } while (0);

            if (ret) {
                is_inited_ = true;
            }
            else {
                WSACleanup();
                GTSERVER_RESOURCE_MANAGER.Finalize();
            }
            return is_inited_;
        }


        bool GT_IOCPWrapper::GetAcceptEXFuncAddress_() {
            GT_LOG_INFO("Get GetAcceptEx address!");
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
            GT_LOG_INFO("Get GetAcceptExSockAddrs address!");
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
            if (GT_READ_CFG_BOOL("server_cfg", "enable_tcp_mode", 1) && *(listen_socket_ptr_.get()) != INVALID_SOCKET) {

                // create socket addr
                serveraddr_.sin_family   = AF_INET;
                serveraddr_.sin_port     = htons(GT_READ_CFG_INT("server_cfg","server_port",5555));
                serveraddr_.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

                // bind socket to server IP
                if (SOCKET_ERROR == bind(*(listen_socket_ptr_.get()), (SOCKADDR*)(&serveraddr_), sizeof(SOCKADDR_IN))) {
                    int err = WSAGetLastError();
                    GT_LOG_ERROR("bind to local failed error code = " << err);
                    return false;
                }

                // set listen num
                if (SOCKET_ERROR  == listen(*listen_socket_ptr_, SOMAXCONN)) {
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

        void GT_IOCPWrapper::GTStartService() {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("GT Service start...");
			printf("GT Service start...\n");

			is_need_continue_wait_completion_port_status_ = true;

			printf("GT Service started.\n");
			GT_LOG_INFO("GT Service started");
        }

        GT_IOCPWrapper& GT_IOCPWrapper::GetInstance() {
            static GT_IOCPWrapper iocpinstance;
            return iocpinstance;
        }

		void GT_IOCPWrapper::InitThreadPool_(std::function<void(IO_EVENT_TYPE, SOCKETCONTEXT_SHAREPTR, IO_BUFFER_PTR, long)>& call_back_func_) {
			std::function<void()> threadfunc = std::bind(&GT_IOCPWrapper::GetCompletionPortEventStatus, this, std::ref(call_back_func_), std::ref(is_need_continue_wait_completion_port_status_));/* create thread pool */
			thread_pool_.Start(std::thread::hardware_concurrency() * 2, threadfunc);
			is_iocp_thread_pool_started_ = true;
		}

        bool GT_IOCPWrapper::StopService() {
            GT_LOG_INFO("Now post exit event to GT Service waiting thread!");
            PostExitEvent_();
			GT_LOG_INFO("Now stopping service...");
			if (is_iocp_thread_pool_started_) {
				GT_LOG_INFO("Now stop iocp thread pool service...");
				thread_pool_.Stop();
			}
            if (is_resource_worker_started_) {
                GT_LOG_INFO("Now stop resource manager service...");
                GTSERVER_RESOURCE_MANAGER.Finalize();
            }
            return true;
        }

		SOCKET_SHAREPTR GT_IOCPWrapper::CreateOverlappedSocket_(int af, int type, int protocl) {
			return SOCKET_SHAREPTR(new (SOCKET)(WSASocket(af, type, protocl, nullptr, 0, WSA_FLAG_OVERLAPPED)));
		}

        void GT_IOCPWrapper::ProcessAcceptEvent_(IO_BUFFER_PTR io_context) { /* use AcceptEX, when new connection comes, the first message may come together */

            GT_LOG_INFO("Process Accept Event!");
            
            int nLocalLen = 0, nRmoteLen = 0;
            LPSOCKADDR pLocalAddr, pRemoteAddr;
            pgetacceptex_sockaddrs_func_((PVOID)io_context->GetWsaBuf().buf,
                io_context->GetWsaBuf().len - ((sizeof(sockaddr_in) + 16) * 2),
                (sizeof(sockaddr_in) + 16),
                (sizeof(sockaddr_in) + 16),
                (SOCKADDR**)&pLocalAddr,
                &nLocalLen,
                (SOCKADDR**)&pRemoteAddr,
                &nRmoteLen);
            /* convert sockaddr to sockaddr_in struct */
            struct sockaddr_in *local_ipv4 = (struct sockaddr_in *)pLocalAddr;
            struct sockaddr_in *remote_ipv4 = (struct sockaddr_in *)pRemoteAddr;
            GT_LOG_INFO("get new connection and local sockaddr = " << inet_ntoa(local_ipv4->sin_addr) << ", remote sockaddr = " << inet_ntoa(remote_ipv4->sin_addr) << ", remote port = " << remote_ipv4->sin_port);
			//printf("get new connection and local sockaddr = %s, remote sockaddr = %s , remote port = %d\n", inet_ntoa(local_ipv4->sin_addr) ,inet_ntoa(remote_ipv4->sin_addr), remote_ipv4->sin_port);
			SOCKETCONTEXT_SHAREPTR completion_key = GTSERVER_RESOURCE_MANAGER.CreateNewSocketContext(io_context->GetClientSocketPtr(), ACCEPTED_SOCKET);
			IO_BUFFER_PTR overlappe_ptr = GTSERVER_RESOURCE_MANAGER.GetIOContextBuffer();
			if (overlappe_ptr == nullptr || completion_key == nullptr) {
				GT_LOG_ERROR("IO Buffer allocate failed!");
				return;
			}
			overlappe_ptr->SetIOBufferEventType(IO_EVENT_READ);
			overlappe_ptr->SetIOBufferSocket(io_context->GetClientSocketPtr());
            completion_key->SetContextSocketAddr(*remote_ipv4);
			completion_key->AddIOContext2Cache(overlappe_ptr);
			bool ret = BindSocketToCompletionPort(completion_key->GetContextSocketPtr(), (ULONG_PTR)completion_key.get());
			if (!ret) {
				GT_LOG_ERROR("bind completion key to completion port failed! error code = " << GetLastError());
				return;
			}
			PostReadRequestEvent_(completion_key, overlappe_ptr);
        }

        void GT_IOCPWrapper::PostReadRequestEvent_(SOCKETCONTEXT_SHAREPTR completion_key_, IO_BUFFER_PTR io_context) {
            GT_LOG_INFO("Post Read Request Event!");
            DWORD bytes_recved_ = 0;
            DWORD flag = 0;
            int ret = WSARecv(*(completion_key_->GetContextSocketPtr().get()), &io_context->GetWsaBuf(), 1, &bytes_recved_, &flag, (LPOVERLAPPED)io_context.get(), nullptr);
            DWORD err = WSAGetLastError();
			if (ret == SOCKET_ERROR) {
				if (WSA_IO_PENDING != err) {
					GT_LOG_ERROR("Send Socket recv event failed! error code = " << err);
					GTSERVER_RESOURCE_MANAGER.ReleaseIOBuffer(io_context);
				}
			}
        }

		void GT_IOCPWrapper::PostAnotherReadRequest_(SOCKETCONTEXT_SHAREPTR completion_key) {
			IO_BUFFER_PTR overlappe_ptr = GTSERVER_RESOURCE_MANAGER.GetIOContextBuffer();
			if (overlappe_ptr == nullptr || completion_key == nullptr) {
				GT_LOG_ERROR("IO Buffer allocate failed!");
				return;
			}
			overlappe_ptr->SetIOBufferEventType(IO_EVENT_READ);
			overlappe_ptr->SetIOBufferSocket(completion_key->GetContextSocketPtr());
			completion_key->SetContextSocketAddr(completion_key->GetSocketAddr());
			completion_key->AddIOContext2Cache(overlappe_ptr);
			PostReadRequestEvent_(completion_key, overlappe_ptr);
		}

        void GT_IOCPWrapper::PostWriteRequestEvent(SOCKETCONTEXT_SHAREPTR completion_key_, IO_BUFFER_PTR io_event_) {
            GT_LOG_INFO("Post Write Event Request!");
            DWORD transfersize = 0;
            int ret = WSASend(*(completion_key_->GetContextSocketPtr().get()), &io_event_->GetWsaBuf(), io_event_->GetBufferSize(), &transfersize, 0, (LPOVERLAPPED)io_event_.get(), nullptr);
            if (ret == SOCKET_ERROR && (WSA_IO_PENDING != WSAGetLastError())) {
                GT_LOG_ERROR("Send Socket write event failed!");
            }
        }

		void GT_IOCPWrapper::PostWriteRequestEvent(PULONG_PTR completion_key_pointer, IO_BUFFER_PTR io_event_) {
			GT_LOG_INFO("Post Write Event Request!");
			DWORD transfersize = 0;
			int ret = WSASend(*(((GT_SocketConetxt*)(completion_key_pointer))->GetContextSocketPtr().get()), &io_event_->GetWsaBuf(), io_event_->GetBufferSize(), &transfersize, 0, (LPOVERLAPPED)io_event_.get(), nullptr);
			if (ret == SOCKET_ERROR && (WSA_IO_PENDING != WSAGetLastError())) {
				GT_LOG_ERROR("Send Socket write event failed!");
			}
		}

        void GT_IOCPWrapper::PrePostAcceptEvent_() {
            /* post accept event for listen socket and post num determined by thread num */
            GT_LOG_INFO("Post Accept Event for listen socket!");
            for (int i = 0; i < std::thread::hardware_concurrency() * 2; i++) {
                PostAnotherAcceptEvent_();
            }
        }
        
		void GT_IOCPWrapper::PostAnotherAcceptEvent_() {
            GT_LOG_INFO("Post Another Accept Event for listen socket!");
			int try_time = 0;
			while( try_time < 2){
				IO_BUFFER_PTR temp_ptr = GTSERVER_RESOURCE_MANAGER.GetIOContextBuffer();
				if (temp_ptr == nullptr) {
					GT_LOG_ERROR("IO Buffer allocate failed!");
					return;
				}
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

					DWORD err = WSAGetLastError();
					if (ret == false && ERROR_IO_PENDING != err) {
						GT_LOG_ERROR("Post Accept Event Failed! error code = " << err << ", try again!");
						GTSERVER_RESOURCE_MANAGER.ReleaseIOBuffer(temp_ptr);
					}
					else {
                        GT_LOG_INFO("Post Accept Event Success!");
                        return;
					}
				}
				try_time++;
			};
        }

		void GT_IOCPWrapper::SendDataUserInterface(PULONG_PTR completion_key_pointer, char* data, size_t len) {
			IO_BUFFER_PTR io_ptr = GTSERVER_RESOURCE_MANAGER.GetIOContextBuffer();
			if (io_ptr == nullptr) {
				GT_LOG_ERROR("IO Buffer allocate failed!");
				return;
			}
			io_ptr->AllocateIOBufferBySize(len);
			memcpy(io_ptr->GetWsaBuf().buf, data, len);
			PostWriteRequestEvent(completion_key_pointer, io_ptr);
		}


		//FIXME: when exit, should send message to exit GetQueuedCompletionStatus, so that the process can exit elegant // FIXED!
		void GT_IOCPWrapper::GetCompletionPortEventStatus(std::function<void(IO_EVENT_TYPE, SOCKETCONTEXT_SHAREPTR, IO_BUFFER_PTR, long)>& call_back_func_, std::atomic_bool& is_need_continue_wait) {
			if (!is_need_continue_wait) {
				//GT_LOG_INFO("[Thread] " << std::this_thread::get_id() << ":, its time to end GT Service, Do not need continue wait for completion port...");
                return;
            }
            //GT_LOG_DEBUG("Get completion port status...");
            DWORD Nnumofbytestransfered = 0;

			ULONG_PTR completion_key;
			OVERLAPPED* overlapped;
			bool ret = GetQueuedCompletionStatus(completion_port_, &Nnumofbytestransfered, &completion_key, &overlapped, INFINITE);
			
			
			GT_SocketConetxt* gt_context = (GT_SocketConetxt*)completion_key;
			GT_IOContextBuffer* gt_io = (GT_IOContextBuffer*)overlapped;
			if (gt_io == NULL || gt_context == NULL || gt_io->GetIOEventType() == IO_EVENT_NULL)
			{
				GT_LOG_ERROR("completion key or io buffer empty...");	/* why the logic can occur at this place ??? */
				return;
			}

			SOCKETCONTEXT_SHAREPTR gt_completion_key_ptr;;
			IO_BUFFER_PTR gt_io_buffer_ptr;


			if (gt_io->GetIOEventType() == IO_EVENT_ACCEPT || gt_io->GetIOEventType() == IO_EVENT_EXIT) { /* if the event is listen socket's event, just return the listen socket completion key */
				gt_completion_key_ptr = accept_socket_completion_key_; 
				gt_io_buffer_ptr = gt_completion_key_ptr->GetIOBufferPtr((ULONG_PTR)gt_io);
			}
			else {
				gt_completion_key_ptr = GTSERVER_RESOURCE_MANAGER.GetCompletionKeyPtr((ULONG_PTR)gt_context);
				if (gt_completion_key_ptr != nullptr) {
					gt_io_buffer_ptr = gt_completion_key_ptr->GetIOBufferPtr((ULONG_PTR)gt_io);
				}
			}

			if (gt_io_buffer_ptr == nullptr || gt_completion_key_ptr == nullptr) {
				GT_LOG_ERROR("completion key or io buffer context empty...");
				GTSERVER_RESOURCE_MANAGER.ReleaseIOBuffer(gt_io_buffer_ptr);
				return;
			}

			if (ret && gt_io->GetIOEventType() == IO_EVENT_EXIT) {
				GT_LOG_ERROR("Get exit IO event, set the is_need_continue_wait flag to false!");
				is_need_continue_wait = false;
			}
			else if (ret && gt_io_buffer_ptr->GetIOEventType() == IO_EVENT_ACCEPT) { /* when accept the socket, AccepEX will get the first data, so the Nnumofbytestransfered may not zero, if the client send data after connect success!*/
                GT_LOG_INFO("Get Accept Event!");
				++ client_num_processed;
				if (Nnumofbytestransfered != 0) { /* the accept socket send data after connect success */
					GT_LOG_INFO("the accept socket bind the first data!");
					call_back_func_(IO_EVENT_READ, gt_completion_key_ptr, gt_io_buffer_ptr, Nnumofbytestransfered);
				}
                ProcessAcceptEvent_(gt_io_buffer_ptr);
                PostAnotherAcceptEvent_();
				gt_completion_key_ptr->ReleaseUsedIOContext(gt_io_buffer_ptr);
            }
            else if (ret && gt_io_buffer_ptr->GetIOEventType() == IO_EVENT_READ && Nnumofbytestransfered != 0 ) {
                GT_LOG_INFO("Get read event from : " << inet_ntoa(gt_completion_key_ptr->GetSocketAddr().sin_addr));
                call_back_func_(IO_EVENT_READ, gt_completion_key_ptr, gt_io_buffer_ptr, Nnumofbytestransfered);
                gt_completion_key_ptr->ResetTimer();
				PostAnotherReadRequest_(gt_completion_key_ptr);
				gt_completion_key_ptr->ReleaseUsedIOContext(gt_io_buffer_ptr);
            }
            else if (ret && gt_io_buffer_ptr->GetIOEventType() == IO_EVENT_WRITE) {
                GT_LOG_INFO("Get write event from : " << inet_ntoa(gt_completion_key_ptr->GetSocketAddr().sin_addr));
                gt_completion_key_ptr->ResetTimer();
                call_back_func_(IO_EVENT_READ, gt_completion_key_ptr, gt_io_buffer_ptr, Nnumofbytestransfered);
				gt_completion_key_ptr->ReleaseUsedIOContext(gt_io_buffer_ptr);
            }
            else if (gt_io_buffer_ptr->GetIOEventType() == IO_EVENT_READ && Nnumofbytestransfered == 0) /* client exit */
            {
                GT_LOG_DEBUG("client exit : " << inet_ntoa(gt_completion_key_ptr->GetSocketAddr().sin_addr));
                GTSERVER_RESOURCE_MANAGER.ReleaseCompletionKey(gt_completion_key_ptr);
            }
            else if (!ret) {
				GTSERVER_RESOURCE_MANAGER.ReleaseIOBuffer(gt_io_buffer_ptr);
                GT_LOG_ERROR("GetQueuedCompletionStatus failed, error code = " << WSAGetLastError());
            }
			else {
				GTSERVER_RESOURCE_MANAGER.ReleaseIOBuffer(gt_io_buffer_ptr);
                GT_LOG_DEBUG("Unkonwn Event! ret = " << ret << ", Nnumofbytestransfered = " << Nnumofbytestransfered);
			}
		}

        void GT_IOCPWrapper::PostExitEvent_() {
            GT_LOG_INFO("Post Exit Event.");
            for (int i = 0; i < std::thread::hardware_concurrency() * 2; i++) {
                IO_BUFFER_PTR temp_ptr = GTSERVER_RESOURCE_MANAGER.GetIOContextBuffer();
				if (temp_ptr == nullptr) {
					GT_LOG_ERROR("IO Buffer allocate failed!");
					return;
				}
                temp_ptr->SetIOBufferEventType(IO_EVENT_EXIT);
                temp_ptr->SetIOBufferSocket(GTSERVER_RESOURCE_MANAGER.GetCachedSocket());
				accept_socket_completion_key_->AddIOContext2Cache(temp_ptr);
				PostQueuedCompletionStatus(completion_port_,  0, (ULONG_PTR)accept_socket_completion_key_.get(), (LPOVERLAPPED)temp_ptr.get());
            }
        }
    }
}
