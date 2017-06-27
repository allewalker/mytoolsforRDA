#pragma once
#define DBG_BUF_MAX				(10240)

#define USB_BLOCK_SIZE			(4160)
#define USB_PACKET_SIZE			(64)
#define USB_IO_FILE_PATH_LEN	(256)
#define FILE_PATH_LEN			(260)

#define SPIFLASH_RAW_LEN		(256)
#define SPIFLASH_PAGE_LEN		(256)
#define SPIFLASH_SECTOR_LEN		(4096)
#define SPIFLASH_BLOCK_LEN		(65536)
#define SPIFLASH_CHIP_LEN		(16 * 1024 * 1024)
#define FLASH_PAGE_SIZE			(1024)
#define PROFILE_NAME			L"MYConfig.ini"

#define WM_CORE_RESET				(WM_MY_MESSAGE + 1)
#define WM_CORE_UPDATE_VAR			(WM_MY_MESSAGE + 2)
#define WM_CORE_UPDATE_PARAM		(WM_MY_MESSAGE + 3)
#define WM_CORE_UPDATE_VERSION		(WM_MY_MESSAGE + 4)
#define WM_CORE_UPDATE_DL			(WM_MY_MESSAGE + 5)

#define CRC16START 0xffff
#define CRC16ROOT 0x1021
#define SATELLITEMAX 32
#define MAXCOMNO 256  //���COM��256
//#define RTCEN
enum
{
	HW_PROTOCOL_JCA01 = 0,
	HW_PROTOCOL_JCD01,
	HW_PROTOCOL_MAX,

	THREAD_STOP = 0,
	THREAD_INIT,
	THREAD_RUN,
	THREAD_QUIT
};

#define IMEI_LEN		(10)
#define IMSI_LEN		(10)
#define URL_LEN_MAX		(48)
#define AUTH_CODE_LEN	(48)
#define ICCID_LEN		(10)

enum SYS_STATE_ENUM
{
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
	WDG_STATE,
	STATE_MAX,
};

enum SYS_ERROR_ENUM
{
	//����
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
	VAR_MAX,
};


enum PARAM_ENUM
{
	PARAM_DETECT_PERIOD,
	PARAM_SENSOR_EN,
	PARAM_COM_BR,
	PARAM_GPS_BR,
	PARAM_STOP_VBAT,
	PARAM_LOW_VBAT,
	PARAM_NORMAL_VBAT,
	PARAM_SMS_ALARM,			//���ű���ʹ��
	PARAM_CALL_AUTO_GET,		//�Զ�����ʹ��
	PARAM_SIM_TO,				//SIM��ʶ��ʱ
	PARAM_GPRS_TO,				//GPRS�������賬ʱ�Ļ�����Ĭ��60,
	PARAM_SYS_MAX,

	PARAM_GS_WAKEUP_GPS = 0,
	PARAM_VACC_WAKEUP_GPS,
	PARAM_GPS_NODATA_TO,		//������ʱ��
	PARAM_GPS_V_TO,				//��Ч��λʱ��
	PARAM_GPS_KEEP_TO,			//GPS��������ʱ��
	PARAM_GPS_SLEEP_TO,			//GPS����ʱ��
	PARAM_AGPS_EN,
	PARAM_GPS_ONLY_ONCE,		//GPS��λ�͹رգ�ֱ�����߽���
	PARAM_GPS_MAX,

	PARAM_GS_WAKEUP_MONITOR = 0,	//�𶯻���ͨѶ�����Ϊ0����ΪVACC����
	PARAM_GS_JUDGE_RUN,		//��ȷ������
	PARAM_UPLOAD_RUN_PERIOD,		//���лش�����
	PARAM_UPLOAD_STOP_PERIOD,		//ͣ���ش�����
	PARAM_UPLOAD_HEART_PERIOD,	//��������
	PARAM_MONITOR_NET_TO,
	PARAM_MONITOR_KEEP_TO,
	PARAM_MONITOR_SLEEP_TO,
	PARAM_MONITOR_RECONNECT_MAX,
	PARAM_MONITOR_ADD_MILEAGE,
	PARAM_MONITOR_ACC_UPLOAD,	//ACC�仯�ϴ�
	PARAM_MONITOR_MAX,

