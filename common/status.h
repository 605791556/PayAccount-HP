#pragma once

//����������
enum JF_APP_RUN_STATUS
{
	//**************������*****************************//
	JF_SOCKET_STATUS_OK = 0,			//��������
	JF_SOCKET_STATUS_CREATE_FAIL, //socket����ʧ��
	JF_SOCKET_STATUS_CONNECT_FAIL,		//���Ӵ���ʧ��
	JF_SOCKET_STATUS_SEND_FAIL,		//������ʧ��
	JF_SOCKET_STATUS_THREAD_EXIT,		//�߳��˳�
	JF_SOCKET_STATUS_CLOSE,				//socket���ӱ��ر�
	JF_SOCKET_STATUS_RQ_OK,				//������ִ��ok
	JF_SOCKET_STATUS_DATA_ERR,				//���ݽ�������
	JF_SOCKET_STATUS_RUN_FAIL,			//��������ִ��ʧ��
	JF_SOCKET_STATUS_BUSY,			//�����̫��Ƶ��
	JF_SOCKET_STATUS_UNKONWN,		//δ֪�����ڴ�ʧ�ܵȵ�
	JF_SOCKET_STATUS_MAX,

	//*********************����������******************//
	JF_RUN_STATUS_OK = JF_SOCKET_STATUS_MAX+1,//ִ�гɹ�
	JF_RUN_STATUS_POINT_NULL,				//ָ��Ϊ��
	JF_RUN_STATUS_DELETE,					//�����Ѿ�ɾ��
	JF_RUN_STATUS_FAIL,						//ִ��ʧ��
	JF_RUN_STATUS_PARA_ERROR,				//��������
	JF_RUN_STATUS_JSON_ERROR,				//JSON��������
	JF_RUN_STATUS_DATA_EMPTY,				//����Ϊ��
	JF_RUN_STATUS_INDEX_OUT_OF_BOUND,		//Խ��
	JF_RUN_STATUS_ALLOCATE_MEMORY_ERROR,		//�����ڴ�ʧ��
	JF_RUN_STATUS_TYPE_ERROR,				//���ʹ���
	JF_RUN_STATUS_OPERATION_EEOR,			//�������ʹ���
	JF_RUN_STATUS_OTHER_ERROR,				//��������
	JF_RUN_STATUS_ERROR_DO,				//�ײ�����Ѿ������ϲ�ָ�����
	JF_RUN_STATUS_MAX						//
};

//������Ϣ����֧�ֵ��о�����
enum OFFICETYPE{
	OFFICETYPE_WORD = 0,
	OFFICETYPE_EXCEL,
	OFFICETYPE_PPT,
	OFFICETYPE_PS,
	OFFICETYPE_MAX
};
