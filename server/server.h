/*************************************************************************
    > File Name: server.h
    > Author: 石志昂
    > Mail: 734109312@qq.com 
    > Created Time: 2021年08月31日 星期二 15时37分11秒
 ************************************************************************/
#ifndef __SERVER_H__
#define __SERVER_H__
#include "server.h"
#include <stdio.h>
#include <strings.h>
#include <sqlite3.h>
#define N 128
//系统配置
#define PORT 8000
#define IP "192.168.1.250"
typedef struct tabelData
{
	int num;//成功失败
	char **data;//数据
	int row;//行
	int column;//列
	char *errmsg;//错误

}TableData;
#define ERR_LOG(msg)do{\
	printf("_%d__\n",__LINE__);\
	perror(msg);\
}while(0)


//数据库配置和方法

int db_create(sqlite3 *db);//创建数据库
int db_open(char *fileName,sqlite3 **db);
int db_insert(sqlite3 *db,char *sql);
TableData db_SelTable(sqlite3 *db,char *sql);
#endif 
