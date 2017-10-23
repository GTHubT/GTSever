#include "GTEpoll_Wrapper.h"
#include "../GTUtlity/GT_Util_GlogWrapper.h"
#include "../GTUtlity/GT_Util_CfgHelper.h"

#include <stdio.h>


namespace GT{

    namespace EPOLL{

        GTEpollWrapper::GTEpollWrapper():listen_fd_(0),listen_port_(0){

        }

        GTEpollWrapper::~GTEpollWrapper() {

        }

        GTEpollWrapper& GTEpollWrapper::GetInstance() {
            static GTEpollWrapper gtepoll_instance_;
            return gtepoll_instance_;
        }

        bool GTEpollWrapper::Initialize(std::string cfgpath) {
            cfg_path_ = cfgpath;
            bool ret = InitializeCfgAndLog();
        }

        bool GTEpollWrapper::InitializeCfgAndLog() {
            bool ret = GT::UTIL::GT_Util_CfgHelper::LoadCfg(cfg_path_);
            if (!ret){
                printf("get cfg failed!\n");
                return false;
            }

            listen_port_ = GT_READ_CFG_INT("server_cfg","server_port",5000);
            ret = GT::UTIL::GT_Util_GlogWrapper::GetInstance().GT_DefaultLogInitialize();
            if (!ret){
                printf("log initialize failed!\n");
            }
            return true;
        }

        bool GTEpollWrapper::RegisterCallBack(GTEPOLL_CALLBACK_TYPE type, gtepoll_callback cb) {

        }

        bool GTEpollWrapper::UnRegisterCallBack(GTEPOLL_CALLBACK_TYPE type) {

        }

        void GTEpollWrapper::CreateListenSock() {

        }

        bool GTEpollWrapper::StartService() {

        }

        bool GTEpollWrapper::StopService() {

        }

    }
}