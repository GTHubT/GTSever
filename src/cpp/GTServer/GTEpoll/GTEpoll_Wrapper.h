#ifndef GTEPOLL_WRAPPER_H_
#define GTEPOLL_WRAPPER_H_


#include "GTEpollDefine.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <string>
#include <netinet/in.h>

namespace GT {

    namespace EPOLL {

        class GTEpollWrapper {
        public:
            ~GTEpollWrapper();

        private:
            GTEpollWrapper();

        public:
            static GTEpollWrapper &GetInstance();

            bool Initialize(std::string cfgpath);

            void RegisterCallBack(GTEPOLL_CALLBACK_TYPE type, gtepoll_callback cb);

            void UnRegisterCallBack(GTEPOLL_CALLBACK_TYPE type);

            bool StartService();

            bool StopService();

        private:
            bool InitializeCfgAndLog_();

            bool CreateListenSock_();

            void StartByMultiprocess_();

            void StartByMultithread_();

        private:
            int listen_fd_;
            u_short listen_port_;
            u_int max_events_num_;
            std::string cfg_path_;
            bool use_multi_process_;
            struct sockaddr_in sock_addr_;

            gtepoll_callback read_cb_;
            gtepoll_callback write_cb_;
            gtepoll_callback conn_cb_;
        };
    }
}

#endif
