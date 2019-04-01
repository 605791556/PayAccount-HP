#pragma once

//程序运行类
enum JF_APP_RUN_STATUS
{
	//**************网络类*****************************//
	JF_SOCKET_STATUS_OK = 0,			//运行正常
	JF_SOCKET_STATUS_CREATE_FAIL, //socket创建失败
	JF_SOCKET_STATUS_CONNECT_FAIL,		//链接创建失败
	JF_SOCKET_STATUS_SEND_FAIL,		//包发送失败
	JF_SOCKET_STATUS_THREAD_EXIT,		//线程退出
	JF_SOCKET_STATUS_CLOSE,				//socket链接被关闭
	JF_SOCKET_STATUS_RQ_OK,				//请求发送执行ok
	JF_SOCKET_STATUS_DATA_ERR,				//数据解析错误
	JF_SOCKET_STATUS_RUN_FAIL,			//网络命令执行失败
	JF_SOCKET_STATUS_BUSY,			//命令发送太过频繁
	JF_SOCKET_STATUS_UNKONWN,		//未知错误：内存失败等等
	JF_SOCKET_STATUS_MAX,

	//*********************常见运行类******************//
	JF_RUN_STATUS_OK = JF_SOCKET_STATUS_MAX+1,//执行成功
	JF_RUN_STATUS_POINT_NULL,				//指针为空
	JF_RUN_STATUS_DELETE,					//试题已经删除
	JF_RUN_STATUS_FAIL,						//执行失败
	JF_RUN_STATUS_PARA_ERROR,				//参数错误
	JF_RUN_STATUS_JSON_ERROR,				//JSON解析错误
	JF_RUN_STATUS_DATA_EMPTY,				//数据为空
	JF_RUN_STATUS_INDEX_OUT_OF_BOUND,		//越界
	JF_RUN_STATUS_ALLOCATE_MEMORY_ERROR,		//分配内存失败
	JF_RUN_STATUS_TYPE_ERROR,				//题型错误
	JF_RUN_STATUS_OPERATION_EEOR,			//操作类型错误
	JF_RUN_STATUS_OTHER_ERROR,				//其他错误
	JF_RUN_STATUS_ERROR_DO,				//底层错误已经处理，上层恢复反馈
	JF_RUN_STATUS_MAX						//
};

//定义信息技术支持的判卷类型
enum OFFICETYPE{
	OFFICETYPE_WORD = 0,
	OFFICETYPE_EXCEL,
	OFFICETYPE_PPT,
	OFFICETYPE_PS,
	OFFICETYPE_MAX
};
