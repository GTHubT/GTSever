#include "GTEpoll_Wrapper.h"
#include "../GTUtlity/GT_Util_GlogWrapper.h"
#include "../GTUtlity/GT_Util_CfgHelper.h"
#include "./GTEpollUtility/GTEpoll_Utility.h"
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h> // for bzero


namespace GT{

    namespace EPOLL{

        GTEpollWrapper::GTEpollWrapper():listen_fd_(0),listen_port_(0),use_multi_process_(false){

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
            ret = CreateListenSock_();
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
                    GT_LOG_DEBUG("unknow register type!");
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

        bool GTEpollWrapper::CreateListenSock_() {
            GT_TRACE_FUNCTION;

            listen_fd_ = socket(AF_INET, IPPROTO_TCP, 0);

            struct sockaddr_in sock_addr_ = {0};
            bzero(&sock_addr_, sizeof(sockaddr_in));
            sock_addr_.sin_family = AF_INET;
            sock_addr_.sin_port = htons(listen_port_);
            sock_addr_.sin_addr.s_addr = htonl(INADDR_ANY);

            bool ret = false;
            do {
                // set socket to no block
                if (!(ret = GT::EpollUtil::GTEpoll_Util::setsocket2noblock(listen_fd_))){
                    printf("set socket to no block failed!\n");
                    GT_LOG_ERROR("set socket to no block failed!");
                    break;
                }

                // set socket to reuse addr
                if (!(ret = GT::EpollUtil::GTEpoll_Util::setsockop(listen_fd_, SO_REUSEADDR))){
                    printf("set socket to reuse addr failed!\n");
                    GT_LOG_ERROR("set socket to reuse addr failed!");
                    break;
                }

                // set socket to reuse port
                if (!(ret = GT::EpollUtil::GTEpoll_Util::setsockop(listen_fd_, SO_REUSEPORT))){ // reuse port is only avaliable from kernal ver:3.9 or later
                    printf("set socket to reuse PORT failed!\n");
                    GT_LOG_ERROR("set socket to reuse PORT failed!");
                    break;
                }
            }while (0);

            return ret;
        }

        bool GTEpollWrapper::StartService() {

        }

        bool GTEpollWrapper::StopService() {

        }

    }
}