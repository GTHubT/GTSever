#include "GT_Select_Core.h"
#include "GTUtlity/GT_Util_GlogWrapper.h"
#include "GTUtlity/GT_Util_CfgHelper.h"

#include <algorithm>

namespace GT {
    namespace NET {

        GT_Select_Core::GT_Select_Core():end_thread_(false)
        {
			
			for (auto& i : socket_set_pos_) {
				i = 0;
			}
			select_cb_func_ = NULL;
        }


        GT_Select_Core::~GT_Select_Core()
        {
        }


		bool GT_Select_Core::Initialize() {
			bool ret = false;

			do 
			{
				/* reset fd set */
				for (auto& iter : socketset) {
					FD_ZERO((fd_set*)&iter);
				}

				/* init socket environment */
				int err;
				WORD	version = MAKEWORD(2, 2);
				WSADATA wsadata;
				err = WSAStartup(version, &wsadata);
				if (!err) {
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
				server_sock_addr.sin_addr.S_un.S_addr = inet_addr(INADDR_ANY);
				server_sock_addr.sin_family =  AF_INET;
				server_sock_addr.sin_port = htons(GT_READ_CFG_INT("server_cfg", "server_port", 2020));
				err = bind(listen_socket_, (sockaddr*)&server_sock_addr, sizeof(SOCKADDR_IN));
				if (!err) {
					GT_LOG_ERROR("bind socket to local failed, error code = " << WSAGetLastError());
					break;
				}

				/* listen on the socket */
				err = listen(listen_socket_, SOMAXCONN);
				if (!err) {
					GT_LOG_ERROR("listen socket failed, error code = " << WSAGetLastError());
					break;
				}

				/* add listen socket to fd read sockets set */
				AddEvent_(EVENT_ACCEPT, listen_socket_);

				ret = true;

			} while (0);

			return ret;
		}

		void GT_Select_Core::StartService() {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("GT Select Service Start!");
			server_thread_ = std::thread(&GT_Select_Core::Select_service_, this);
		}

		void GT_Select_Core::Select_service_() {
			GT_TRACE_FUNCTION;
			while (!end_thread_) {
				fd_set_pri& readset = socketset[0];
				fd_set_pri& writeset = socketset[1];
				fd_set_pri& expset = socketset[2];

				int fd_count = readset.sock_count > writeset.sock_count ? readset.sock_count > expset.sock_count ? readset.sock_count : expset.sock_count : writeset.sock_count;
				if (!fd_count) {
					GT_LOG_ERROR("Select Service Got No Socket to Serve, Just Break Out!");
					end_thread_ = true;
					return;
				}

				int ret = select(NULL, (fd_set*)&readset, (fd_set*)&writeset, (fd_set*)&expset, NULL);

				if (ret == SOCKET_ERROR) {
					GT_LOG_ERROR("got error from select, error code = " << WSAGetLastError());
					continue;
				}
				else if (!ret) {
					GT_LOG_DEBUG("select returned may got timeout!");
					continue;
				}

				
			}
		}

		void GT_Select_Core::AddEvent_(EVENT_TYPE type, SOCKET s) {
		
			if (socket_set_pos_[type] == socketset[type].sock_count) {	/* socket pos record the next used socket position */
				GrowSet_(type);
			}
			socketset[type].fd_sock_array[socket_set_pos_[type]] = s;
			socket_set_pos_[type] ++;
		}

		void GT_Select_Core::GrowSet_(EVENT_TYPE type) {
			GT_TRACE_FUNCTION;
			int grow_size = GT_READ_CFG_INT("select_control", "fd_grow_size", 100);
			SOCKET* set_pos = socketset[type].fd_sock_array + socket_set_pos_[type];
			set_pos = new SOCKET[grow_size];
		}

		void GT_Select_Core::DelEvent_(EVENT_TYPE, SOCKET, int index) {

		}

		void GT_Select_Core::RegisterCallback(gt_event_callback cb) {
			select_cb_func_ = cb;
		}

		void GT_Select_Core::UnRegisterCallback() {
			select_cb_func_ = NULL;
		}

		void GT_Select_Core::DispatchEvent_(EVENT_TYPE type, ULONG_PTR sock_ptr, char* data, size_t len) {
			select_cb_func_(type, sock_ptr, data, len);
		}

		void GT_Select_Core::StopService() {

		}

		bool GT_Select_Core::Finalize() {
            return false;
		}

    }
}
