/**
 * Simplest Librtmp Receive
 *
 * �����裬����
 * leixiaohua1020@126.com
 * zhanghuicuc@gmail.com
 * �й���ý��ѧ/���ֵ��Ӽ���
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * ���������ڽ���RTMP��ý�岢�ڱ��ر����FLV��ʽ���ļ���
 * This program can receive rtmp live stream and save it as local flv file.
 */
#include <stdio.h>
#include "librtmp/rtmp_sys.h"
#include "librtmp/log.h"

int InitSockets()
{
#ifdef WIN32
	WORD version;
	WSADATA wsaData;
	version = MAKEWORD(1, 1);
	return (WSAStartup(version, &wsaData) == 0);
#endif
}

void CleanupSockets()
{
#ifdef WIN32
	WSACleanup();
#endif
}

int main(int argc, char* argv[])
{
	//��ʼ��Socket 
	InitSockets();
	
	double duration=-1;
	int nRead;
	//is live stream ?
	bool bLiveStream=true;				
	
	
	int bufsize=1024*1024*10;			
	char *buf=(char*)malloc(bufsize);
	memset(buf,0,bufsize);
	long countbufsize=0;
	
	FILE *fp=fopen("receive.flv","wb");
	if (!fp){
		RTMP_LogPrintf("Open File Error.\n");
		CleanupSockets();
		return -1;
	}
	
	/* set log level */
	//RTMP_LogLevel loglvl=RTMP_LOGDEBUG;
	//RTMP_LogSetLevel(loglvl);
	//Ϊ�ṹ�塰RTMP�������ڴ�
	RTMP *rtmp=RTMP_Alloc();
	//��ʼ���ṹ�塰RTMP���еĳ�Ա����
	RTMP_Init(rtmp);
	//set connection timeout,default 30s ���ӳ�ʱʱ��
	rtmp->Link.timeout=10;	
	// ���������RTMP���ӵ�URL �޸�URLΪ��rtmp://58.200.131.2:1935/livetv/cctv1����
	if(!RTMP_SetupURL(rtmp,"rtmp://media3.scctv.net/live/scctv_800"))
	{
		RTMP_Log(RTMP_LOGERROR,"SetupURL Err\n");
		RTMP_Free(rtmp);
		CleanupSockets();
		return -1;
	}
	if (bLiveStream){
		rtmp->Link.lFlags|=RTMP_LF_LIVE;
	}
	
	//1hour
	RTMP_SetBufferMS(rtmp, 3600*1000);		
	//����RTMP���ӣ�����һ��RTMPЭ��淶�е�NetConnection
	if(!RTMP_Connect(rtmp,NULL)){
		RTMP_Log(RTMP_LOGERROR,"Connect Err\n");
		RTMP_Free(rtmp);
		CleanupSockets();
		return -1;
	}
	//����һ��RTMPЭ��淶�е�NetStream
	if(!RTMP_ConnectStream(rtmp,0)){
		RTMP_Log(RTMP_LOGERROR,"ConnectStream Err\n");
		RTMP_Close(rtmp);
		RTMP_Free(rtmp);
		CleanupSockets();
		return -1;
	}
	//�ӷ�������ȡ����
	while(nRead=RTMP_Read(rtmp,buf,bufsize)){
		fwrite(buf,1,nRead,fp);

		countbufsize+=nRead;
		RTMP_LogPrintf("Receive: %5dByte, Total: %5.2fkB\n",nRead,countbufsize*1.0/1024);
	}

	if(fp)
		fclose(fp);

	if(buf){
		free(buf);
	}

	if(rtmp){
		//�ر�RTMP����
		RTMP_Close(rtmp);
		//�ͷŽṹ�塰RTMP��
		RTMP_Free(rtmp);
		//�ر�Socket
		CleanupSockets();
		rtmp=NULL;
	}	
	return 0;
}