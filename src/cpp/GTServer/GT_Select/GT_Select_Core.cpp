#include "GT_Select_Core.h"
#include "GTUtlity/GT_Util_GlogWrapper.h"
#include "GTUtlity/GT_Util_CfgHelper.h"


namespace GT {
    namespace NET {

        GT_Select_Core::GT_Select_Core()
        {
			select_cb_func_ = NULL;
        }


        GT_Select_Core::~GT_Select_Core()
        {
        }


		bool GT_Select_Core::Initialize() {
			bool ret = false;

			do 
			{
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
				ret = true;

			} while (0);

			return ret;
		}

		void GT_Select_Core::StartService() {
			
		}

		void GT_Select_Core::Select_service_() {

		}

		void GT_Select_Core::RegisterCallback(gt_event_callback cb) {
			select_cb_func_ = cb;
		}

		void GT_Select_Core::UnRegisterCallback() {
			select_cb_func_ = NULL;
		}

		void GT_Select_Core::DispatchEvent_(EVENT_TYPE type, ULONG_PTR sock_ptr, char* data, size_t len) {

		}

		void GT_Select_Core::StopService() {

		}

		bool GT_Select_Core::Finalize() {
            return false;
		}

    }
}
