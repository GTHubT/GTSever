#include "GTEpoll_Wrapper.h"
#include "../GTUtlity/GT_Util_GlogWrapper.h"
#include "../GTUtlity/GT_Util_CfgHelper.h"
#include "./GTEpollUtility/GTEpoll_Utility.h"
#include "./GTEpollUtility/GTEpoll_thread_pool.h"

#include <stdio.h>
#include <arpa/inet.h>
#include <string.h> // for bzero
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <wait.h>
#include <algorithm>
#include <vector>
#include <thread>
#include <string>
#include <sys/types.h>


static void sig_handle(int sig, siginfo_t* siginfo, void* ucontext){
    printf("get signal %d, child process killed!\n", sig);
    printf("siginfo content killed pid = %d\n", siginfo->si_pid);
}

#define MAX_BUFFER 2048

namespace GT{

    namespace EPOLL{

        GTEpollWrapper::GTEpollWrapper():listen_port_(0),
                                         thread_or_proc_num_(0),
                                         use_multi_process_(false){

        }

        GTEpollWrapper::~GTEpollWrapper() {

        }

        GTEpollWrapper& GTEpollWrapper::getInstance() {
            static GTEpollWrapper gtepoll_instance_;
            return gtepoll_instance_;
        }

        bool GTEpollWrapper::initialize(std::string cfgpath) {
            cfg_path_ = cfgpath;
            bool ret = initializeCfgAndLog_();

            // add signal process function, and use sigaction with SA_SIGINFO parameter to get more detail info
            struct sigaction sigact;
            bzero(&sigact, sizeof(sigact));
            sigact.sa_sigaction = sig_handle;
            sigact.sa_flags = SA_SIGINFO;           // if use SA_SIGINFO handle should declare like sig_handle(int sig, siginfo_t* siginfo, void* ucontext)
            sigemptyset(&sigact.sa_mask);
            sigaddset(&sigact.sa_mask, SIGQUIT);
            sigaction(SIGHUP, &sigact, 0);

            return ret;
        }

        bool GTEpollWrapper::initializeCfgAndLog_() {
            bool ret = GT::UTIL::GT_Util_CfgHelper::LoadCfg(cfg_path_);
            if (!ret){
                printf("get cfg failed!\n");
                return false;
            }

            listen_port_ = (u_short)GT_READ_CFG_INT("server_cfg","server_port",5000);
            use_multi_process_ = GT_READ_CFG_BOOL("epoll_control","use_multi_process", 0);
            max_events_num_ = GT_READ_CFG_INT("epoll_control","event_num_pertime",128);
            thread_or_proc_num_ = GT_READ_CFG_INT("epoll_control","thread_or_process_num", std::thread::hardware_concurrency());

            if (!(ret = GT::UTIL::GT_Util_GlogWrapper::GetInstance().GT_DefaultLogInitialize())){
                printf("log initialize failed!\n");
            }
            return ret;
        }

        void GTEpollWrapper::registerCallBack(GTEPOLL_CALLBACK_TYPE type, gtepoll_callback cb) {
            GT_TRACE_FUNCTION;
            switch (type){
                case GTEPOLL_READ:
                    GT_LOG_DEBUG("set read callback");
                    read_cb_ = cb;
                    break;
                case GTEPOLL_CONN:
                    GT_LOG_DEBUG("set connect callback");
                    conn_cb_ = cb;
                    break;
                default:
                    GT_LOG_DEBUG("unknown register type!");
                    break;
            }
        }

        void GTEpollWrapper::unRegisterCallBack(GTEPOLL_CALLBACK_TYPE type) {
            GT_TRACE_FUNCTION;
            switch (type){
                case GTEPOLL_READ:
                    GT_LOG_DEBUG("set read callback");
                    read_cb_ = nullptr;
                    break;
                case GTEPOLL_CONN:
                    GT_LOG_DEBUG("set connect callback");
                    conn_cb_ = nullptr;
                    break;
                default:
                    GT_LOG_DEBUG("unknow register type!");
                    break;
            }
        }

        int GTEpollWrapper::createListenSock_() {
            GT_TRACE_FUNCTION;

            int listen_fd = socket(AF_INET, IPPROTO_TCP, 0);

            sockaddr_in sock_addr = {0};
            bzero(&sock_addr, sizeof(sockaddr_in));
            sock_addr.sin_family = AF_INET;
            sock_addr.sin_port = htons(listen_port_);
            sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);