	PARAM_VACC_CTRL_ALARM = 0,
	PARAM_ALARM_ON_DELAY,		//ACC�����𶯺��ƶ���������ʱ
	PARAM_CRASH_GS,				//����ֵ
	PARAM_CRASH_JUDGE_TO,		//�𶯱����ж�ʱ�䴰
	PARAM_CRASH_JUDGE_CNT,		//�𶯱����ж�����
	PARAM_CRASH_ALARM_WAIT_TO,
	PARAM_CRASH_ALARM_FLUSH_TO,
	PARAM_CRASH_ALARM_REPEAT,
	PARAM_CRASH_WAKEUP_MOVE,	//�ƶ�����ǰ�Ƿ��������
	PARAM_MOVE_RANGE,
	PARAM_MOVE_JUDGE_TO,
	PARAM_MOVE_ALARM_FLUSH_TO,
	PARAM_MOVE_ALARM_REPEAT,		//�ƶ������Ƿ���������
	PARAM_ALARM1_MAX,

	PARAM_VACC_WAKEUP_CUTLINE_TO = 0,
	PARAM_CUTLINE_ALARM_DELAY,
	PARAM_CUTLINE_ALARM_FLUSH_TO,	//���߱���ˢ��ʱ�䣬���Ϊ0������������
	PARAM_OVERSPEED_ALARM_VAL,		//���ٱ�����ֵ�����Ϊ0���򲻼��
	PARAM_OVERSPEED_ALARM_DELAY,	//���ٱ����ӳ٣����Ϊ0��һ���������̱���
	PARAM_ALARM_ENABLE,			//������뱨������
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

//�Ƽ���λ��ϢRMC�����ṹ��
typedef struct
{
	Date_UserDataStruct UTCDate;//UTCʱ��
	Time_UserDataStruct UTCTime;
	u32 HighLevel;
	u32 LatDegree;
	u32 LatMin;
	u32 LgtDegree;
	u32 LgtMin;
	u32 Speed;//�ֶ�7:�������ʣ�0~999.9999�ڣ�
	u32 Cog;//�ֶ�8:���溽��0 ~359.999 �ȣ����汱Ϊ�ο���׼��ǰ���0Ҳ�������䣩
	s8 LocatStatus;//�ֶ�2:��λ״̬��1=��Ч��λ��0=��Ч��λ
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
	u16 TCPPort;	//���Ϊ0����ʾʹ��UDP
	u16 UDPPort;	//���Ϊ0����ʾʹ��TCP
}Param_MainStruct;

typedef struct
{
	s8 APNName[16];
	s8 APNUser[16];
	s8 APNPassword[16];
}Param_APNStruct;

typedef struct
{
	s8 Url[URL_LEN_MAX - 12];
	s8 Usr[12];
	s8 Pwd[12];
}Param_FtpStruct;

typedef struct
{
	s8 Num[3][16];
}Param_NumberStruct;

typedef struct
{
	u32 Param[15];
}Param_DWStruct;

typedef struct
{

	u32 UpdateIP;
	s8 UpdateURL[URL_LEN_MAX];
	u16 TCPPort;	//���Ϊ0����ʾʹ��UDP
	u16 UDPPort;	//���Ϊ0����ʾʹ��TCP
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
	u32 Var[VAR_MAX];
	u8 State[STATE_MAX];
	u8 Error[ERROR_MAX];
	u8 IMEI[IMEI_LEN];
	u8 IMSI[IMSI_LEN];
	u8 ICCID[ICCID_LEN];
	RMC_InfoStruct RMC;
	GSV_InfoStruct GSV;
	Param_Byte60Union Param[PARAM_TYPE_MAX];
}GDTM_DataStruct;