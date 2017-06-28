#ifndef __MY_DEVICE_H__
#define __MY_DEVICE_H__

#include <windows.h>
#include "CApi.h"
#include "config.h"
#include "ComSerial.h"
#define SATELLITEMAX 32
enum
{
	LED_OFF,
	LED_ON,
	LED_FLUSH_SLOW,
	LED_FLUSH_FAST,

	LED_TYPE_GSM = 0,
	LED_TYPE_GPS,
	LED_TYPE_MAX,
};

enum SYS_STATE_ENUM
{
	//状态
	SYSTEM_STATE,
	SIM_STATE,
	SMS_STATE,
	REG_STATE,
	GPRS_STATE,
	GPRS_ATTACH_STATE,
	GPRS_ACT_STATE,			//bit0~bit6, pdp1~7
	MONITOR_STATE,
	RSSI_STATE,
	GPS_STATE,
	ALARM_STATE,
	CRASH_STATE,
	MOVE_STATE,
	CUTLINE_STATE,
	OVERSPEED_STATE,
	PRINT_STATE,
	FIRST_LOCAT_STATE,
	TRACE_STATE,
	WDG_STATE,
	LED_STATE,
	LED_STATE_MAX = LED_STATE + LED_TYPE_MAX - 1,
	REBOOT_STATE,
	STATE_MAX,
};

enum SYS_ERROR_ENUM
{
	//故障
	SIM_ERROR,
	GPRS_ERROR,
	GPS_ERROR,
	ANT_SHORT_ERROR,
	ANT_BREAK_ERROR,
	SENSOR_ERROR,
	LOW_POWER_ERROR,
	ERROR_MAX,
};

enum SYS_VAR_ENUM
{
	SYS_TIME,
	VBAT,
	UTC_DATE,
	UTC_TIME,
	IO_VAL,
	GSENSOR_VAL,
	GSENSOR_ALARM_VAL,
	GSENSOR_MONITOR_VAL,
	GSENSOR_KEEP_VAL,
	CELL_ID,
	SHUTDOWN_TIME,
	SOFTWARE_VERSION,
	MAIN_FREQ,
	TRACE_TO,
	VAR_MAX,
};

enum
{
	PARAM_DETECT_PERIOD,
	PARAM_SENSOR_EN,
	PARAM_COM_BR,
	PARAM_GPS_BR,
	PARAM_STOP_VBAT,
	PARAM_LOW_VBAT,
	PARAM_NORMAL_VBAT,

	PARAM_SMS_ALARM,			//短信报警使能
	PARAM_CALL_AUTO_GET,		//自动接听使能
	PARAM_SIM_TO,				//SIM卡识别超时
	PARAM_GPRS_TO,				//GPRS各个步骤超时的基础，默认60,
	PARAM_SYS_MAX,

	PARAM_GS_WAKEUP_GPS = 0,
	PARAM_VACC_WAKEUP_GPS,
	PARAM_GPS_NODATA_TO,		//无数据时间
	PARAM_GPS_V_TO,				//无效定位时间
	PARAM_GPS_KEEP_TO,			//GPS连续工作时间
	PARAM_GPS_SLEEP_TO,			//GPS休眠时间
	PARAM_AGPS_EN,
	PARAM_GPS_ONLY_ONCE,		//GPS定位就关闭，直到休眠结束
	PARAM_GPS_MAX,

	PARAM_GS_WAKEUP_MONITOR = 0,	//震动唤醒通讯，如果为0，则为VACC唤醒
	PARAM_GS_JUDGE_RUN,		//震动确认骑行
	PARAM_UPLOAD_RUN_PERIOD,		//骑行回传周期
	PARAM_UPLOAD_STOP_PERIOD,		//停车回传周期
	PARAM_UPLOAD_HEART_PERIOD,	//心跳周期
	PARAM_MONITOR_NET_TO,
	PARAM_MONITOR_KEEP_TO,
	PARAM_MONITOR_SLEEP_TO,
	PARAM_MONITOR_RECONNECT_MAX,
	PARAM_MONITOR_ADD_MILEAGE,
	PARAM_MONITOR_ACC_UPLOAD,	//ACC变化上传
	PARAM_MONITOR_MAX,

