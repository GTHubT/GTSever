//
// Created by parallels on 10/10/17.
//

#include <sys/socket.h>
#include <iostream>
#include <string.h>
#include <netinet/in.h>


int main(){
    int clifd = socket(AF_INET, IPPROTO_TCP, 0);
    sockaddr_in servaddr;
    bzero(&servaddr, sizeof(sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5000);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = connect(clifd, (sockaddr*)&servaddr, sizeof(sockaddr_in));

    if (ret ==0){
        std::cout<<"connect to server success!";
    }

    char  a = 'v';
    std::cin>>a;
    return 0;
}