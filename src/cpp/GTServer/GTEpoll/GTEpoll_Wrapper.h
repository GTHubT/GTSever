#ifndef GTEPOLL_WRAPPER_H_
#define GTEPOLL_WRAPPER_H_


#include "GTEpollDefine.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <string>

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

            void CreateListenSock();

            void RegisterCallBack(GTEPOLL_CALLBACK_TYPE type, gtepoll_callback cb);

            bool UnRegisterCallBack(GTEPOLL_CALLBACK_TYPE type);

            bool StartService();

            bool StopService();

        private:
            bool InitializeCfgAndLog();

        private:
            int listen_fd_;
            int listen_port_;
            std::string cfg_path_;

        };
    }
}

#endif
