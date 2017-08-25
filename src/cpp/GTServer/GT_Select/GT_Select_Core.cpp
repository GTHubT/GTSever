#include "GT_Select_Core.h"
#include "GTUtlity/GT_Util_GlogWrapper.h"
#include "GTUtlity/GT_Util_CfgHelper.h"
#include "GT_Select_Resource_Manager.h"
#include "GTUtlity/GT_Util_OSInfo.h"

#include <algorithm>

namespace GT {
    namespace NET {

#ifndef GT_SELECT_RESOURCE_MANAGER
#define GT_SELECT_RESOURCE_MANAGER GT_Select_Resource_Manager::GetInstance()
#endif

        GT_Select_Core::GT_Select_Core():end_thread_(false)
        {
			for (auto& i : socket_set_pos_) {
				i = 0;
			}
			for (auto&i : socket_set_total_size_) {
				i = 0;
			}
            udp_port_ = -1;
			select_cb_func_ = NULL;
			service_started_ = false;
			service_inited_ = false;
        }


        GT_Select_Core::~GT_Select_Core()
        {
        }


		bool GT_Select_Core::Initialize() {
			if (service_inited_)
				return true;

			do 
			{
				/* reset fd set and pre allocate some socket as a buffer */

				int grow_size = GT_READ_CFG_INT("select_control", "fd_grow_size", 100);
				GrowSet_(EVENT_READ, grow_size);
				GrowSet_(EVENT_WRITE, grow_size);
				GrowSet_(EVENT_EXCEPTION, grow_size);

				/* init socket environment */
				int err;
 				WORD	version = MAKEWORD(2, 2);
				WSADATA wsadata;
				err = WSAStartup(version, &wsadata);
				if (err != 0) {
					GT_LOG_ERROR("WSAStartup failed, error code = " << WSAGetLastError());
					WSACleanup();
					break;
				}
				if (LOBYTE(wsadata.wVersion) != 2 || (HIBYTE(wsadata.wVersion) != 2)) {
					GT_LOG_ERROR("no property winsock version can be use!");
					break;
				}

				/* create listen socket */
				listen_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

				/* bind local address */
				SOCKADDR_IN server_sock_addr;
				memset(&server_sock_addr, 0, sizeof(SOCKADDR_IN));
				server_sock_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
				server_sock_addr.sin_family =  AF_INET;
				server_sock_addr.sin_port = htons(GT_READ_CFG_INT("server_cfg", "server_port", 2020));
				err = bind(listen_socket_, (sockaddr*)&server_sock_addr, sizeof(SOCKADDR_IN));
				if (err != 0) {
					GT_LOG_ERROR("bind socket to local failed, error code = " << WSAGetLastError());
					break;
				}

				/* listen on the socket */
				err = listen(listen_socket_, SOMAXCONN);
				if (err != 0) {
					GT_LOG_ERROR("listen socket failed, error code = " << WSAGetLastError());
					break;
				}

				/* add listen socket to fd read sockets set */
                AddListenEvent_(listen_socket_);

				service_inited_ = true;

			} while (0);

			return service_inited_;
		}

		void GT_Select_Core::StartGTService() {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("GT Select Service Start!");
			if (service_started_)
				return;
			server_thread_ = std::thread(&GT_Select_Core::Select_service_, this);
			service_started_ = true;
		}

		void GT_Select_Core::Select_service_() {
			GT_TRACE_FUNCTION;
			int fd_count = 0;
			while (!end_thread_) {
				fd_set* readset = (fd_set*)socketset[0];
				fd_set* writeset = (fd_set*)socketset[1];
				fd_set* expset = (fd_set*)socketset[2];

				int fd_count = readset->fd_count > writeset->fd_count ? readset->fd_count > expset->fd_count ? readset->fd_count : expset->fd_count : writeset->fd_count;
				if (!fd_count) {
					GT_LOG_ERROR("Select Service Got No Socket to Serve, Just Break Out!");
					end_thread_ = true;
					return;
				}

				int ret = select(fd_count, readset, writeset, expset, NULL); /* the fd_set must have a valid socket and the fd_count must equal to the valid socket*/

				if (ret == SOCKET_ERROR) {
					GT_LOG_ERROR("got error from select, error code = " << WSAGetLastError());
					continue;
				}
				else if (!ret) {
					GT_LOG_DEBUG("select returned may got timeout!");
					continue;
				}

				if (readset->fd_array[0] > 1000)
					printf("test\n");

				for (auto& iter : readset->fd_array) {
					if (FD_ISSET(iter, readset)) {
						ProcessReadEvent_(iter);    /* in process event, do not change the address of each fd set */
					}
				}

				for (auto& iter : writeset->fd_array) {
					if (FD_ISSET(iter, writeset)) {
						ProcessWriteEvent_(iter);
					}
				}

				for (auto& iter : expset->fd_array) {
					if (FD_ISSET(iter, expset)) {
						ProcessExpEvent_(iter);
					}
				}

                RefreshFDSet_();
			}
			GT_LOG_DEBUG("service thread exit!");
		}