            bool ret = false;
            do {
                // set socket to no block
                if (!(ret = GT::EpollUtil::GTEpoll_Util::setsocket2noblock(listen_fd))){
                    printf("set socket to no block failed!\n");
                    GT_LOG_ERROR("set socket to no block failed!");
                    break;
                }

                // set socket to reuse addr
                if (!(ret = GT::EpollUtil::GTEpoll_Util::setsockop(listen_fd, SO_REUSEADDR))){
                    printf("set socket to reuse addr failed!\n");
                    GT_LOG_ERROR("set socket to reuse addr failed!");
                    break;
                }

                // set socket to reuse port
                if (!(ret = GT::EpollUtil::GTEpoll_Util::setsockop(listen_fd, SO_REUSEPORT))){ // reuse port is only avaliable from kernal ver:3.9 or later
                    printf("set socket to reuse PORT failed!\n");
                    GT_LOG_ERROR("set socket to reuse PORT failed!");
                    break;
                }

                socklen_t len = sizeof(sock_addr);
                int retu = -1;
                if (0 == (retu = bind(listen_fd, (sockaddr*)&sock_addr, len))){
                    printf("bind socket to local failed!\n");
                    GT_LOG_ERROR("bind socket to local failed!");
                    break;
                }
            }while (0);

            return listen_fd;
        }

        void GTEpollWrapper::startService() {
            GT_TRACE_FUNCTION;
            use_multi_process_?startByMultiprocess_():startByMultithread_();
        }

        bool GTEpollWrapper::stopService() {
            GT_TRACE_FUNCTION;
        }

        void GTEpollWrapper::startByMultiprocess_() {
            GT_TRACE_FUNCTION;
            for(int i =0;i< thread_or_proc_num_;i++){
                pid_t pid = fork();
                if (pid == 0){
                    workerFunc_();
                }
            }

            int status = -1;
            while(waitpid(0, &status, 0)>0){

            }
        }

        void GTEpollWrapper::startByMultithread_() {
            GT_TRACE_FUNCTION;
            std::vector<std::thread> th_vec;
            for (int i=0; i<thread_or_proc_num_;i++){
                th_vec.push_back(std::thread(&GTEpollWrapper::workerFunc_, this));
            }
            std::for_each(th_vec.begin(),th_vec.end(), [](std::thread &x)->void{x.join();});
        }

        void GTEpollWrapper::workerFunc_() { // worker function
            int listen_fd = createListenSock_();

            // create epoll fd
            int epfd = epoll_create(1); // 1 is not use in the kernal
            epoll_event ev[max_events_num_]={0}, nev={0};
            nev.events |= EPOLLET|EPOLLIN;
            nev.data.fd = listen_fd;

            int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &nev);
            if (!ret){
                goto ctl_err;
            }

            for(;;){

                ret = epoll_wait(epfd, ev, max_events_num_, 2000);

                if (ret == 0){
                    GT_LOG_DEBUG("epoll wait timeout!");
                    continue;
                }else if(ret == -1){
                    GT_LOG_ERROR("epoll got error break out, error code = "<<errno);
                    break;
                }else if (ret > 0) {
                    GT_LOG_DEBUG("get event, num = " << ret);
                    for (int i = 0; i < ret; i++){
                        if(ev[i].data.fd == listen_fd && ev[i].events&EPOLLIN){
                            struct sockaddr_in client_addr_;
                            socklen_t sock_add_len =  sizeof(sockaddr);
                            int new_conn = accept(listen_fd, (sockaddr*)&client_addr_, &sock_add_len);
                            GT::EpollUtil::GTEpoll_Util::setsocket2noblock(new_conn);
                            bool rt = addNewConn2Epoll_(new_conn, epfd, &client_addr_);
                            conn_cb_((void*)&new_conn, sizeof(new_conn), nullptr);
                            if(!rt){
                                GT_LOG_ERROR("add new connect to epoll failed!");
                                continue;
                            }
                        }else if(ev[i].events&EPOLLIN){
                            int fd = ev[i].data.fd;
                            std::string recv_content_;
                            char* rvbuffer = new char[MAX_BUFFER];
                            for(;;){
                                ssize_t rl = recv(fd, rvbuffer, MAX_BUFFER, 0);
                                if (rl < MAX_BUFFER || errno == EAGAIN || errno == EWOULDBLOCK){
                                    if(client_state_.find(fd) != client_state_.end()){
                                        client_state_[fd]->is_read_finished_ = true;
                                    }
                                    break;
                                }
                                recv_content_.append(rvbuffer, rl);
                                bzero(rvbuffer, MAX_BUFFER);
                            }
                            delete[] rvbuffer;
                            read_cb_((void*)recv_content_.c_str(),recv_content_.length(), nullptr);
                        }else if(ev[i].events&EPOLLOUT){
                            procSendEvents(ev[i].data.fd);
                        }else if(ev[i].events&EPOLLERR | ev[i].events&EPOLLHUP){
                            shutdown(ev[i].data.fd, 2);
                            close(ev[i].data.fd);
                            rmClientFromMap(ev[i].data.fd);
                            GT_LOG_DEBUG("get err events!");
                            continue;
                        }else{
                            GT_LOG_DEBUG("unknown event type");
                        }
                    }
                }
            }