	PARAM_VACC_CTRL_ALARM = 0,
	PARAM_ALARM_ON_DELAY,		//ACC控制震动和移动报警的延时
	PARAM_CRASH_GS,				//震动阈值
	PARAM_CRASH_JUDGE_TO,		//震动报警判定时间窗
	PARAM_CRASH_JUDGE_CNT,		//震动报警判定次数
	PARAM_CRASH_ALARM_WAIT_TO,
	PARAM_CRASH_ALARM_FLUSH_TO,
	PARAM_CRASH_ALARM_REPEAT,
	PARAM_CRASH_WAKEUP_MOVE,	//移动报警前是否必须有震动
	PARAM_MOVE_RANGE,
	PARAM_MOVE_JUDGE_TO,
	PARAM_MOVE_ALARM_FLUSH_TO,
	PARAM_MOVE_ALARM_REPEAT,		//移动报警是否连续报警
	PARAM_ALARM1_MAX,

	PARAM_VACC_WAKEUP_CUTLINE_TO = 0,
	PARAM_CUTLINE_ALARM_DELAY,
	PARAM_CUTLINE_ALARM_FLUSH_TO,	//剪线报警刷新时间，如果为0，则不连续报警
	PARAM_OVERSPEED_ALARM_VAL,		//超速报警阈值，如果为0，则不检测
	PARAM_OVERSPEED_ALARM_DELAY,	//超速报警延迟，如果为0，一旦超速立刻报警
	PARAM_ALARM_ENABLE,			//允许进入报警流程
	PARAM_ALARM2_MAX,

	PARAM_TYPE_MAIN = 0,
	PARAM_TYPE_SYS,
	PARAM_TYPE_GPS,
	PARAM_TYPE_MONITOR,
	PARAM_TYPE_ALARM1,
	PARAM_TYPE_ALARM2,
	PARAM_TYPE_APN,
	PARAM_TYPE_LOCAT,
	PARAM_TYPE_UPDATE,
	PARAM_TYPE_NUMBER,
	PARAM_TYPE_USER,
	PARAM_TYPE_MAX,
};

#define IMEI_LEN		(10)
#define IMSI_LEN		(10)
#define ICCID_LEN		(10)
#define URL_LEN_MAX		(48)
#define AUTH_CODE_LEN	(48)
#define COM_BUF_LEN		(2048)

#define RDA_UPGRADE_MAGIC_NUM (0x12345678)

typedef struct
{
	Date_UserDataStruct UTCDate;//UTC时间
	Time_UserDataStruct UTCTime;
	u32 HighLevel;
	u32 LatDegree;
	u32 LatMin;
	u32 LgtDegree;
	u32 LgtMin;
	u32 Speed;//字段7:地面速率（0~999.9999节）
	u32 Cog;//字段8:地面航向（0 ~359.999 度，以真北为参考基准，前面的0也将被传输）
	s8 LocatStatus;//字段2:定位状态，1=有效定位，0=无效定位
	s8 LatNS;
	s8 LgtEW;
	u8 LocatMode;
}RMC_InfoStruct;

typedef struct
{
	u8 PRN[2][48];
	u8 CN[2][48];
	u8 Pos[2];
	u8 IsVaild[2];
}GSV_InfoStruct;

typedef struct
{
	u32 UID[3];
	s8 MainURL[URL_LEN_MAX - 8];
	u32 MainIP;
	u16 TCPPort;	//如果为0，表示使用UDP
	u16 UDPPort;	//如果为0，表示使用TCP
}Param_MainStruct;

typedef struct
{
	s8 APNName[20];
	s8 APNUser[20];
	s8 APNPassword[20];
}Param_APNStruct;

typedef struct
{
	s8 Url[URL_LEN_MAX - 12];
	s8 Usr[12];
	s8 Pwd[12];
}Param_FtpStruct;

typedef struct
{
	u8 Num[8];
}Phone_NumberStruct;

typedef struct
{
	Phone_NumberStruct Phone[7];
}Param_NumberStruct;

typedef struct
{
	u32 Param[15];
}Param_DWStruct;

typedef struct
{

	u32 UpdateIP;
	s8 UpdateURL[URL_LEN_MAX];
	u16 TCPPort;	//如果为0，表示使用UDP
	u16 UDPPort;	//如果为0，表示使用TCP
}Param_UpdateStruct;

typedef union
{
	Param_FtpStruct Ftp;
	Param_UpdateStruct Update;
	u8 pad[60];
}Param_UpdateUnion;

