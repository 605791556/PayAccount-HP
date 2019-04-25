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

#define  MSG_BEGN  0xEB//消息头
#define  MSG_END   0x03//消息尾

#define ZZ_SWAP_2BYTE(L) ((((L) & 0x00FF) << 8) | (((L) & 0xFF00) >> 8))
#define ZZ_SWAP_4BYTE(L) ((ZZ_SWAP_2BYTE ((L) & 0xFFFF) << 16) | ZZ_SWAP_2BYTE(((L) >> 16) & 0xFFFF))
#define ZZ_SWAP_8BYTE(L) ((ZZ_SWAP_4BYTE (((uint64_t)(L)) & 0xFFFFFFFF) << 32) | ZZ_SWAP_4BYTE((((uint64_t)(L)) >> 32) & 0xFFFFFFFF))


enum NET_STATE
{
	NET_STATE_OK,//服务正常
	NET_STATE_CLOSE,//服务断开
	NET_STATE_MAX
};

enum USER_TYPE
{
	TYPE_COMMON,//普通用户
	TYPE_MNG,//管理员
	TYPE_SUPER,//超级管理员
	TYPE_MAX
};
extern CString TypeName[];

//控制编辑框输入字符类型
enum EDIT_TYPE
{
	EDIT_TYPE_NUMBER,//只允许数字
	EDIT_TYPE_NUMBER_X,//只允许数字和X，适用于身份证的输入
	EDIT_TYPE_FLOAT,//浮点数
	EDIT_TYPE_NO_CHN,//除了中文
	EDIT_TYPE_ALL//无限制
};

enum PROJECT_TYPE
{
	PROJECT_TYPE_JIAODING=1,//胶订
	PROJECT_TYPE_DY,//点页
	PROJECT_TYPE_ZY,//折页
	PROJECT_TYPE_OTHER,//其它
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

//项目做工件数范围类型
enum PRO_NUM_TYPE
{
	PRO_NUM_TYPE_YS,//印数
	PRO_NUM_TYPE_LS,//令数
	PRO_NUM_TYPE_MAX
};
extern CString proNumType[PRO_NUM_TYPE_MAX];

//项目进度是否需要工人填入
enum PRO_STAFF_TYPE
{
	PRO_STAFF_TYPE_YES,//是
	PRO_STAFF_TYPE_NO,//否
	PRO_STAFF_TYPE_MAX
};
extern CString proStaffType[PRO_STAFF_TYPE_MAX];

//注：表dai_number_pay中id序号与该枚举索引必须一致
enum DAINUMPAY_TYPE
{
	DAINUMPAY_TYPE_W=1,//万代数量
	DAINUMPAY_TYPE_2,//2000一下
	DAINUMPAY_TYPE_2_5,//2001~5000
	DAINUMPAY_TYPE_5_9,//5001~9999
	DAINUMPAY_TYPE_10,//10000以上
	DAINUMPAY_TYPE_18,//18代10印张
	DAINUMPAY_TYPE_MAX
};

//注：表zheye_number_pay中id序号与该枚举索引必须一致
enum ZHEYEPAY_TYPE
{
	ZHEYEPAY_TYPE_Q4 = 1,//全开四折页
	ZHEYEPAY_TYPE_4,//四折页
	ZHEYEPAY_TYPE_D3,//大三折页
	ZHEYEPAY_TYPE_3,//三折页
	ZHEYEPAY_TYPE_2,//二折页
	ZHEYEPAY_TYPE_MAX
};
extern CString ZyType[];

enum BOOK_RK
{
	BOOK_RK_NO,//未入库
	BOOK_RK_YES,//已入库
	BOOK_RK_MAX
};
extern CString rkType[];

enum BOOK_TYPE
{
	BOOK_TYPE_QUAN,//全开
	BOOK_TYPE_DUI,//对开
	BOOK_TYPE_MAX
};
extern CString BookType[];

enum STAFF_TYPE
{
	STAFF_TYPE_PT,//普通工人
	STAFF_TYPE_SF,//师傅
	STAFF_TYPE_PT_SF,//师傅+工人
	STAFF_TYPE_MAX
};
extern CString StaffType[];

enum DAYPAY_TYPE
{
	DAYPAY_TYPE_DAY,//按天
	DAYPAY_TYPE_JIJIAN,//计件
	DAYPAY_TYPE_DEL,//扣除
	DAYPAY_TYPE_MAX
};



//明细
struct STU_DETAIL
{
	CString strName;
	CString stridCard;
	int number;
};

struct WOCKCAL
{
	int proID;//项目ID
	CString strProName;
	CString strBookID;//图书ID
	CString strBookName;
	int number;//数量
	CString strTime;
};

struct DAYPAY
{
	int id;
	DAYPAY_TYPE type;//计算方式
	int     proID;//项目ID
	CString strProName;
	CString strBookID;//图书ID
	CString strBookName;
	CString pay;//单价
	double number;//数量
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
	double  fDaypay;//该职工日工工资
	int sort;
	int nFirstAscii;//拼音首字母ascii值，用于排序
};

//用于保存和获取其它项目单价
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
	int show_pass;//1:记住密码 0：不记
	CString strTime;
};

struct PROJECT_STU
{
public:
	int nID;
	int ndex;//显示序号，从1开始
	CString strName;
	PRO_NUM_TYPE pn_type;
	PRO_STAFF_TYPE ps_type;
	int nAllBook;//单价是否适用全书
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
	//判断机器是大端还是小端，return true:大端，false:小端
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
	string m_LocalIP;//本地IP
	int    m_TcpPort;//本机TCP/IP服务端口
	CString m_strName;//名称
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