		void GT_Select_Core::ProcessAcceptEvent_() {
			GT_TRACE_FUNCTION;
			SOCKADDR_IN client_addr;
			int size_ = sizeof(client_addr);
			SOCKET s = accept(listen_socket_, (SOCKADDR*)&client_addr, &size_);
			AddEvent_(EVENT_ACCEPT, s);
		}

		void GT_Select_Core::ProcessExpEvent_(SOCKET& s) {
			GT_TRACE_FUNCTION;
			GT_LOG_DEBUG("Got exception event!");
		}

		void GT_Select_Core::ProcessReadEvent_(SOCKET& s) {
			if (s == listen_socket_){
				ProcessAcceptEvent_();
			}
			else {
				select_buffer* bu = GT_SELECT_RESOURCE_MANAGER.GetSelectBuffer();
                SOCKADDR_IN client_addr;
                do {

                    int size_ = sizeof(SOCKADDR_IN);
                    getpeername(s, (SOCKADDR*)&client_addr, &size_);
                    int ret = recv(s, bu->data, bu->buffer_len, NULL);

                    if (client_addr.sin_port == udp_port_) {
                        GT_LOG_DEBUG("get exit flag, service will exit!");
                        break;;
                    }

                    if (!ret || ret == SOCKET_ERROR) {
                        DelEvent_(EVENT_READ, s);
                        printf("client exit: IP: %s, port = %d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
                        GT_LOG_DEBUG("client exit, client ip addr = " << inet_ntoa(client_addr.sin_addr) << ", port = " << client_addr.sin_port);
                    }
                    else {
                        std::string str(bu->data, ret);
                        printf("get client data: %s , IP: %s, port = %d\n", str.c_str(), inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
                        DispatchEvent_(EVENT_READ, (PULONG_PTR)&s, bu->data, ret);
                    }
                } while (0);

                GT_SELECT_RESOURCE_MANAGER.ReleaseSelectBuffer(bu);
			}

		}

		void GT_Select_Core::ProcessWriteEvent_(SOCKET& s) {
			GT_TRACE_FUNCTION;
			GT_LOG_DEBUG("Got write event!");
		}

		void GT_Select_Core::AddEvent_(EVENT_TYPE type, SOCKET s) {
            new_added_client_vec_[type].insert(s);
			//if (socket_set_total_size_[type] == (*socketset)[type].sock_count) {	/* socket pos record the next used socket position */
			//	GrowSet_(type);
			//}
			//(*socketset)[type].fd_sock_array[(*socketset)[type].sock_count++] = s;
		}

        void GT_Select_Core::AddListenEvent_(SOCKET s) {
            if (socket_set_total_size_[EVENT_READ] == (*socketset)[EVENT_READ].sock_count) {	/* socket pos record the next used socket position */
                GrowSet_(EVENT_READ);
            }
            (*socketset)[EVENT_READ].fd_sock_array[(*socketset)[EVENT_READ].sock_count++] = s;
        }

		void GT_Select_Core::GrowSet_(EVENT_TYPE type, int grow_size) {
			//GT_TRACE_FUNCTION;
			if (socketset[type] == nullptr) {
				socketset[type] = (fd_set_pri*)(new char[sizeof(fd_set_pri) + sizeof(SOCKET)*(grow_size -1)]); /* the fd_set have already get one */
				socket_set_total_size_[type] = grow_size;
				socketset[type]->sock_count = 0;
			}
			else {
                    unsigned int sock_count = socketset[type]->sock_count;
                    grow_size += sock_count;
                    fd_set_pri* temp = (fd_set_pri*)(new char[sizeof(fd_set_pri) + sizeof(SOCKET)*(grow_size - 1)]);
                    temp->sock_count = sock_count;  /* sock_count is the used socket count in fd_set */
                    socket_set_total_size_[type] = grow_size;
                    for (auto i = 0; i < sock_count; i++) {
                        temp->fd_sock_array[i] = socketset[type]->fd_sock_array[i];
                    }
                    delete socketset[type];/* delete the old socket set */
                    socketset[type] = temp;
                    //memcpy(socketset[t], temp, sizeof(fd_set_pri) + sizeof(SOCKET)*grow_size);/* this place got a problem, that after memcpy, sockset[0] may overwrite the socketset[1] memory, this will lead to memory read wrong value*/
                                                                                                 /* so i place a tricky design here, once a set need grow set, i reallocate other type of set */
                    //delete[](char*)temp; /* delete the memory by the char mode to ensure the memory can be all release */
			}
		}

		void GT_Select_Core::DelEvent_(EVENT_TYPE type, SOCKET s) {
			GT_TRACE_FUNCTION;
            GT_LOG_DEBUG("Collect Socket Resource...");
            closed_client_need_clean_[type].insert(s);
			//int type_t = 0;
			//for (auto& ss : socketset) {
            //     for (int i = 0; i < ss->sock_count; i++) {
			//		if (ss->fd_sock_array[i] == s) {
            //          ss->fd_sock_array[i] = ss->fd_sock_array[--ss->sock_count];/* move the end socket behind the del index to the index of the del */
			//			break;
			//		}
			//	}
			//	type_t++;
			//}
		}

        void GT_Select_Core::RefreshFDSet_() {
			for (int type = EVENT_READ; type <= EVENT_EXCEPTION; type++) {
				std::vector<SOCKET> temp_vec;
				fd_set* fdset = (fd_set*)socketset[type];
				std::vector<int> unclean_index;
				if (closed_client_need_clean_[(EVENT_TYPE)type].empty() && new_added_client_vec_[(EVENT_TYPE)type].empty()) {
                        break;
				}
				if (socket_set_total_size_[type] == fdset->fd_count) {	/* socket position record the next used socket position */
					GrowSet_((EVENT_TYPE)type);
					fd_set* fdset = (fd_set*)socketset[type];
				}
                for (int sock_index = 0; sock_index < fdset->fd_count ; sock_index++) {
                    for (auto&item : closed_client_need_clean_[(EVENT_TYPE)type]) {     /* the socket need remove from the socket set */
						if (fdset->fd_array[sock_index] == item)
							unclean_index.push_back(sock_index);
                    }
                }
				if (unclean_index.size() > new_added_client_vec_.size()) {
					for (auto& item : new_added_client_vec_[(EVENT_TYPE)type]) {
						fdset->fd_array[fdset->fd_count++] = item;
					}

					for (auto in : unclean_index) {
						if (in == 0)
							printf("");
						if ((fdset->fd_count - 1) != in) {
							fdset->fd_array[in] = fdset->fd_array[fdset->fd_count - 1];
						}
						if (fdset->fd_count == 1)			/* it is a walk around, and should dive into select again */
							fdset->fd_array[0] = listen_socket_;
						else
							fdset->fd_count--;
					}
				}
				else {
					for (auto in : unclean_index) {
						if (in == 0)
							printf("");
						fdset->fd_array[in] = *new_added_client_vec_[(EVENT_TYPE)type].begin();
						new_added_client_vec_[(EVENT_TYPE)type].erase(new_added_client_vec_[(EVENT_TYPE)type].begin());
					}

					for (auto& item : new_added_client_vec_[(EVENT_TYPE)type]) {
						if (socket_set_total_size_[type] == fdset->fd_count) {	/* socket position record the next used socket position */
							GrowSet_((EVENT_TYPE)type);
						}
						fdset->fd_array[fdset->fd_count++] = item;
					}
				}
            }
            new_added_client_vec_.clear();
            closed_client_need_clean_.clear();
        }

		void GT_Select_Core::RegisterCallback(internal_call_back cb) {
			select_cb_func_ = cb;
		}

		void GT_Select_Core::UnRegisterCallback() {
			select_cb_func_ = NULL;
		}

		void GT_Select_Core::DispatchEvent_(EVENT_TYPE type, PULONG_PTR sock_ptr, char* data, size_t len) {
			if (!select_cb_func_)
				select_cb_func_(type, sock_ptr, data, len);
		}

		void GT_Select_Core::StopService_() {
			GT_TRACE_FUNCTION;
			if (!end_thread_) {
				end_thread_ = true;

                WakeupSelectThread_();  /* wake up select before join on the server thread */

				if (server_thread_.joinable()) {
					server_thread_.join();
				}
				GT_LOG_DEBUG("select server service thread exited!");
			}
		}


        void GT_Select_Core::WakeupSelectThread_() {
            GT_TRACE_FUNCTION;
            GT_LOG_INFO("trying wakeup select thread...");
            
            /* use UDP connect to itself to wakeup select within timeout */
                        
            SOCKET udp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);/* create a UDP socket */
            
            sockaddr_in udp_addr;
            udp_addr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
            udp_addr.sin_port = htons(0);
            udp_addr.sin_family = AF_INET;
            bind(udp_sock, (sockaddr*)&udp_addr, sizeof(sockaddr_in));/* bind the socket */

            sockaddr_in temp_addr;
            int addr_len = sizeof(temp_addr);
            getsockname(udp_sock, (sockaddr*)&temp_addr, &addr_len);
            udp_port_ = temp_addr.sin_port;

            AddEvent_(EVENT_READ, udp_sock);/* add the socket to select fd set */
           
            connect(udp_sock, (sockaddr*)&udp_addr, sizeof(sockaddr_in)); /* connect UDP socket to itself, this will wakeup the select */
        }

        
		void GT_Select_Core::CollectResource_() {
			GT_TRACE_FUNCTION;
			for (auto& iter : socketset) {
				delete iter;
			}
			
		}

		bool GT_Select_Core::Finalize() {
			GT_TRACE_FUNCTION;
			if (service_started_) {
				StopService_();
				CollectResource_();
			}
            return true;
		}

    }
}
