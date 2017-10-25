#ifndef GT_EPOLL_INTERFACE_H_
#define GT_EPOLL_INTERFACE_H_

#include <string>
#include "./GTEpollDefine.h"

#ifdef __cplusplus
extern "C"
#endif
bool Initialize(std::string cfg_path);


#ifdef __cplusplus
extern "C"
#endif
void Register(GTEPOLL_CALLBACK_TYPE type, gtepoll_callback cb);


#ifdef __cplusplus
extern "C"
#endif
void UnRegister(GTEPOLL_CALLBACK_TYPE type);


#ifdef __cplusplus
extern "C"
#endif
void StopService();


#ifdef __cplusplus
extern "C"
#endif
void StartService();


#endif