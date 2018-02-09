
//服务端程序
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//链表
typedef struct User
{
    char *name; //客户端串来的数据
    char *ip; //客户端的ip地址
    int port;

    
    struct User *next; //指针域
}User;  


//头节点
User *head = NULL;

int main()
{
    
    //创建一个socket套接字
    int fd = socket(AF_INET,SOCK_DGRAM,0);
    
    //构建结构体地址信息
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    //端口号
    addr.sin_port = htons(5000);

    //绑定地址
    int ret = bind(fd,(struct sockaddr*)&addr,sizeof(addr));


    while(1)
    {
    
        char buf[4096];
        //清空数组
        memset(buf,0,sizeof(buf));
        
        //客户端发来的ip地址存在在结构体中
        struct sockaddr_in paddr;
        socklen_t len = sizeof(paddr);
        
        //接受客户端发送来的数据
        recvfrom(fd,buf,sizeof(buf),0,(struct sockaddr*)&paddr,&len);

        //输出一下从客户端接收到的值就是buf的值
        printf("%s\n",buf);
        
        //判断接收的值的类型
        if(buf[0] == '1') //当接受到的字符串的第一个字符是'1'的时候
        {
            //标志客户端上线
            char *name = &buf[1]; //&buf[1] 相当于指针往后移动一位
            char *ip = inet_ntoa(paddr.sin_addr);//注意传过来的是整型数要转换成字符串类型
            
            //将客户端的信息存放在链表中
            //定义一个节点，并申请空间
            User *user = (User*)malloc(sizeof(User)); //sizeof(*user) 跟里面的等价
            
            //将数据赋值到节点中
           char * name1 = (char *)malloc(sizeof(char));
           name1 = name;
           strcpy(user->name,name);
           user->ip = strdup(ip);
           user->port = ntohs(paddr.sin_port);

           //将节点插入到链表中,头插法
           user->next = head;
           head = user;
        }
            
        if(buf[0] == '2')//当为2的时候
        {
            User *user = head;

            int n = 1;
        
            //遍历出所有的链表中的数据
            while(user)
            {
                //sprintf返回值是拼接成的字符串的字节数
                int ret = sprintf(buf+n,"%s:%s\n",user->ip,user->name);
                
                n += ret;
                user = user->next;
            }
            
            //发送数据到客户端
            sendto(fd,buf,strlen(buf),0,(struct sockaddr*)&paddr,sizeof(paddr));
        
        }


        //服务器转发数据
        if(buf[0] == '3')
        {
            
            User *user = head;
            while(user)
            {
                if(strncmp(user->name,buf+1,strlen(user->name)) == 0)
                {
                    break;
                }
                user = user->next;
            }

            if(user)
            {
                struct sockaddr_in oaddr;
                oaddr.sin_family = AF_INET;
                oaddr.sin_port = htons(user->port);
                oaddr.sin_addr.s_addr = inet_addr(user->ip);
                
                sendto(fd,buf,strlen(buf),0,(struct sockaddr*)&oaddr,sizeof(oaddr));
            }

        }
        



    }

    
}
