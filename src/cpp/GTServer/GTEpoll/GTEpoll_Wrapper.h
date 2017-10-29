#ifndef GTEPOLL_WRAPPER_H_
#define GTEPOLL_WRAPPER_H_


#include "GTEpollDefine.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <string>
#include <netinet/in.h>
#include <map>
#include <memory>

namespace GT {

    namespace EPOLL {

        struct sock_state{
            int     client_fd_;
            bool    is_read_finished_;
            bool    is_write_finished_;
            int     offset_;        // if the op is not finished, offset record the position of the op
            void*   content_;       // record the content need to send but not send complete
            int     content_len_;
            sock_state():client_fd_(-1),is_read_finished_(-1),is_write_finished_(-1),offset_(-1),content_(nullptr),content_len_(-1){

            }
        };

        class GTEpollWrapper {
        public:
            ~GTEpollWrapper();

        private:
            GTEpollWrapper();

        public:
            static GTEpollWrapper &getInstance();

            bool initialize(std::string cfgpath);

            void registerCallBack(GTEPOLL_CALLBACK_TYPE type, gtepoll_callback cb);

            void unRegisterCallBack(GTEPOLL_CALLBACK_TYPE type);

            void startService();

            bool stopService();

            void sendData(int fd, void* data, unsigned long len);
        private:

            bool initializeCfgAndLog_();

            int  createListenSock_();

            void startByMultiprocess_();

            void startByMultithread_();

            void workerFunc_();

            bool addNewConn2Epoll_(int newconn, int epfd, sockaddr_in*);

            void push2ClientMap(int fd);

            void rmClientFromMap(int fd);

            void procSendEvents(int fd);

        private:
            int thread_or_proc_num_;
            u_short listen_port_;
            int max_events_num_;
            std::string cfg_path_;
            bool use_multi_process_;

            std::map<int,std::shared_ptr<sock_state>> client_state_;

            gtepoll_callback read_cb_;
            gtepoll_callback write_cb_;
            gtepoll_callback conn_cb_;
        };
    }
}

#endif
