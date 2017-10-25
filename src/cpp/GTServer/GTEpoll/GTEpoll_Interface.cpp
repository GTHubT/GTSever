#include "GTEpoll_Interface.h"
#include "GTEpoll_Wrapper.h"
#include "../../GTServer/GTUtlity/GT_Util_GlogWrapper.h"

#include <stdio.h>

#ifndef GTEPOLL_INSTANCE
#define GTEPOLL_INSTANCE GT::EPOLL::GTEpollWrapper::GetInstance()
#endif

bool Initialize(std::string cfg_path){
    bool ret = GTEPOLL_INSTANCE.Initialize(cfg_path);
    if (!ret)
        printf("GT Epoll service init failed!\n");
    return ret;
}


void Register(GTEPOLL_CALLBACK_TYPE type, gtepoll_callback cb){
    GT_TRACE_FUNCTION;
    GTEPOLL_INSTANCE.RegisterCallBack(type, cb);
}

void UnRegister(GTEPOLL_CALLBACK_TYPE type){
    GT_TRACE_FUNCTION;
    GTEPOLL_INSTANCE.UnRegisterCallBack(type);
}

void StartService(){
    GT_TRACE_FUNCTION;
    GTEPOLL_INSTANCE.StartService();
}

void StopService(){
    GT_TRACE_FUNCTION;
    GTEPOLL_INSTANCE.StopService();
}
