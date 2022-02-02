#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 1024

int main() {
    //检查待发送文件
    char *filename = "./send.flv";
    FILE *fp = fopen(filename, "rb"); //二进制打开文件
    if(fp == NULL){
        printf("Cannot open file, press any key to exit!\n");
        system("pause"); //防止命令行窗口关闭
        exit(0);
    }

    //创建socket
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //将socket与地址、端口绑定
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr)); //地址每个字节都用0填充
    serv_addr.sin_family = AF_INET; //使用ipv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(1234); //端口号，大端存储，host to net short
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    //进入监听状态
    listen(serv_sock, 100); //能同时处理的访问数

    //接受客户端请求
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr); //客户端地址长度
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

    //向客户端循环发送数据，直到文件结尾
    char buffer[BUF_SIZE] = {0}; //定义并将缓冲区初始化为0
    int nCount;
    while((nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0){
        //fread返回成功读取的元素总数(前面都是bufsize, 最后一次可能小于)，每次读取BUF_SIZE个元素，每个元素为1字节，循环次数为fp/BUF_SIZE
        write(clnt_sock, buffer, nCount);
    }

    shutdown(clnt_sock, SHUT_RDWR);
    read(clnt_sock, buffer, BUF_SIZE); //客户端接收完毕会发回0(Fin包)

    //关闭socket
    fclose(fp);
    close(clnt_sock);
    close(serv_sock);

    system("pause");
    return 0;
}
