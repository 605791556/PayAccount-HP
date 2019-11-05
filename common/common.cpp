#include "stdafx.h"
#include "common.h"

const char CMD_LOGIN[EM_LOGIN_MAX][CMD_CHAR_MAX]=
{
	"name",
	"pass",
	"showpass"
};

const char CMD_LOGINMSG[EM_LOGINMSG_MAX][CMD_CHAR_MAX]=
{
	"bok",
	"name",
	"pass",
	"type",
	"logind"
};

const char CMD_GETUSER[EM_USER_MAX][CMD_CHAR_MAX]=
{
	"name",
	"type",
	"time",
	"showpass",
	"pass"
};

const char CMD_RetType[EM_CMD_RETYPE_MAX][CMD_CHAR_MAX]=
{
	"value",
	"result"
};

const char CMD_GETBOOK[GETBOOK_MAX][CMD_CHAR_MAX]=
{
	"keyword",
	"rktype",
	"date",
	"nstart",
	"number"
};

const char CMD_DELBOOK[EM_DEL_BOOK_MAX][CMD_CHAR_MAX]=
{
	"id"
};

const char CMD_RKBOOK[EM_RK_BOOK_MAX][CMD_CHAR_MAX]=
{
	"id"
};

const char CMD_JUDGEBOOK[EM_JUDGE_BOOK_MAX][CMD_CHAR_MAX]=
{
	"name",
	"bhad"
};

const char CMD_BOOKMSG[BOOKMSG_MAX][CMD_CHAR_MAX]=
{
	"id",
	"name",
	"cbs",
	"date",
	"bc1",
	"bc2",
	"size1",
	"size2",
	"kb",
	"yz",
	"ys",
	"bc",
	"ls",
	"pay",
	"b_type",
	"zy_type",
	"rk_type",
	"msg",
	"count",
	"xcount"
};

const char CMD_GETPRO[EM_GETPRO_MAX][CMD_CHAR_MAX]=
{
	"bwrite"
};

const char CMD_DELPROJECT[EM_DEL_PROJECT_MAX][CMD_CHAR_MAX]=
{
	"id"
};

const char CMD_JUDGEPROJECT[EM_JUDGE_PROJECT_MAX][CMD_CHAR_MAX]=
{
	"name",
	"bhad"
};

const char CMD_PROMSG[EM_PROMSG_MAX][CMD_CHAR_MAX]=
{
	"id",
	"ndex",
	"name",
	"num_type",
	"bwrite",
	"all_book",
	"pay",
	"count"
};

const char CMD_GETPROGRESS[EM_GETPROGRESS_MAX][CMD_CHAR_MAX]=
{
	"bookid",
	"proid",
	"index"
};

const char CMD_PROGRESSMSG[EM_PROGRESSMSG_MAX][CMD_CHAR_MAX]=
{
	"ys",
	"ls",
	"number",
	"ndex"
};

const char CMD_CHECKPWD[EM_CHECKPWD_MAX][CMD_CHAR_MAX]=
{
	"name",
	"pwd"
};

const char CMD_MDFPWD[EM_MDFPWD_MAX][CMD_CHAR_MAX]=
{
	"name",
	"pwd"
};

const char CMD_ADDUSER[EM_ADD_USER_MAX][CMD_CHAR_MAX]=
{
	"name",
	"pwd",
	"type"
};

const char CMD_DELUSER[EM_DEL_USER_MAX][CMD_CHAR_MAX]=
{
	"name"
};

const char CMD_JUDGEUSER[EM_JUDGE_USER_MAX][CMD_CHAR_MAX]=
{
	"name",
	"bhad"
};

const char CMD_GETSTAFF[EM_GET_STAFF_MAX][CMD_CHAR_MAX]=
{
	"keyword",
	"nstart",
	"number"
};

const char CMD_STAFFMSG[EM_STAFF_MSG_MAX][CMD_CHAR_MAX]=
{
	"name",
	"sex",
	"age",
	"staffid",
	"idcard",
	"intime",
	"tel",
	"type",
	"sort",
	"daypay",
	"count"
};

const char CMD_DELSTAFF[EM_DEL_STAFF_MAX][CMD_CHAR_MAX]=
{
	"id"
};

const char CMD_JUDGESTAFF[EM_JUDGE_STAFF_MAX][CMD_CHAR_MAX]=
{
	"idcard",
	"bhad"
};

const char CMD_DAIPAY[EM_DAI_PAY_MAX][CMD_CHAR_MAX]=
{
	"a_w",
	"2",
	"2_5",
	"5_9",
	"10",
	"18",
	"sf_w",
	"sf_2",
	"sf_2_5",
	"sf_5_9",
	"sf_10",
	"sf_18",
	"td_w",
	"td_2",
	"td_2_5",
	"td_5_9",
	"td_10",
	"td_18"
};

const char CMD_ZHEYEPAY[EM_ZHEYE_PAY_MAX][CMD_CHAR_MAX]=
{
	"a_q4",
	"a_4",
	"a_d3",
	"a_3",
	"a_2",
	"sf_q4",
	"sf_4",
	"sf_d3",
	"sf_3",
	"sf_2",
	"td_q4",
	"td_4",
	"td_d3",
	"td_3",
	"td_2"
};

const char CMD_DYPAY[EM_DY_PAY_MAX][CMD_CHAR_MAX]=
{
	"down",
	"up"
};

const char CMD_OTHERPAY[EM_OTHER_PAY_MAX][CMD_CHAR_MAX]=
{
	"proid"
};

const char CMD_OTHERPAYMSG[EM_OTHER_PAY_MSG_MAX][CMD_CHAR_MAX]=
{
	"bookid",
	"pay"
};

const char CMD_GETDETAILS[EM_GET_DETAILS_MAX][CMD_CHAR_MAX]=
{
	"ndex",
	"bookid",
	"proid",
	"proids"
};

const char CMD_DETAILMSG[EM_DETAIL_MSG_MAX][CMD_CHAR_MAX]=
{
	"ndex",
	"proid",
	"name",
	"idcard",
	"number"
};

const char CMD_GETDAYPAY[EM_GET_DAYPAY_MAX][CMD_CHAR_MAX]=
{
	"staffid",
	"staffname",
	"date"
};

const char DAYPAYMSG[EM_DAYPAY_MSG_MAX][CMD_CHAR_MAX]=
{
	"id",
	"type",
	"proid",
	"proname",
	"bookid",
	"bookname",
	"pay",
	"number",
	"money",
	"payday",
	"days",
	"del_msg"
};

const char MPAYMSG[EM_GET_MPAY_MAX][CMD_CHAR_MAX]=
{
	"staffid",
	"staffname",
	"date",
	"dex",
	"id",
	"type",
	"proid",
	"proname",
	"bookid",
	"bookname",
	"pay",
	"number",
	"money",
	"payday",
	"days"
};

const char HEARTMSG[EM_HEART_MAX][CMD_CHAR_MAX]=
{
	"user",
	"name"
};

const char GETPAYMSG[EM_GET_PAY_MAX][CMD_CHAR_MAX]=
{
	"staffid",
	"proid",
	"bookid",
	"pay",
	"nitem"
};