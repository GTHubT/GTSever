#include "GTEpoll_Wrapper.h"
#include "../GTUtlity/GT_Util_GlogWrapper.h"
#include "../GTUtlity/GT_Util_CfgHelper.h"
#include "./GTEpollUtility/GTEpoll_Utility.h"
#include "./GTEpollUtility/GTEpoll_thread_pool.h"
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h> // for bzero
#include <thread>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <wait.h>


static void sig_handle(int sig, siginfo_t* siginfo, void* ucontext){
    printf("get signal %d, child process killed!\n", sig);
    printf("siginfo content killed pid = %d\n", siginfo->si_pid);
}


namespace GT{

    namespace EPOLL{

        GTEpollWrapper::GTEpollWrapper():listen_port_(0),
                                         thread_or_proc_num_(0),
                                         use_multi_process_(false){

        }

        GTEpollWrapper::~GTEpollWrapper() {

        }

        GTEpollWrapper& GTEpollWrapper::GetInstance() {
            static GTEpollWrapper gtepoll_instance_;
            return gtepoll_instance_;
        }

        bool GTEpollWrapper::Initialize(std::string cfgpath) {
            cfg_path_ = cfgpath;
            bool ret = InitializeCfgAndLog_();

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

        bool GTEpollWrapper::InitializeCfgAndLog_() {
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

        void GTEpollWrapper::RegisterCallBack(GTEPOLL_CALLBACK_TYPE type, gtepoll_callback cb) {
            GT_TRACE_FUNCTION;
            switch (type){
                case GTEPOLL_READ:
                    GT_LOG_DEBUG("set read callback");
                    read_cb_ = cb;
                    break;
                case GTEPOLL_WRITE:
                    GT_LOG_DEBUG("set write callback");
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

        void GTEpollWrapper::UnRegisterCallBack(GTEPOLL_CALLBACK_TYPE type) {
            GT_TRACE_FUNCTION;
            switch (type){
                case GTEPOLL_READ:
                GT_LOG_DEBUG("set read callback");
                    read_cb_ = nullptr;
                    break;
                case GTEPOLL_WRITE:
                GT_LOG_DEBUG("set write callback");
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

        int GTEpollWrapper::CreateListenSock_() {
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

        void GTEpollWrapper::StartService() {
            GT_TRACE_FUNCTION;
            use_multi_process_?StartByMultiprocess_():StartByMultithread_();
        }

        bool GTEpollWrapper::StopService() {
            GT_TRACE_FUNCTION;
        }

        void GTEpollWrapper::StartByMultiprocess_() {
            GT_TRACE_FUNCTION;
            for(int i =0;i< thread_or_proc_num_;i++){
                pid_t pid = fork();
                if (pid == 0){
                    WorkerFunc_();
                }
            }

            int status = -1;
            while(waitpid(0, &status, 0)>0){

            }
        }

        void GTEpollWrapper::StartByMultithread_() {
            GT_TRACE_FUNCTION;
        }

        void GTEpollWrapper::WorkerFunc_() { // worker function
            int listen_fd = CreateListenSock_();

            // create epoll fd
            int epfd = epoll_create(1); // 1 is not use in the kernal
            epoll_event ev[max_events_num_]={0}, nev={0};
            nev.events |= EPOLLET|EPOLLIN;
            nev.data.fd = listen_fd;

            int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &nev);
            if (!ret){
                GT_LOG_ERROR("add listen fd to epfd failed, err = " << errno);
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
                            GT_LOG_DEBUG("get new connect");
                            conn_cb_((void*)&ev[i], 1);
                            int new_conn = accept(listen_fd, nullptr, 0);
                            bool rt = AddNewConn2Epoll_(new_conn, epfd);
                            if(!rt){
                                GT_LOG_ERROR("add new connect to epoll failed!");
                                continue;
                            }
                        }else if(ev[i].events&EPOLLIN){
                            read_cb_((void*)&ev[i],1);
                        }else if(ev[i].events&EPOLLOUT){
                            write_cb_((void*)&ev[i],1);
                        }else if(ev[i].events&EPOLLERR | ev[i].events&EPOLLHUP){
                            shutdown(ev[i].data.fd, 2);
                            close(ev[i].data.fd);
                            GT_LOG_DEBUG("get err events!")
                            continue;
                        }else{
                            GT_LOG_DEBUG("unknown event type");
                        }
                    }
                }
            }

            ctl_err:
                close(listen_fd);
                close(epfd);

        }

        bool GTEpollWrapper::AddNewConn2Epoll_(int newconn, int epfd){
            epoll_event ev = {0};
            ev.events |= EPOLLIN|EPOLLET|EPOLLRDHUP;  // EPOLLRDHUP means other side close the socket
            ev.data.fd = newconn;
            int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, newconn, &ev);
            if (!ret){
                GT_LOG_ERROR("add listen fd to epfd failed, err = " << errno);
                close(newconn);
                return false;
            }
            return true;
        }

    }
}