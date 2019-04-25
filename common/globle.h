#pragma once
#include <vector>
#include <string>
#include "HPSocket.h"
#include "helper.h"

using std::string;
using std::vector;
using namespace std;

#define MAXBUFFLEN 1024*10*10*5

#define  WM_VDATAUPDATE WM_USER+11
#define  WM_NET_STATE   WM_USER+12

#define  MSG_BEGN  0xEB//��Ϣͷ
#define  MSG_END   0x03//��Ϣβ

#define ZZ_SWAP_2BYTE(L) ((((L) & 0x00FF) << 8) | (((L) & 0xFF00) >> 8))
#define ZZ_SWAP_4BYTE(L) ((ZZ_SWAP_2BYTE ((L) & 0xFFFF) << 16) | ZZ_SWAP_2BYTE(((L) >> 16) & 0xFFFF))
#define ZZ_SWAP_8BYTE(L) ((ZZ_SWAP_4BYTE (((uint64_t)(L)) & 0xFFFFFFFF) << 32) | ZZ_SWAP_4BYTE((((uint64_t)(L)) >> 32) & 0xFFFFFFFF))


enum NET_STATE
{
	NET_STATE_OK,//��������
	NET_STATE_CLOSE,//����Ͽ�
	NET_STATE_MAX
};

enum USER_TYPE
{
	TYPE_COMMON,//��ͨ�û�
	TYPE_MNG,//����Ա
	TYPE_SUPER,//��������Ա
	TYPE_MAX
};
extern CString TypeName[];

//���Ʊ༭�������ַ�����
enum EDIT_TYPE
{
	EDIT_TYPE_NUMBER,//ֻ��������
	EDIT_TYPE_NUMBER_X,//ֻ�������ֺ�X�����������֤������
	EDIT_TYPE_FLOAT,//������
	EDIT_TYPE_NO_CHN,//��������
	EDIT_TYPE_ALL//������
};

enum PROJECT_TYPE
{
	PROJECT_TYPE_JIAODING=1,//����
	PROJECT_TYPE_DY,//��ҳ
	PROJECT_TYPE_ZY,//��ҳ
	PROJECT_TYPE_OTHER,//����
	PROJECT_TYPE_MAX
};

enum EM_DATE_TYPE
{
	EM_DATE_TYPE_ALL,
	EM_DATE_TYPE_1WEEK,
	EM_DATE_TYPE_1MONTH,
	EM_DATE_TYPE_3MONTH,
	EM_DATE_TYPE_6MONTH,
	EM_DATE_TYPE_12MONTH,
	EM_DATE_TYPE_MAX
};
extern CString DateName[];

//��Ŀ����������Χ����
enum PRO_NUM_TYPE
{
	PRO_NUM_TYPE_YS,//ӡ��
	PRO_NUM_TYPE_LS,//����
	PRO_NUM_TYPE_MAX
};
extern CString proNumType[PRO_NUM_TYPE_MAX];

//��Ŀ�����Ƿ���Ҫ��������
enum PRO_STAFF_TYPE
{
	PRO_STAFF_TYPE_YES,//��
	PRO_STAFF_TYPE_NO,//��
	PRO_STAFF_TYPE_MAX
};
extern CString proStaffType[PRO_STAFF_TYPE_MAX];

//ע����dai_number_pay��id������ö����������һ��
enum DAINUMPAY_TYPE
{
	DAINUMPAY_TYPE_W=1,//�������
	DAINUMPAY_TYPE_2,//2000һ��
	DAINUMPAY_TYPE_2_5,//2001~5000
	DAINUMPAY_TYPE_5_9,//5001~9999
	DAINUMPAY_TYPE_10,//10000����
	DAINUMPAY_TYPE_18,//18��10ӡ��
	DAINUMPAY_TYPE_MAX
};

//ע����zheye_number_pay��id������ö����������һ��
enum ZHEYEPAY_TYPE
{
	ZHEYEPAY_TYPE_Q4 = 1,//ȫ������ҳ
	ZHEYEPAY_TYPE_4,//����ҳ
	ZHEYEPAY_TYPE_D3,//������ҳ
	ZHEYEPAY_TYPE_3,//����ҳ
	ZHEYEPAY_TYPE_2,//����ҳ
	ZHEYEPAY_TYPE_MAX
};
extern CString ZyType[];

enum BOOK_RK
{
	BOOK_RK_NO,//δ���
	BOOK_RK_YES,//�����
	BOOK_RK_MAX
};
extern CString rkType[];

enum BOOK_TYPE
{
	BOOK_TYPE_QUAN,//ȫ��
	BOOK_TYPE_DUI,//�Կ�
	BOOK_TYPE_MAX
};
extern CString BookType[];

enum STAFF_TYPE
{
	STAFF_TYPE_PT,//��ͨ����
	STAFF_TYPE_SF,//ʦ��
	STAFF_TYPE_PT_SF,//ʦ��+����
	STAFF_TYPE_MAX
};
extern CString StaffType[];

enum DAYPAY_TYPE
{
	DAYPAY_TYPE_DAY,//����
	DAYPAY_TYPE_JIJIAN,//�Ƽ�
	DAYPAY_TYPE_DEL,//�۳�
	DAYPAY_TYPE_MAX
};



//��ϸ
struct STU_DETAIL
{
	CString strName;
	CString stridCard;
	int number;
};

struct WOCKCAL
{
	int proID;//��ĿID
	CString strProName;
	CString strBookID;//ͼ��ID
	CString strBookName;
	int number;//����
	CString strTime;
};

