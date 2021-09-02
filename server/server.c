/*************************************************************************
    > File Name: server.c
    > Author: 石志昂
    > Mail: 734109312@qq.com 
    > Created Time: 2021年08月31日 星期二 15时13分47秒
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "server.h"
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sqlite3.h>
#define DATA_LEN 64
#define N 128

//接收数据的格式
typedef struct receiveData
{
	unsigned char header[2];            //帧头
    unsigned char dataLength;          //数据个数（1字节），小于64
    unsigned char my_data[DATA_LEN];    //数据（小于64字节）
    unsigned char check_sum;           //校验和（1字节） 前面所有字节累加和
}DataInfo;


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
char revData[255];


#define ERR_MSG(msg) do{\
    printf("__%d__ %s\n", __LINE__, __func__);\
    perror(msg);\
}while(0)


int recv_cli_msg(sqlite3 *db,int newfd,struct sockaddr_in cin);
int order_dispose(sqlite3 *db,int newfd,Data data);
int data_send(int newfd,Data data);
int main(int argc, const char *argv[])
{
	printf("------学生管理系统服务器启动------\n");
	printf("IP:%s:",IP);	
	printf("%d\n",PORT);
	sqlite3 *db=NULL;
	//打开数据库
	if(db_open("MyStaff_md.db",&db)<0)
	{
		printf(">>打开数据库失败\n");
		return -1;
	}
	printf("链接数据库[成功]\n");
	if(db_create(db)<0){
		printf(">>初始化失败\n");
		return -1;
	}
	


	//创建socket
    //创建套接字
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd < 0)
    {
        ERR_MSG("socket");
        return -1;
    }


    //允许本地端口快速重用
    int reuse = 1;
    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse))<0)
    {
        ERR_MSG("setsockopt");
        return -1;
    }


    //绑定服务器的ip和端口
    struct sockaddr_in sin;
    sin.sin_family      = AF_INET;
    sin.sin_port        = htons(PORT);
    sin.sin_addr.s_addr = inet_addr(IP);   //ifconfig查询

    if(bind(sfd, (struct sockaddr*)&sin, sizeof(sin)) <0)
    {
        ERR_MSG("bind");
        return -1;
    }

    //将套接字设置为被动监听状态
    if(listen(sfd, 5) <0)
    {
        ERR_MSG("listen");
        return -1;
    }
    printf("监听成功\n");

    struct sockaddr_in cin;
    socklen_t addrlen = sizeof(cin);

 

    char buf[N];
    ssize_t res = 0;
	//等待新的套接字
	pid_t pid=0;
	pid=fork();
    while(1)
    {
	   //等待获取新的套接字
		int newfd = accept(sfd, (struct sockaddr*)&cin, &addrlen);
		if(newfd < 0)
		{

	        ERR_MSG("accept");
		    return -1;
		}
		 printf("newfd = %d\n", newfd);
		 printf("[%s:%d]已上线\n", inet_ntoa(cin.sin_addr), ntohs(cin.sin_port));
		
		 pid=fork();
		 if(pid>0)
		 {
			 close(newfd);//父进程关闭
		 }else if(0==pid)
		 {
			close(sfd);//紫禁城只负责交互
			recv_cli_msg(db,newfd,cin);
			//紫禁城从教书函数退出后，直接结束进程
			exit(0);
		 }else{
			 ERR_MSG("fork");
			 return -1;
		 }
    }

    close(sfd);

    return 0;
}
int recv_cli_msg(sqlite3 *db,int newfd,struct sockaddr_in cin)
{
	Data data;
	ssize_t res=0;
	while(1)
	{
		memset(&data,sizeof(data),0);//初始化
		//接受
		res=recv(newfd,&data,sizeof(data),0);
		
		//res=recv(newfd,revData,255,0);
		//printf("%c\n",data.head[0]);
		if(res<0)
		{
			ERR_MSG("recv");
			return -1;
		}else if(0==res)//清空
		{
			
			//修改登录状态
			printf("对方关闭\n");
			break;
		}
		order_dispose(db,newfd,data);
	}
}
//接受命令处理
int order_dispose(sqlite3 *db,int newfd,Data data)
{
	Data *retData;
	char sql[1000]="";
	//printf("指令:%d",data.order);
	printf("命令[0]=%c\n",data.head[0]);
	printf("命令[1]=%c\n",data.head[1]);
	switch(data.head[0])
	{
		//bzero(sql,sizeof(sql));
		case 'G':
			data.head[0]='G';
			data.head[1]='1';
			//查询用户是否存在
			data_send(newfd,data);
			break;
		case 'R':
			data.head[1]='1';
			//注册
			UserInfo *u=&data.user;
			printf("获取注册信息:%s\n",data.user.name);
			//	sprintf(sql,"insert into users values((select ifnull(max(number),21000)+1 from users),'myshi','男',20,120,150,'111','班级','学生','123','',1,'2012','2012')");
			//sprintf(sql,"insert into users values((select ifnull(max(number),21000)+1 from users),'%s','%s',%d,%d,%d,%s,'','','%s','',%s,'2021-9-2','2012-9-2');",data.user.name,data.user.sex,data.user.age,data.user.stature,data.user.weight,data.user.phone,data.user.password,data.user.remark,1,'2011-11-2','2021-11-2');
			sprintf(sql,"insert into users(number,name,sex,age,stature,weight,phone,pwd,remark,state,addTime,upTime) values((select ifnull(max(number),21000)+1 from users),'%s','%s',%d,%d,%d,'%s','%s','',0,'2021-1-1','2021-2-2')",u->name,u->sex,u->age,u->stature,u->weight,u->phone,u->password);
			if(db_insert(db,sql)<0)
			{
				sprintf(data.msg,"%s",">>注册失败!");
			}else{
				 	data.num=1;//注册成功
				 sprintf(data.msg,"%s",">>注册成功");
			}
			data_send(newfd,data);
			break;
		case 'F':
				sprintf(sql,"select * from users");;
				TableData dt=db_SelTable(db,sql);//dt.row //dt.column
				UserInfo UserList[dt.row];
				data.head[0]='F';
				data.head[1]='1';
				data.head[2]='0';//开始发送
				data.head[3]='A';
				 data.num=12;//dt.row
				printf("%s\n","开始查询数据信息");
				int i=0,j=0,k=0;
				for(i=0;i<dt.row;i++)
				{
						for(j=0;j<dt.column;j++)
						{	k++;
							//printf("%-8s",dt.data[k++]);
							//sprintf(data.msg,"%-4s",dt.data[k++]);
						}
						printf("\n");
						UserList[i].number=atoi(dt.data[k]);
						printf("%-8d",UserList[i].number);
						strcpy(UserList[i].name,dt.data[k+1]);
						printf("%-8s",UserList[i].name);
						//strcpy(UserList[i].sex,dt.data[k++]);
						//printf("%-8s",UserList[i].sex);
						//UserList[i].age=atoi(dt.data[k++]);
						// printf("%-8d",UserList[i].age);
						// UserList[i].stature=atoi(dt.data[k++]);
						// printf("%-8d",UserList[i].stature);
						// UserList[i].weight=atoi(dt.data[k++]);
						// printf("%-8d",UserList[i].weight);
						// strcpy(UserList[i].phone,dt.data[k++]);
						// printf("%-8s",UserList[i].phone);
						// strcpy(UserList[i].password,dt.data[k+3]);
						// printf("%-8s",UserList[i].password);
						// strcpy(UserList[i].remark,dt.data[k++]);
						// printf("%-8s",UserList[i].remark);
						// UserList[i].state=atoi(dt.data[k++]);
						// printf("%-8d",UserList[i].state);
						// strcpy(UserList[i].addTime,dt.data[k++]);
						// printf("%-8s",UserList[i].addTime);
						// strcpy(UserList[i].upTime,dt.data[k++]);
						// printf("%-8s",UserList[i].upTime);
						printf("\n");
						
				}
				//memset(&data.dt,sizeof(data.dt),1);//初始化结构体
				// if(send(newfd,&UserList,sizeof(UserList),0)<0)
				// 	{
				// 		ERR_LOG("sen");
				// 		return -1;
				// 	}
				// 	printf("用户结构体发送成功\n");
				 // sprintf(data.msg,"%s",">>数据查询成功");
				//发送协议
			
				 for(i=0;i<dt.row;i++)
				{
						if(i==dt.row-1){
								data.head[2]='1';//发送结束
						}
						data.info=UserList[i];
						data_send(newfd,data);
					 /* code */
				 }
				break;
	}
	//data_send(newfd,retData);


	printf("收到消息");
}

//发送消息
int data_send(int newfd,Data data)
{
	if(send(newfd,&data,sizeof(data),0)<0)
	{
		ERR_LOG("sen");
		return -1;
	}
	printf("发送成功\n");

}