            ctl_err:
                GT_LOG_ERROR("add listen fd to epfd failed, err = " << errno);
                close(listen_fd);
                close(epfd);

        }

        void GTEpollWrapper::sendData(int fd, void* data, unsigned long len){
            GT_TRACE_FUNCTION;
            if (len<=0 || data == nullptr){
                GT_LOG_WARN("data len or data content is not invalid!");
                return;
            }
            if (client_state_.find(fd) != client_state_.end()){
                ssize_t sl = send(fd, data, len, 0);
                if (sl == len){
                    GT_LOG_DEBUG("all data was send to client:" << client_state_[fd]->ip << ", port = " << client_state_[fd]->port);
                    client_state_[fd]->is_write_finished_ = true;
                }else if(errno == EAGAIN){  // if not send all the data, we will record the offset for the next send when the socket send buffer is available
                    client_state_[fd]->is_write_finished_ = false;
                    client_state_[fd]->content_remain_len_ = len - sl;
                    if (client_state_[fd]->content_ )
                        delete[] client_state_[fd]->content_;
                    client_state_[fd]->content_ = new char[len-sl];
                    memcpy(client_state_[fd]->content_, data + sl, len-sl);
                    GT_LOG_DEBUG("client " << client_state_[fd]->ip << ", did not send all the data and need send next time. send len = " << sl);
                }else{
                    GT_LOG_ERROR("client"<< client_state_[fd]->ip << " send data failed, errno = "<< errno);
                }
            }
        }

        bool GTEpollWrapper::addNewConn2Epoll_(int newconn, int epfd, sockaddr_in* addr){
            epoll_event ev = {0};
            ev.events |= EPOLLIN|EPOLLET;  // EPOLLRDHUP means other side close the socket
            ev.data.fd = newconn;
            int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, newconn, &ev);
            if (!ret){
                GT_LOG_ERROR("add listen fd to epfd failed, err = " << errno);
                close(newconn);
                return false;
            }
            char client_ip[20];
            inet_ntop(AF_INET,&addr->sin_addr,client_ip, sizeof(client_ip));
            GT_LOG_DEBUG("get new connect from:" << client_ip << ", port = " << ntohs(addr->sin_port));
            push2ClientMap(newconn, client_ip, addr->sin_port);
            return true;
        }

        void GTEpollWrapper::push2ClientMap(int fd, char* ip, short port) {
            GT_TRACE_FUNCTION;
            if (client_state_.find(fd) != client_state_.end()){
                std::shared_ptr<sock_state> ss(new sock_state());
                ss->client_fd_ = fd;
                memcpy(ss->ip, ip, sizeof(ss->ip));
                ss->port = port;
                client_state_.insert(std::make_pair(fd, ss));
            }else{
                GT_LOG_DEBUG("Did not find the sock fd in map!");
            }
        }

        void GTEpollWrapper::rmClientFromMap(int fd) {
            GT_TRACE_FUNCTION;
            if (client_state_.find(fd) != client_state_.end()){
                client_state_[fd].reset();
                client_state_.erase(fd);
            }
        }

        void GTEpollWrapper::procSendEvents(int fd) {   // when the EPOLLOUT event setup, it means that the send buffer is now available to send more data
            GT_TRACE_FUNCTION;
            if(client_state_.find(fd) != client_state_.end()){
                if (!client_state_[fd]->is_write_finished_){
                    sendData(fd, client_state_[fd]->content_, client_state_[fd]->content_remain_len_);
                }
            }
        }

    }
}