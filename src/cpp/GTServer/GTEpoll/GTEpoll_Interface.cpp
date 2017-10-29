#include "GTEpoll_Interface.h"
#include "GTEpoll_Wrapper.h"
#include "../../GTServer/GTUtlity/GT_Util_GlogWrapper.h"

#include <stdio.h>

#ifndef GTEPOLL_INSTANCE
#define GTEPOLL_INSTANCE GT::EPOLL::GTEpollWrapper::getInstance()
#endif

bool Initialize(std::string cfg_path){
    bool ret = GTEPOLL_INSTANCE.initialize(cfg_path);
    if (!ret)
        printf("GT Epoll service init failed!\n");
    return ret;
}


void Register(GTEPOLL_CALLBACK_TYPE type, gtepoll_callback cb){
    GT_TRACE_FUNCTION;
    GTEPOLL_INSTANCE.registerCallBack(type, cb);
}

void UnRegister(GTEPOLL_CALLBACK_TYPE type){
    GT_TRACE_FUNCTION;
    GTEPOLL_INSTANCE.unRegisterCallBack(type);
}

void StartService(){
    GT_TRACE_FUNCTION;
    GTEPOLL_INSTANCE.startService();
}

void StopService(){
    GT_TRACE_FUNCTION;
    GTEPOLL_INSTANCE.stopService();
}

void SendData(int fd, void* data, unsigned long len){
    GT_TRACE_FUNCTION;
    GTEPOLL_INSTANCE.sendData(fd, data, len);
}
