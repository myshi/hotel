/*************************************************************************
    > File Name: sqlhelper.c
    > Author: 石志昂
    > Mail: 734109312@qq.com 
    > Created Time: 2021年08月31日 星期二 15时52分50秒
 ************************************************************************/
#include "server.h"
#include <stdio.h>
#include <strings.h>
#include <sqlite3.h>
//打开数据库
//创建数据库
int db_create(sqlite3 *db)
{
	char sql[200]="";
	bzero(sql,sizeof(sql));

	//创建用户表
	//create table if not exists users(number int number key,name char,sex char,age int,stature int,weight int,phone char,class char,role char,pwd char,remark char,state int,addTime char,upTime char);
	sprintf(sql,"create table if not exists users(number int number key,name char,sex char,age int,stature int,weight int,phone char,class char,role char,pwd char,remark char,state int,addTime char,upTime char);");
	if(db_insert(db,sql)<0)
	{
		printf(">>用户表创建失败\n");
		return -1;
	}
	printf(">>用户表创建成功");
    bzero(sql,sizeof(sql));
	//创建审批记录表
	//CREATE TABLE `audit` ( `id` int NOT NULL, `number` int, `put_number` int, `state` int, `time` char )
	sprintf(sql,"CREATE TABLE   if not exists `audit` ( `id` int NOT NULL, `number` int, `put_number` int, `state` int, `time` char );");
	if(db_insert(db,sql)<0)
	{
		printf(">>审批记录表失败\n");
		return -1;
	}
	printf(">>审批记录表成功");
    bzero(sql,sizeof(sql));
	//创建权限
	//CREATE TABLE `power` ( `id` int NOT NULL, `name` char, `state` int, `addTime` char )
	sprintf(sql,"CREATE TABLE   if not exists `power` ( `id` int NOT NULL, `name` char, `state` int, `addTime` char );");
	if(db_insert(db,sql)<0)
	{
		printf(">>创建权限失败\n");
		return -1;
	}
	printf(">>创建权限成功");
    bzero(sql,sizeof(sql));
	//角色
	//CREATE TABLE `role` ( `id` int NOT NULL, `name` char, `state` int, `addTime` char, PRIMARY KEY(`id`) )
	sprintf(sql,"CREATE TABLE     if not exists  `role` ( `id` int NOT NULL, `name` char, `state` int, `addTime` char, PRIMARY KEY(`id`) );");
	if(db_insert(db,sql)<0)
	{
		printf(">>角色失败\n");
		return -1;
	}
	printf(">>角色成功");
    bzero(sql,sizeof(sql));
	return 0;

}
//打开链接
int db_open(char *fileName,sqlite3 **db)
{
	if(sqlite3_open(fileName,db))
	{
		//打印错误信息
		printf("%d\n",sqlite3_errcode(*db));
		printf("%s\n",sqlite3_errmsg(*db));
		return -1;
	}
	return 0;
}
//插入数据
int db_insert(sqlite3 *db,char *sql)
{
	printf("insert into>>%s \n",sql);
	char *errmsg=NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=0)
	{
		printf("__%d__%s sqliste3_exec:%s\n",__LINE__,__func__,errmsg);
		return -1;
	}
	return 0;
}
//查询语句
TableData db_SelTable(sqlite3 *db,char *sql)
{
	TableData dt;
	printf("%s",sql);
	dt.data=NULL;
	void sqlite3_free_table(char **result);
	if(sqlite3_get_table(db,sql,&dt.data,&dt.row,&dt.column,&dt.errmsg)!=0)
	{
		dt.num=-1;
		fprintf(stderr,"__%d__sqlite3_get_table::%s\n",__LINE__,dt.errmsg);
		return dt;
	}
	printf("\nrow=%d,column=%d\n",dt.row,dt.column);
	dt.num=dt.row;
	return dt;	

}