typedef struct
{
	u32 LYIP;
	u16 LYTCPPort;
	u16 LYUDPPort;
}Param_LYStruct;

typedef struct
{
	u8 AuthCode[AUTH_CODE_LEN];
	u16 ProvinceID;
	u16 CityID;
	u8 PlateID;
	u8 AuthCodeLen;
}Param_KQStruct;

typedef union
{
	Param_LYStruct LY;
	Param_KQStruct KQ;
}Param_UserStruct;

typedef struct
{
	RMC_InfoStruct RMCSave;
	u32 MileageKM;
	u32 MileageM;
}Param_LocatStruct;

typedef union
{
	Param_MainStruct MainInfo;
	Param_APNStruct APN;
	Param_UpdateUnion Update;
	Param_DWStruct ParamDW;
	Param_UserStruct UserInfo;
	Param_NumberStruct Number;
	Param_LocatStruct LocatInfo;
	u8 pad[60];
}Param_Byte60Union;

typedef struct
{
	Param_Byte60Union Data;
	u32 CRC32;
}Param_Byte64Struct;



typedef struct
{
	u32 Data[1024];
}Section_DataStruct;

typedef struct
{
	u32 MaigcNum;
	u32 CRC32;
	u32 MainVersion;
	u32 AppVersion;
	u32 BinFileLen;
}File_HeadStruct;

typedef struct
{
	File_HeadStruct Head;
	Section_DataStruct SectionData[63];
}Upgrade_FileStruct;


typedef struct 
{
	u32 ID;
	u32 Param1;
	u32 Param2;
	void *pData;
}OperationReq_Struct;

enum EVENTENUM
{
	EVENT_THREAD_STOP = 0,
	EVENT_COM_TX,
	EVENT_MODE_SW,
	EVENT_OPERATION_REQ,
	EVENT_WAIT_NUM,
	EVENT_COM_RX = EVENT_WAIT_NUM,
	EVENT_NUM_MAX,

	COM_MODE_NORMAL = 0,
	COM_MODE_USP,
};

enum OPERATIONREQENUM
{
	REQ_RESET,
	REQ_SET_PARAM,
	REQ_UPGRADE,
};

enum WORKMODEENUM
{
	WORK_IDLE,
	WORK_USP_AUTO,
	WORK_NORMAL_COM,
};

typedef struct 
{
	u32 Var[VAR_MAX];
	u8 IMEI[IMEI_LEN];
	u8 IMSI[IMSI_LEN];
	u8 ICCID[ICCID_LEN];
	u8 State[STATE_MAX];
	u8 Error[ERROR_MAX];
	RMC_InfoStruct RMCInfo;
	GSV_InfoStruct GSVInfoSave;
	Param_Byte64Struct nParam[PARAM_TYPE_MAX];
}GDTM_DataStruct;

typedef struct  
{
	u32 ComNo;
	HANDLE hCom;
	u32 SearchBR;
	u32 CommBR;
	u8 Mode;
	u8 IsWork;
}Com_CtrlStruct;

typedef struct
{
	HANDLE hThread;
	bool ThreadRun;
	u8 WorkMode;
	HANDLE Event[EVENT_NUM_MAX];
	HWND mMainWnd;
	u32 ComNoList[MAXCOMNO];
	RBuffer OperationList;
	OperationReq_Struct OperationData[128];
	Upgrade_FileStruct UpgradeFileBuf;
	u32 CRC32Table[256];
	RBuffer UartTxBuf;
	u8 UartTxData[DBG_BUF_MAX/10];
	RBuffer UartRxBuf;
	u8 UartRxData[DBG_BUF_MAX];
	GDTM_DataStruct DevData;
	Com_CtrlStruct ComCtrl;
}SysVar_Struct;
extern SysVar_Struct gSys;
void MyDeviceInit(void);
void MyDeviceSetWnd(HWND);
void MyDeviceQuit(void);
void MyDeviceStopUSPMode(void);
void MyDeviceStartUSPMode(u32 SearchBR, u32 CommBR);
void MyDeviceOperationReq(u32 ID, u32 Param1, u32 Param2, void *pData);
void MyDeviceStopUartMode(void);
void MyDeviceStartUartMode(u32 ComNo, u32 BR);
void MyDeviceUartSend(u8 *Data, u32 Len);
#endif