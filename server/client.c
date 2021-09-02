/*************************************************************************
    > File Name: client.c
    > Author: 石志昂
    > Mail: 734109312@qq.com 
    > Created Time: 2021年09月01日 星期三 09时07分27秒
 ************************************************************************/
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<strings.h>
#include<unistd.h>
#include<string.h>
#include <stdlib.h>

#define ERR_MSG(msg)do{\
	printf("__%d__\n",__LINE__);\
	perror(msg);\
}while(0)
#define DATA_LEN 64
#define N 128

#define PORT 8000
#define IP "192.168.1.250"
//接收数据的格式
typedef struct receiveData
{
	unsigned char header[2];            //帧头
    unsigned char dataLength;          //数据个数（1字节），小于64
    unsigned char my_data[DATA_LEN];    //数据（小于64字节）
    unsigned char check_sum;           //校验和（1字节） 前面所有字节累加和
}DataInfo;
typedef struct tabelData
{
	int num;//成功失败
	char **data;//数据
	int row;//行
	int column;//列
	char *errmsg;//错误

}TableData;

typedef struct userinfo
{
    int number;
    char name[50];
    char password[50];
    char sex[50];
    int stature;//身高
    int weight;//体重
    int age;//年龄
    char phone[100];//手机号
    int state;//状态
    char remark[200];
    char addTime[200];
    char upTime[200];
}UserInfo;

typedef struct data
{	
	int num;//编号
	char head[4];//头结点
	char msg[200];//数据位数
	UserInfo user;
	UserInfo info;
	int total;//总数
	

}Data;



int socket_open(int *sfd);
int socket_send(int *sfd,Data *data);
int menu(int *sfd);
int sys_login(int *sfd);
int sys_order_dispose(int *sfd,Data data);
int sys_register(int * sfd);
int login_menu(int *sfd,Data *data);
int main(int argc, const char *argv[])
{
	int sfd=0;
	int ret=socket_open(&sfd);
	if(ret<0)
	{
		ERR_MSG(">>链接服务器失败");
		return -1;
	}
	printf(">>链接服务器成功");
	menu(&sfd);
	close(sfd);
	printf("关闭socket\n");
	printf("退出系统\n");
	return 0;
}

int menu(int *sfd)
{
	while(1)
	{
		system("clear");
		printf("××××××××字典客户端××××××××\n");
		printf("1.登录\n");
		printf("2.注册\n");
		printf("3.退出\n");
		printf("**************************\n");
		char btn_or='0';
		printf("请选择>>");
		scanf("%c",&btn_or);
		while(getchar()!=10);
		switch(btn_or)
		{
			case '1':
				printf(">>正在登录系统<<\n");
				sys_login(sfd);
				break;
			case '2':
				printf(">>正在注册信息<<\n");
				sys_register(sfd);
				break;

			case '3':
				goto END;
				break;
			default:
				printf("输入错误,请重新输入\n");
				break;
		}
		printf("请输入任意字符清屏>>\n");
		while(getchar()!=10);
	}
	END:
	return 0;
}
int sys_register(int * sfd)
{
	Data data;
	data.head[0]='R';
	data.head[1]='0';
	printf("请输入用户信息");
	printf("姓名:");
	scanf("%s",data.user.name);
	printf("性别:");
	scanf("%s",data.user.sex);
	printf("年龄:");
	scanf("%d",&data.user.age);
	printf("身高:");
	scanf("%d",&data.user.stature);
	printf("体重:");
	scanf("%d",&data.user.weight);
	printf("年龄:");
	scanf("%d",&data.user.age);
	printf("手机号:");
	scanf("%s",data.user.phone);
	socket_send(sfd,&data);
	return 0;
}
//系统登录
int sys_login(int * sfd)
{
	Data data;
	data.head[0]='G';
	data.head[1]='0';
	printf("请输入账户 密码\n");
	printf("账户:");
	scanf("%s",data.user.name);
	printf("密码:");
	scanf("%s",data.user.password);
//	memcpy(data.my_data,"sa,123",sizeof(data.my_data));
	socket_send(sfd,&data);
	return 0;
}

int login_menu(int *sfd,Data *data)
{
	while(1)
	{
		system("clear");
		printf(">>>>学生系统<<<<\n");
		printf(">>1.查询全部用户信息\n");
		printf(">>2.审核注册信息\n");
		printf(">>3.角色信息查询\n");
		printf(">>4.权限信息查询\n");
		printf(">>5.退出登录\n");
		printf("---------------\n");
		char btn_or='0';
		printf("请选择>>");
		scanf("%c",&btn_or);
		while(getchar()!=10);
		switch(btn_or)
		{
			case '1':
				printf(">>查询自己信息<<\n");
				data->head[0]='S';
				data->head[1]='0';
				data->head[3]='T';
				socket_send(sfd,data);
				break;
			case '2':
				printf(">>审核注册信息<<\n");
				printf(">>");
				data->head[0]='A';
				data->head[1]='0';
				scanf("%s",data->msg);
				socket_send(sfd,data);
				while(getchar()!=10);
				break;
			default:
				printf("输入有误\n");
				break;
		}
		printf("请输入任意字符清屏>>>");
		while(getchar()!=10);
		
	}
NEXT:
	return 0;
}



//socket 打开函数
int socket_open(int *sfd)
{
	//创建socket
	*sfd=socket(AF_INET,SOCK_STREAM,0);
	if(sfd<0)
	{
		ERR_MSG("socket");
		return -1;
	}	
	//链接服务器
    struct sockaddr_in sin;
    sin.sin_family      = AF_INET;
    sin.sin_port        = htons(PORT);
    sin.sin_addr.s_addr = inet_addr(IP);

    if(connect(*sfd, (struct sockaddr*)&sin, sizeof(sin))<0)
    {
        ERR_MSG("connect");
        return -1;
    }
	return 0;
}

//发送数据
int socket_send(int *sfd,Data *data)
{
	ssize_t res=0;
	//recv 发送数据
	if(send(*sfd,data,sizeof(*data),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	while(1)
	{
		res=recv(*sfd,data,sizeof(*data),0);
		if(res<0)
		{
			ERR_MSG("recv");
			return -1;
		}else if(0==res)
		{
			printf("关闭服务器\n");
			return 0;
		}
		if(res>0)
		{
			sys_order_dispose(sfd,*data);
		}
		printf("握草");
	}
	return 0;
}

//命令处理
int sys_order_dispose(int *sfd,Data data)
{
	switch(data.head[0])
	{
		case 'G':
			while(getchar()!=10);
			if(data.head[1]=='1')
			{
				login_menu(sfd,&data);
			}else
			{
				printf("msg:%s\n",data.msg);
			}
		break;
		case 'S':
			while(getchar()!=10);
			if(data.head[3]=='T')
			{	
				//UserInfo UserList[data.num];

				// ssize_t res=0;
				// res=recv(*sfd,UserList,sizeof(*UserList),0);
				// if(res<0)
				// {
				// 	ERR_MSG("send");
				// 	return -1;
				// }
				
				printf("收到数据\n");
				printf("姓名[0]>>%s\n",data.info.name);

			}else
			{
				printf("msg:%s\n",data.msg);
			}
			break;
			
	}
	printf("收到命令\n");
	sys_login(sfd);

}