struct DAYPAY
{
	int id;
	DAYPAY_TYPE type;//���㷽ʽ
	int     proID;//��ĿID
	CString strProName;
	CString strBookID;//ͼ��ID
	CString strBookName;
	CString pay;//����
	double number;//����
	CString money;

	CString strMsg;

	CString strPayDay;
	CString strDays;

	DAYPAY()
	{
		type = DAYPAY_TYPE_MAX;
		number = 0;
	}
};

struct STAFF_STU
{
public:
	CString strname;
	CString strSex;
	int     age;
	CString strStaffID;
	CString strIdCard;
	CString strInTime;
	CString strTel;
	STAFF_TYPE type;
	double  fDaypay;//��ְ���չ�����
	int sort;
	int nFirstAscii;//ƴ������ĸasciiֵ����������
};

//���ڱ���ͻ�ȡ������Ŀ����
struct OTHER_PRO_PAY
{
	CString strBookID;
	CString strPay;
};

struct MONTHPAY_DAY
{
	vector<DAYPAY> v_daypay;
    CString strDate;
    int ndex;
};

struct MONTH_PAY_STAFF
{
    CString strStaffID;
    CString strStaffName;
    list<MONTHPAY_DAY> vDays;
};

struct BOOK_STU
{
	CString strBookID;
	CString strname;
	CString strCbs;
	CString strDate;
	int nBc1,nBc2;
	int nSize1,nSize2;
	int nKb;
	double fYz;
	int nYs;
	int nBc;
	double fLs;
	CString strPay;
	BOOK_TYPE type;
	ZHEYEPAY_TYPE  zyType;
	BOOK_RK rkType;
	CString strMsg;
	BOOK_STU()
	{
		type = BOOK_TYPE_MAX;
		zyType = ZHEYEPAY_TYPE_MAX;
		rkType = BOOK_RK_MAX;
	}
};

struct USER_STU
{
public:
	CString strname;
	CString strPass;
	int type;
	int show_pass;//1:��ס���� 0������
	CString strTime;
};

struct PROJECT_STU
{
public:
	int nID;
	int ndex;//��ʾ��ţ���1��ʼ
	CString strName;
	PRO_NUM_TYPE pn_type;
	PRO_STAFF_TYPE ps_type;
	int nAllBook;//�����Ƿ�����ȫ��
	CString strPay;

	PROJECT_STU()
	{
		ndex = 0;
		nAllBook = 0;
		pn_type = PRO_NUM_TYPE_MAX;
		ps_type = PRO_STAFF_TYPE_MAX;
	}
};

struct STU_DAI_PAY
{
	CString strA_w;
	CString strA_2;
	CString strA_2_5;
	CString strA_5_9;
	CString strA_10;
	CString strA_18;
	CString strSf_w;
	CString strSf_2;
	CString strSf_2_5;
	CString strSf_5_9;
	CString strSf_10;
	CString strSf_18;
	CString strTd_w;
	CString strTd_2;
	CString strTd_2_5;
	CString strTd_5_9;
	CString strTd_10;
	CString strTd_18;
};

struct STU_ZHEYE_PAY
{
	CString strAQ4;
	CString strA4;
	CString strAD3;
	CString strA3;
	CString strA2;

	CString str_sf_Q4;
	CString str_sf_4;
	CString str_sf_D3;
	CString str_sf_3;
	CString str_sf_2;

	CString str_td_Q4;
	CString str_td_4;
	CString str_td_D3;
	CString str_td_3;
	CString str_td_2;

};

void Log(char* fmt, ...);
typedef void (*FUN_CALLBACK)(void* p,string strData);
class CGloble: public CTcpPullClientListener
{
public:
	CGloble();
	~CGloble();
private:
	void Convert(char* strIn,char* strOut, int sourceCodepage, int targetCodepage);
    CString HzToPinYin(CString str);
	char convert(wchar_t n);
public:
    CString GetWorkDir();
	char* EncodeToUTF8(const char* mbcsStr);
	char* UTF8ToEncode(const char* mbcsStr);
	bool InitGloble();
	bool get_local_ip();
	char* CombineSendData(string strData,long& allLen);
	//�жϻ����Ǵ�˻���С�ˣ�return true:��ˣ�false:С��
	bool EndianJudge();
	int GetDays(int year,int month);
	CString ReturnBeginTime(EM_DATE_TYPE type);
	int GetFirstAsciiValue(CString strName);
	string GetLogFileName();
	string GetAppDataPath();

	bool ConnectSer();
	int SendTo(string strData);
	void DoRun(string strData);
	void SetCallback(FUN_CALLBACK callback,void* param);
private:
	virtual EnHandleResult OnConnect(ITcpClient* pSender, CONNID dwConnID);
	virtual EnHandleResult OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, int iLength);
	virtual EnHandleResult OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);

public:
	string m_LogRootPath;
	string m_LogPath;
	string m_LocalIP;//����IP
	int    m_TcpPort;//����TCP/IP����˿�
	CString m_strName;//����
	CString m_ConfigFilePath;
	CString m_strUserName;
	CString m_strUserPwd;
	int     m_nType;

public:
	BOOL m_bAsyncConn;
	TPkgInfo m_pkgInfo;
	CTcpPullClientPtr m_Client;
	FUN_CALLBACK     m_pCallBack;
	void*            m_pHand;
    HANDLE           m_heart; 
    HANDLE           m_hExit;  
};
extern CGloble g_Globle;