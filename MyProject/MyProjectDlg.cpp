
// MyProjectDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyProject.h"
#include "MyProjectDlg.h"
#include "afxdialogex.h"
#include "AES.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CMyProjectDlg 对话框
enum 
{
	LIST_VAR_COL,
	LIST_GPS_INFO_COL,
	LIST_GPS_COL,
	LIST_BD_COL,
	LIST_MAIN_PARAM_COL,
	LIST_SYS_PARAM_COL,
	LIST_GPS_PARAM_COL,
	LIST_MONITOR_PARAM_COL,
	LIST_ALARM1_PARAM_COL,
	LIST_ALARM2_PARAM_COL,
	LIST_OTHER_PARAM_COL,
	LIST_COL_MAX,

	GPS_INFO_UTC_DATE_RAW = 0,
	GPS_INFO_UTC_TIME_RAW,
	GPS_INFO_LOCAT_RAW,
	GPS_INFO_LAT_NS_RAW,
	GPS_INFO_LAT_RAW,
	GPS_INFO_LGT_EW_RAW,
	GPS_INFO_LGT_RAW,
	GPS_INFO_SPEED_RAW,

	MAIN_PARAM_UID0_RAW = 0,
	MAIN_PARAM_UID1_RAW,
	MAIN_PARAM_UID2_RAW,
	MAIN_PARAM_IP_RAW,
	MAIN_PARAM_URL_RAW,
	MAIN_PARAM_TCP_PORT_RAW,
	MAIN_PARAM_UDP_PORT_RAW,
	MAIN_PARAM_USER_CODE_RAW,

	MAIN_VERSION_RAW = 0,
	APN_NAME_RAW,
	APN_USER_RAW,
	APN_PWD_RAW,
	IMEI_RAW,
	IMSI_RAW,
	ICCID_RAW,
	CAR_OWN0_RAW,
};


CMyProjectDlg::CMyProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyProjectDlg::IDD, pParent)
{

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	MyDeviceInit();
}

void CMyProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MY_CLEAR_BUTTON, mMyClearButton);
	DDX_Control(pDX, IDC_MY_SAVE_BUTTON, mMySaveButton);
	DDX_Control(pDX, IDC_MY_SHOW_EDIT, mMyShowEdit);
	DDX_Control(pDX, IDC_MY_SHOW_LIST, mMyShowList);
}

BEGIN_MESSAGE_MAP(CMyProjectDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_MY_CLEAR_BUTTON, &CMyProjectDlg::OnBnClickedMyClearButton)
	ON_MESSAGE(WM_TRACE, &CMyProjectDlg::OnTraceMessage)
	ON_MESSAGE(WM_CORE_RESET, &CMyProjectDlg::OnUpdateResetMessage)
	ON_MESSAGE(WM_CORE_UPDATE_VAR, &CMyProjectDlg::OnUpdateVarMessage)
	ON_MESSAGE(WM_CORE_UPDATE_PARAM, &CMyProjectDlg::OnUpdateParamMessage)
	ON_MESSAGE(WM_CORE_UPDATE_VERSION, &CMyProjectDlg::OnUpdateVersionMessage)
	ON_MESSAGE(WM_CORE_UPDATE_DL, &CMyProjectDlg::OnUpdateDLMessage)
	ON_MESSAGE(WM_COM_RX, &CMyProjectDlg::OnComRxMessage)
	ON_MESSAGE(WM_COM_STATE, &CMyProjectDlg::OnComStateMessage)
	ON_COMMAND(ID_COM_CTRL, &CMyProjectDlg::OnComCtrl)
	ON_COMMAND(ID_UPGRADE_CTRL, &CMyProjectDlg::OnUpgradeCtrl)
END_MESSAGE_MAP()


// CMyProjectDlg 消息处理程序

BOOL CMyProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CString Str;

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	mMyShowEdit.SetLimitText(UINT32_MAX);
	gDBG.SetWnd(m_hWnd);
	MyDeviceSetWnd(m_hWnd);
	mComCtrl.Create(IDD_COM_CTRL_DIALOG, this);
	mComCtrl.ShowWindow(SW_HIDE);
	mDevUpgrade.Create(IDD_DEV_UPGRADE_DIALOG, this);
	mDevUpgrade.ShowWindow(SW_HIDE);
	//初始化表格
	DWORD dwStyle = mMyShowList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;// 选中某行使整行高亮（只适用与report 风格的listctrl ）
	dwStyle |= LVS_EX_GRIDLINES;// 网格线（只适用与report 风格的listctrl ）
	mMyShowList.SetExtendedStyle(dwStyle); // 设置扩展风格 
	mMyShowList.InsertColumn(0, L"序号", LVCFMT_CENTER, 40);// 插入列
	mMyShowList.InsertColumn(1 + 2 * LIST_VAR_COL, L"变量", LVCFMT_CENTER, 80);// 插入列
	mMyShowList.InsertColumn(2 + 2 * LIST_VAR_COL, L"", LVCFMT_CENTER, 80);// 插入列
	mMyShowList.InsertColumn(1 + 2 * LIST_GPS_INFO_COL, L"GPS信息", LVCFMT_CENTER, 60);// 插入列
	mMyShowList.InsertColumn(2 + 2 * LIST_GPS_INFO_COL, L"", LVCFMT_CENTER, 80);// 插入列
	mMyShowList.InsertColumn(1 + 2 * LIST_MAIN_PARAM_COL, L"主要参数", LVCFMT_CENTER, 90);// 插入列
	mMyShowList.InsertColumn(2 + 2 * LIST_MAIN_PARAM_COL, L"", LVCFMT_CENTER, 80);// 插入列
	mMyShowList.InsertColumn(1 + 2 * LIST_SYS_PARAM_COL, L"系统参数", LVCFMT_CENTER, 90);// 插入列
	mMyShowList.InsertColumn(2 + 2 * LIST_SYS_PARAM_COL, L"", LVCFMT_CENTER, 40);// 插入列
	mMyShowList.InsertColumn(1 + 2 * LIST_GPS_PARAM_COL, L"GPS参数", LVCFMT_CENTER, 90);// 插入列
	mMyShowList.InsertColumn(2 + 2 * LIST_GPS_PARAM_COL, L"", LVCFMT_CENTER, 40);// 插入列
	mMyShowList.InsertColumn(1 + 2 * LIST_MONITOR_PARAM_COL, L"监控参数", LVCFMT_CENTER, 90);// 插入列
	mMyShowList.InsertColumn(2 + 2 * LIST_MONITOR_PARAM_COL, L"", LVCFMT_CENTER, 40);// 插入列
	mMyShowList.InsertColumn(1 + 2 * LIST_ALARM1_PARAM_COL, L"报警参数1", LVCFMT_CENTER, 90);// 插入列
	mMyShowList.InsertColumn(2 + 2 * LIST_ALARM1_PARAM_COL, L"", LVCFMT_CENTER, 40);// 插入列
	mMyShowList.InsertColumn(1 + 2 * LIST_ALARM2_PARAM_COL, L"报警参数2", LVCFMT_CENTER, 90);// 插入列
	mMyShowList.InsertColumn(2 + 2 * LIST_ALARM2_PARAM_COL, L"", LVCFMT_CENTER, 40);// 插入列
	mMyShowList.InsertColumn(1 + 2 * LIST_OTHER_PARAM_COL, L"其他参数", LVCFMT_CENTER, 90);// 插入列
	mMyShowList.InsertColumn(2 + 2 * LIST_OTHER_PARAM_COL, L"", LVCFMT_CENTER, 200);// 插入列
	mMyShowList.InsertColumn(1 + 2 * LIST_GPS_COL, L"GPS卫星", LVCFMT_CENTER, 60);// 插入列
	mMyShowList.InsertColumn(2 + 2 * LIST_GPS_COL, L"", LVCFMT_CENTER, 40);// 插入列
	mMyShowList.InsertColumn(1 + 2 * LIST_BD_COL, L"BD卫星", LVCFMT_CENTER, 60);// 插入列
	mMyShowList.InsertColumn(2 + 2 * LIST_BD_COL, L"", LVCFMT_CENTER, 40);// 插入列
	for (int i = 0; i < 100; i++)
	{
		Str.Format(_T("%d"), i);
		mMyShowList.InsertItem(i, Str);
	}
	mMyShowList.SetItemText(SYS_TIME, 1 + 2 * LIST_VAR_COL, L"运行时间");
	mMyShowList.SetItemText(VBAT, 1 + 2 * LIST_VAR_COL, L"电压");
	mMyShowList.SetItemText(UTC_DATE, 1 + 2 * LIST_VAR_COL, L"UTC日期");
	mMyShowList.SetItemText(UTC_TIME, 1 + 2 * LIST_VAR_COL, L"UTC时间");
	mMyShowList.SetItemText(IO_VAL, 1 + 2 * LIST_VAR_COL, L"VACC");
	mMyShowList.SetItemText(GSENSOR_VAL, 1 + 2 * LIST_VAR_COL, L"实时GS");
	mMyShowList.SetItemText(GSENSOR_ALARM_VAL, 1 + 2 * LIST_VAR_COL, L"报警GS");
	mMyShowList.SetItemText(GSENSOR_MONITOR_VAL, 1 + 2 * LIST_VAR_COL, L"监控GS");
	mMyShowList.SetItemText(GSENSOR_KEEP_VAL, 1 + 2 * LIST_VAR_COL, L"缓存GS");
	mMyShowList.SetItemText(CELL_ID, 1 + 2 * LIST_VAR_COL, L"当前基站");
	mMyShowList.SetItemText(SHUTDOWN_TIME, 1 + 2 * LIST_VAR_COL, L"关机时间");
	mMyShowList.SetItemText(SOFTWARE_VERSION, 1 + 2 * LIST_VAR_COL, L"软件版本");
	mMyShowList.SetItemText(MAIN_FREQ, 1 + 2 * LIST_VAR_COL, L"主频");
	mMyShowList.SetItemText(TRACE_TO, 1 + 2 * LIST_VAR_COL, L"TRACE TO");

	mMyShowList.SetItemText(SYSTEM_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"系统状态");
	mMyShowList.SetItemText(SIM_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"SIM卡状态");
	mMyShowList.SetItemText(SMS_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"SMS状态");
	mMyShowList.SetItemText(REG_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"注册状态");
	mMyShowList.SetItemText(GPRS_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"GPRS状态");
	mMyShowList.SetItemText(GPRS_ATTACH_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"GPRS附着");
	mMyShowList.SetItemText(GPRS_ACT_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"GPRS激活");
	mMyShowList.SetItemText(MONITOR_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"监控运行");
	mMyShowList.SetItemText(RSSI_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"信号");
	mMyShowList.SetItemText(GPS_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"GPS状态");
	mMyShowList.SetItemText(ALARM_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"防盗状态");
	mMyShowList.SetItemText(CRASH_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"碰撞");
	mMyShowList.SetItemText(MOVE_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"移动");
	mMyShowList.SetItemText(CUTLINE_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"剪线");
	mMyShowList.SetItemText(OVERSPEED_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"超速");
	mMyShowList.SetItemText(PRINT_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"打印模式");
	mMyShowList.SetItemText(FIRST_LOCAT_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"首次定位");
	mMyShowList.SetItemText(TRACE_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"TRACE状态");
	mMyShowList.SetItemText(WDG_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"WDG VAL");
	mMyShowList.SetItemText(LED_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"绿灯");
	mMyShowList.SetItemText(LED_STATE + 1 + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"红灯");
	mMyShowList.SetItemText(REBOOT_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"重启");


	mMyShowList.SetItemText(SIM_ERROR + VAR_MAX + STATE_MAX, 1 + 2 * LIST_VAR_COL, L"SIM卡故障");
	mMyShowList.SetItemText(GPRS_ERROR + VAR_MAX + STATE_MAX, 1 + 2 * LIST_VAR_COL, L"GPRS故障");
	mMyShowList.SetItemText(GPS_ERROR + VAR_MAX + STATE_MAX, 1 + 2 * LIST_VAR_COL, L"GPS故障");
	mMyShowList.SetItemText(ANT_SHORT_ERROR + VAR_MAX + STATE_MAX, 1 + 2 * LIST_VAR_COL, L"天线短路");
	mMyShowList.SetItemText(ANT_BREAK_ERROR + VAR_MAX + STATE_MAX, 1 + 2 * LIST_VAR_COL, L"天线断路");
	mMyShowList.SetItemText(SENSOR_ERROR + VAR_MAX + STATE_MAX, 1 + 2 * LIST_VAR_COL, L"GS故障");
	mMyShowList.SetItemText(LOW_POWER_ERROR + VAR_MAX + STATE_MAX, 1 + 2 * LIST_VAR_COL, L"低电故障");

	mMyShowList.SetItemText(GPS_INFO_UTC_DATE_RAW, 1 + 2 * LIST_GPS_INFO_COL, L"GPS日期");
	mMyShowList.SetItemText(GPS_INFO_UTC_TIME_RAW, 1 + 2 * LIST_GPS_INFO_COL, L"GPS时间");
	mMyShowList.SetItemText(GPS_INFO_LOCAT_RAW, 1 + 2 * LIST_GPS_INFO_COL, L"定位");
	mMyShowList.SetItemText(GPS_INFO_LAT_NS_RAW, 1 + 2 * LIST_GPS_INFO_COL, L"NS");
	mMyShowList.SetItemText(GPS_INFO_LAT_RAW, 1 + 2 * LIST_GPS_INFO_COL, L"纬度");
	mMyShowList.SetItemText(GPS_INFO_LGT_EW_RAW, 1 + 2 * LIST_GPS_INFO_COL, L"EW");
	mMyShowList.SetItemText(GPS_INFO_LGT_RAW, 1 + 2 * LIST_GPS_INFO_COL, L"经度");
	mMyShowList.SetItemText(GPS_INFO_SPEED_RAW, 1 + 2 * LIST_GPS_INFO_COL, L"速度节");


	mMyShowList.SetItemText(MAIN_PARAM_UID0_RAW, 1 + 2 * LIST_MAIN_PARAM_COL, L"UID0");
	mMyShowList.SetItemText(MAIN_PARAM_UID1_RAW, 1 + 2 * LIST_MAIN_PARAM_COL, L"UID1");
	mMyShowList.SetItemText(MAIN_PARAM_UID2_RAW, 1 + 2 * LIST_MAIN_PARAM_COL, L"UID2");
	mMyShowList.SetItemText(MAIN_PARAM_IP_RAW, 1 + 2 * LIST_MAIN_PARAM_COL, L"主IP");
	mMyShowList.SetItemText(MAIN_PARAM_URL_RAW, 1 + 2 * LIST_MAIN_PARAM_COL, L"主域名");
	mMyShowList.SetItemText(MAIN_PARAM_TCP_PORT_RAW, 1 + 2 * LIST_MAIN_PARAM_COL, L"TCP端口");
	mMyShowList.SetItemText(MAIN_PARAM_UDP_PORT_RAW, 1 + 2 * LIST_MAIN_PARAM_COL, L"UDP端口");
	mMyShowList.SetItemText(MAIN_PARAM_USER_CODE_RAW, 1 + 2 * LIST_MAIN_PARAM_COL, L"用户代号");

	mMyShowList.SetItemText(PARAM_DETECT_PERIOD, 1 + 2 * LIST_SYS_PARAM_COL, L"检测周期");
	mMyShowList.SetItemText(PARAM_SENSOR_EN, 1 + 2 * LIST_SYS_PARAM_COL, L"检测使能");
	mMyShowList.SetItemText(PARAM_COM_BR, 1 + 2 * LIST_SYS_PARAM_COL, L"通讯BR");
	mMyShowList.SetItemText(PARAM_GPS_BR, 1 + 2 * LIST_SYS_PARAM_COL, L"GPSBR");
	mMyShowList.SetItemText(PARAM_STOP_VBAT, 1 + 2 * LIST_SYS_PARAM_COL, L"停止电压");
	mMyShowList.SetItemText(PARAM_LOW_VBAT, 1 + 2 * LIST_SYS_PARAM_COL, L"低电电压");
	mMyShowList.SetItemText(PARAM_NORMAL_VBAT, 1 + 2 * LIST_SYS_PARAM_COL, L"正常电压");
	mMyShowList.SetItemText(PARAM_SMS_ALARM, 1 + 2 * LIST_SYS_PARAM_COL, L"短信报警");
	mMyShowList.SetItemText(PARAM_CALL_AUTO_GET, 1 + 2 * LIST_SYS_PARAM_COL, L"自动接听");
	mMyShowList.SetItemText(PARAM_SIM_TO, 1 + 2 * LIST_SYS_PARAM_COL, L"SIM卡超时");
	mMyShowList.SetItemText(PARAM_GPRS_TO, 1 + 2 * LIST_SYS_PARAM_COL, L"GPRS超时");

	mMyShowList.SetItemText(PARAM_GS_WAKEUP_GPS, 1 + 2 * LIST_GPS_PARAM_COL, L"震动唤醒");
	mMyShowList.SetItemText(PARAM_VACC_WAKEUP_GPS, 1 + 2 * LIST_GPS_PARAM_COL, L"ACC唤醒");
	mMyShowList.SetItemText(PARAM_GPS_NODATA_TO, 1 + 2 * LIST_GPS_PARAM_COL, L"无数据时间");
	mMyShowList.SetItemText(PARAM_GPS_V_TO, 1 + 2 * LIST_GPS_PARAM_COL, L"不定位时间");
	mMyShowList.SetItemText(PARAM_GPS_KEEP_TO, 1 + 2 * LIST_GPS_PARAM_COL, L"唤醒保持时间");
	mMyShowList.SetItemText(PARAM_GPS_SLEEP_TO, 1 + 2 * LIST_GPS_PARAM_COL, L"唤醒周期");
	mMyShowList.SetItemText(PARAM_AGPS_EN, 1 + 2 * LIST_GPS_PARAM_COL, L"启用AGPS");
	mMyShowList.SetItemText(PARAM_GPS_ONLY_ONCE, 1 + 2 * LIST_GPS_PARAM_COL, L"只定位一次");

	mMyShowList.SetItemText(PARAM_GS_WAKEUP_MONITOR, 1 + 2 * LIST_MONITOR_PARAM_COL, L"GS唤醒");
	mMyShowList.SetItemText(PARAM_GS_JUDGE_RUN, 1 + 2 * LIST_MONITOR_PARAM_COL, L"GS判断行车");
	mMyShowList.SetItemText(PARAM_UPLOAD_RUN_PERIOD, 1 + 2 * LIST_MONITOR_PARAM_COL, L"行车回传");
	mMyShowList.SetItemText(PARAM_UPLOAD_STOP_PERIOD, 1 + 2 * LIST_MONITOR_PARAM_COL, L"停车回传");
	mMyShowList.SetItemText(PARAM_UPLOAD_HEART_PERIOD, 1 + 2 * LIST_MONITOR_PARAM_COL, L"心跳间隔");
	mMyShowList.SetItemText(PARAM_MONITOR_NET_TO, 1 + 2 * LIST_MONITOR_PARAM_COL, L"TCP发送超时");
	mMyShowList.SetItemText(PARAM_MONITOR_KEEP_TO, 1 + 2 * LIST_MONITOR_PARAM_COL, L"保持时间");
	mMyShowList.SetItemText(PARAM_MONITOR_SLEEP_TO, 1 + 2 * LIST_MONITOR_PARAM_COL, L"睡眠时间");
	mMyShowList.SetItemText(PARAM_MONITOR_RECONNECT_MAX, 1 + 2 * LIST_MONITOR_PARAM_COL, L"重连次数");
	mMyShowList.SetItemText(PARAM_MONITOR_ADD_MILEAGE, 1 + 2 * LIST_MONITOR_PARAM_COL, L"附加里程");
	mMyShowList.SetItemText(PARAM_MONITOR_ACC_UPLOAD, 1 + 2 * LIST_MONITOR_PARAM_COL, L"ACC变化上传");

	mMyShowList.SetItemText(PARAM_VACC_CTRL_ALARM, 1 + 2 * LIST_ALARM1_PARAM_COL, L"ACC控制");
	mMyShowList.SetItemText(PARAM_ALARM_ON_DELAY, 1 + 2 * LIST_ALARM1_PARAM_COL, L"报警检测延迟");
	mMyShowList.SetItemText(PARAM_CRASH_GS, 1 + 2 * LIST_ALARM1_PARAM_COL, L"震动阀值");
	mMyShowList.SetItemText(PARAM_CRASH_JUDGE_TO, 1 + 2 * LIST_ALARM1_PARAM_COL, L"震动报警时间");
	mMyShowList.SetItemText(PARAM_CRASH_JUDGE_CNT, 1 + 2 * LIST_ALARM1_PARAM_COL, L"震动次数阀值");

	mMyShowList.SetItemText(PARAM_CRASH_ALARM_WAIT_TO, 1 + 2 * LIST_ALARM1_PARAM_COL, L"震动报警延迟");
	mMyShowList.SetItemText(PARAM_CRASH_ALARM_FLUSH_TO, 1 + 2 * LIST_ALARM1_PARAM_COL, L"震动报警间隔");
	mMyShowList.SetItemText(PARAM_CRASH_ALARM_REPEAT, 1 + 2 * LIST_ALARM1_PARAM_COL, L"震动连续报警");
	mMyShowList.SetItemText(PARAM_CRASH_WAKEUP_MOVE, 1 + 2 * LIST_ALARM1_PARAM_COL, L"震动使能移动");
	mMyShowList.SetItemText(PARAM_MOVE_RANGE, 1 + 2 * LIST_ALARM1_PARAM_COL, L"移动报警半径");

	mMyShowList.SetItemText(PARAM_MOVE_JUDGE_TO, 1 + 2 * LIST_ALARM1_PARAM_COL, L"移动检测时间");
	mMyShowList.SetItemText(PARAM_MOVE_ALARM_FLUSH_TO, 1 + 2 * LIST_ALARM1_PARAM_COL, L"移动报警间隔");
	mMyShowList.SetItemText(PARAM_MOVE_ALARM_REPEAT, 1 + 2 * LIST_ALARM1_PARAM_COL, L"移动连续报警");

	mMyShowList.SetItemText(PARAM_VACC_WAKEUP_CUTLINE_TO, 1 + 2 * LIST_ALARM2_PARAM_COL, L"ACC关延迟");
	mMyShowList.SetItemText(PARAM_CUTLINE_ALARM_FLUSH_TO, 1 + 2 * LIST_ALARM2_PARAM_COL, L"剪线刷新时间");
	mMyShowList.SetItemText(PARAM_CUTLINE_ALARM_DELAY, 1 + 2 * LIST_ALARM2_PARAM_COL, L"剪线报警延迟");
	mMyShowList.SetItemText(PARAM_OVERSPEED_ALARM_VAL, 1 + 2 * LIST_ALARM2_PARAM_COL, L"超速报警阈值");
	mMyShowList.SetItemText(PARAM_OVERSPEED_ALARM_DELAY, 1 + 2 * LIST_ALARM2_PARAM_COL, L"超速报警延迟");
	mMyShowList.SetItemText(PARAM_ALARM_ENABLE, 1 + 2 * LIST_ALARM2_PARAM_COL, L"报警使能");

	mMyShowList.SetItemText(MAIN_VERSION_RAW, 1 + 2 * LIST_OTHER_PARAM_COL, L"核心版本");

	mMyShowList.SetItemText(APN_NAME_RAW, 1 + 2 * LIST_OTHER_PARAM_COL, L"APN名字");
	mMyShowList.SetItemText(APN_USER_RAW, 1 + 2 * LIST_OTHER_PARAM_COL, L"APN用户名");
	mMyShowList.SetItemText(APN_PWD_RAW, 1 + 2 * LIST_OTHER_PARAM_COL, L"APN密码");

	mMyShowList.SetItemText(IMEI_RAW, 1 + 2 * LIST_OTHER_PARAM_COL, L"IMEI");
	mMyShowList.SetItemText(IMSI_RAW, 1 + 2 * LIST_OTHER_PARAM_COL, L"IMSI");
	mMyShowList.SetItemText(ICCID_RAW, 1 + 2 * LIST_OTHER_PARAM_COL, L"ICCID");

	mMyShowList.SetItemText(CAR_OWN0_RAW, 1 + 2 * LIST_OTHER_PARAM_COL, L"车主号码1");
	mMyShowList.SetItemText(CAR_OWN0_RAW + 1, 1 + 2 * LIST_OTHER_PARAM_COL, L"车主号码2");
	mMyShowList.SetItemText(CAR_OWN0_RAW + 2, 1 + 2 * LIST_OTHER_PARAM_COL, L"车主号码3");
	mMyShowList.SetItemText(CAR_OWN0_RAW + 3, 1 + 2 * LIST_OTHER_PARAM_COL, L"车主号码4");
	mMyShowList.SetItemText(CAR_OWN0_RAW + 4, 1 + 2 * LIST_OTHER_PARAM_COL, L"车主号码5");
	mMyShowList.SetItemText(CAR_OWN0_RAW + 5, 1 + 2 * LIST_OTHER_PARAM_COL, L"车主号码6");
	mMyShowList.SetItemText(CAR_OWN0_RAW + 6, 1 + 2 * LIST_OTHER_PARAM_COL, L"车主号码7");
	
	Reset();

// 	u8 Data[] = { 0x11, 0x01, 0x07, 0x52, 0x53, 0x36, 0x78, 0x90, 0x02, 0x42, 0x70, 0x00, 0x32, 0x01, 0x00, 0x05 };
// 	u16 crc16 = ~CRC16Cal(Data, sizeof(Data), CRC16_START, CRC16_CCITT_GEN, 1);
// 	gDBG.Trace("%08x", crc16);
// 	AES_CtrlStruct AES;
// 	u8 Key[16] = { 32, 87, 47, 82, 54, 75, 63, 71, 48, 80, 65, 88, 17, 99, 45, 43 };
// 	u8 Temp1[16] = { 0x06, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// 	u8 Temp2[16] = { 0xcf, 0x2a, 0x12, 0xfd, 0xf7, 0x8f, 0xa0, 0xa8, 0x35, 0x83, 0x27, 0x5e, 0x9f, 0xd0, 0x73, 0xdc };
// 	u8 Temp3[16];
// 	memset(&AES, 0, sizeof(AES));
// 	memcpy(AES.AESKey, Key, 16);
// 	memset(Temp3, 0x00, sizeof(Temp3));
// 	
// 	AES_EncInit(&AES);
// 	AES_Encrypt(&AES, Temp1, Temp3);
// 	gDBG.HexTrace(Temp3, 16);
// 
// 	memset(Temp3, 0x00, sizeof(Temp3));
// 	AES_DecInit(&AES);
// 	AES_Decrypt(&AES, Temp2, Temp3);
// 	gDBG.HexTrace(Temp3, 16);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMyProjectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMyProjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//接收到结束消息
void CMyProjectDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (MessageBox(L"您确定要退出程序吗", L"提示", MB_ICONINFORMATION|MB_YESNO) == IDNO)
	{
		return;
	}
	mComCtrl.Stop();
	MyDeviceQuit();
	WaitForSingleObject(gSys.hThread, 50);
	CDialogEx::OnClose();
}


void CMyProjectDlg::OnBnClickedMyClearButton()
{
	// TODO:  在此添加控件通知处理程序代码
	mMyShowEdit.SetWindowTextW(L"");
	Reset();
}

LRESULT CMyProjectDlg::OnTraceMessage(WPARAM wParam, LPARAM lParam)
{
	uint8_t Buf[DBG_BUF_MAX + 4];
	uint8_t Buf1[DBG_BUF_MAX + 4];
	uint16_t Buf2[DBG_BUF_MAX + 4];
	uint32_t Len;
	DWORD Result = WaitForSingleObject(gDBG.hInSem, 100);
	switch (Result)
	{
	case WAIT_OBJECT_0:
		if (gDBG.m_MsgBuffer.Len >= DBG_BUF_MAX)
		{
			Len = ReadRBuffer(&gDBG.m_MsgBuffer, Buf, DBG_BUF_MAX);
		} 
		else
		{
			memcpy(Buf, gDBG.m_MsgBuffer.Data, gDBG.m_MsgBuffer.Len);
			Len = gDBG.m_MsgBuffer.Len;
			gDBG.m_MsgBuffer.Len = 0;
			gDBG.m_MsgBuffer.Offset = 0;
		}
		ReleaseSemaphore(gDBG.hInSem, 1, NULL);
		mLastByte = ShowReceiveData(Buf, Len, Buf1, Buf2, &mMyShowEdit, mLastByte);
		break;
	case WAIT_FAILED:
		//ReleaseSemaphore(gDBG.hInSem, 1, NULL);
		break;
	case WAIT_TIMEOUT:
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CMyProjectDlg::OnUpdateResetMessage(WPARAM wParam, LPARAM lParam)
{
	Reset();
	return 0;
}

LRESULT CMyProjectDlg::OnUpdateVarMessage(WPARAM wParam, LPARAM lParam)
{
	int i;
	CString Str;
	IO_ValueUnion uIO;
	Date_Union uDate;
	Time_Union uTime;
	u32 dwTemp;
	for (i = 0; i < VAR_MAX; i++)
	{
		if (gSys.DevData.Var[i] != ShowData.Var[i])
		{
			ShowData.Var[i] = gSys.DevData.Var[i];
			switch (i)
			{
			case UTC_DATE:
				uDate.dwDate = ShowData.Var[i];
				Str.Format(_T("%d-%d-%d"), uDate.Date.Year, uDate.Date.Mon, uDate.Date.Day);
				break;
			case UTC_TIME:
				uTime.dwTime = ShowData.Var[i];
				Str.Format(_T("%d:%d:%d"), uTime.Time.Hour, uTime.Time.Min, uTime.Time.Sec);
				break;
			case IO_VAL:
				uIO.Val = ShowData.Var[i];
				Str.Format(_T("%d:%d:%d"), uIO.IOVal.VACC, uIO.IOVal.VCC, uIO.IOVal.ACC);
				break;
			case CELL_ID:
				Str.Format(_T("%08x"), ShowData.Var[i]);
				break;
			case GSENSOR_VAL:
			case GSENSOR_ALARM_VAL:
			case GSENSOR_MONITOR_VAL:
			case GSENSOR_KEEP_VAL:
				if (ShowData.Var[i] > 5)
				{
					Str.Format(_T("%d"), ShowData.Var[i]);
				}
				else
				{
					Str.Format(_T("%d"), 0);
				}
				break;
			default:
				Str.Format(_T("%d"), ShowData.Var[i]);
				break;
			}
			mMyShowList.SetItemText(i, 2 + 2 * LIST_VAR_COL, Str);
		}
	}

	for (i = 0; i < STATE_MAX; i++)
	{
		if (gSys.DevData.State[i] != ShowData.State[i])
		{
			ShowData.State[i] = gSys.DevData.State[i];
			Str.Format(_T("%d"), ShowData.State[i]);
			mMyShowList.SetItemText(i + VAR_MAX, 2 + 2 * LIST_VAR_COL, Str);
		}
	}

	for (i = 0; i < ERROR_MAX; i++)
	{
		if (gSys.DevData.Error[i] != ShowData.Error[i])
		{
			ShowData.Error[i] = gSys.DevData.Error[i];
			Str.Format(_T("%d"), ShowData.Error[i]);
			mMyShowList.SetItemText(i + VAR_MAX + STATE_MAX, 2 + 2 * LIST_VAR_COL, Str);
		}
	}

	if (memcmp(ShowData.IMEI, gSys.DevData.IMEI, IMEI_LEN))
	{
		memcpy(ShowData.IMEI, gSys.DevData.IMEI, IMEI_LEN);
		Str.Format(_T("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x"), ShowData.IMEI[0], ShowData.IMEI[1], ShowData.IMEI[2], ShowData.IMEI[3], ShowData.IMEI[4]
			, ShowData.IMEI[5], ShowData.IMEI[6], ShowData.IMEI[7], ShowData.IMEI[8], ShowData.IMEI[9]);
		mMyShowList.SetItemText(IMEI_RAW, 2 + 2 * LIST_OTHER_PARAM_COL, Str);
	}

	if (memcmp(ShowData.IMSI, gSys.DevData.IMSI, IMSI_LEN))
	{
		memcpy(ShowData.IMSI, gSys.DevData.IMSI, IMSI_LEN);
		Str.Format(_T("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x"), ShowData.IMSI[0], ShowData.IMSI[1], ShowData.IMSI[2], ShowData.IMSI[3], ShowData.IMSI[4]
			, ShowData.IMSI[5], ShowData.IMSI[6], ShowData.IMSI[7], ShowData.IMSI[8], ShowData.IMSI[9]);
		mMyShowList.SetItemText(IMSI_RAW, 2 + 2 * LIST_OTHER_PARAM_COL, Str);
	}

	if (memcmp(ShowData.ICCID, gSys.DevData.ICCID, ICCID_LEN))
	{
		memcpy(ShowData.ICCID, gSys.DevData.ICCID, ICCID_LEN);
		Str.Format(_T("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x"), ShowData.ICCID[0], ShowData.ICCID[1], ShowData.ICCID[2], ShowData.ICCID[3], ShowData.ICCID[4]
			, ShowData.ICCID[5], ShowData.ICCID[6], ShowData.ICCID[7], ShowData.ICCID[8], ShowData.ICCID[9]);
		mMyShowList.SetItemText(ICCID_RAW, 2 + 2 * LIST_OTHER_PARAM_COL, Str);
	}

	if (memcmp(&ShowData.RMCInfo.UTCDate, &gSys.DevData.RMCInfo.UTCDate, sizeof(Date_UserDataStruct)))
	{
		ShowData.RMCInfo.UTCDate = gSys.DevData.RMCInfo.UTCDate;
		uDate.Date = ShowData.RMCInfo.UTCDate;
		Str.Format(_T("%d-%d-%d"), uDate.Date.Year, uDate.Date.Mon, uDate.Date.Day);
		mMyShowList.SetItemText(GPS_INFO_UTC_DATE_RAW, 2 + 2 * LIST_GPS_INFO_COL, Str);
	}

	if (memcmp(&ShowData.RMCInfo.UTCTime, &gSys.DevData.RMCInfo.UTCTime, sizeof(Date_UserDataStruct)))
	{
		ShowData.RMCInfo.UTCTime = gSys.DevData.RMCInfo.UTCTime;
		uTime.Time = ShowData.RMCInfo.UTCTime;
		Str.Format(_T("%d:%d:%d"), uTime.Time.Hour, uTime.Time.Min, uTime.Time.Sec);
		mMyShowList.SetItemText(GPS_INFO_UTC_TIME_RAW, 2 + 2 * LIST_GPS_INFO_COL, Str);
	}

	if (ShowData.RMCInfo.LocatStatus != gSys.DevData.RMCInfo.LocatStatus)
	{	
		ShowData.RMCInfo.LocatStatus = gSys.DevData.RMCInfo.LocatStatus;
		Str.Format(_T("%d"), ShowData.RMCInfo.LocatStatus);
		mMyShowList.SetItemText(GPS_INFO_LOCAT_RAW, 2 + 2 * LIST_GPS_INFO_COL, Str);
	} 

	if (ShowData.RMCInfo.Speed != gSys.DevData.RMCInfo.Speed)
	{
		ShowData.RMCInfo.Speed = gSys.DevData.RMCInfo.Speed;
		Str.Format(_T("%d"), ShowData.RMCInfo.Speed);
		mMyShowList.SetItemText(GPS_INFO_SPEED_RAW, 2 + 2 * LIST_GPS_INFO_COL, Str);
	}

	if (ShowData.RMCInfo.LatNS != gSys.DevData.RMCInfo.LatNS)
	{
		ShowData.RMCInfo.LatNS = gSys.DevData.RMCInfo.LatNS;
		Str.Format(_T("%c"), ShowData.RMCInfo.LatNS);
		mMyShowList.SetItemText(GPS_INFO_LAT_NS_RAW, 2 + 2 * LIST_GPS_INFO_COL, Str);
	}

	if (ShowData.RMCInfo.LgtEW != gSys.DevData.RMCInfo.LgtEW)
	{
		ShowData.RMCInfo.LgtEW = gSys.DevData.RMCInfo.LgtEW;
		Str.Format(_T("%c"), ShowData.RMCInfo.LgtEW);
		mMyShowList.SetItemText(GPS_INFO_LGT_EW_RAW, 2 + 2 * LIST_GPS_INFO_COL, Str);
	}

	if ((ShowData.RMCInfo.LatDegree * 1000000 + ShowData.RMCInfo.LatMin) != (gSys.DevData.RMCInfo.LatDegree * 1000000 + gSys.DevData.RMCInfo.LatMin))
	{
		ShowData.RMCInfo.LatDegree = gSys.DevData.RMCInfo.LatDegree;
		ShowData.RMCInfo.LatMin = gSys.DevData.RMCInfo.LatMin;
		dwTemp = ShowData.RMCInfo.LatDegree * 1000000 + ShowData.RMCInfo.LatMin;
		Str.Format(_T("%d"), dwTemp);
		mMyShowList.SetItemText(GPS_INFO_LAT_RAW, 2 + 2 * LIST_GPS_INFO_COL, Str);
	}

	if ((ShowData.RMCInfo.LgtDegree * 1000000 + ShowData.RMCInfo.LgtMin) != (gSys.DevData.RMCInfo.LgtDegree * 1000000 + gSys.DevData.RMCInfo.LgtMin))
	{
		ShowData.RMCInfo.LgtDegree = gSys.DevData.RMCInfo.LgtDegree;
		ShowData.RMCInfo.LgtMin = gSys.DevData.RMCInfo.LgtMin;
		dwTemp = ShowData.RMCInfo.LgtDegree * 1000000 + ShowData.RMCInfo.LgtMin;
		Str.Format(_T("%d"), dwTemp);
		mMyShowList.SetItemText(GPS_INFO_LGT_RAW, 2 + 2 * LIST_GPS_INFO_COL, Str);
	}

	return 0;
}

LRESULT CMyProjectDlg::OnUpdateParamMessage(WPARAM wParam, LPARAM lParam)
{
	int i;
	CString Str;
	IP_AddrUnion uIP;
	wchar_t wbuf[URL_LEN_MAX];
	u8 PhoneBCD[7];
	for (i = 0; i < 3; i++)
	{
		if (gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UID[i] != ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UID[i])
		{
			ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UID[i] = gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UID[i];
			Str.Format(_T("%d"), ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UID[i]);
			mMyShowList.SetItemText(i + MAIN_PARAM_UID0_RAW, 2 + 2 * LIST_MAIN_PARAM_COL, Str);
		}
	}

	if (gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.MainIP != ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.MainIP)
	{
		ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.MainIP = gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.MainIP;
		uIP.u32_addr = ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.MainIP;
		Str.Format(_T("%d.%d.%d.%d"), uIP.u8_addr[0], uIP.u8_addr[1], uIP.u8_addr[2], uIP.u8_addr[3]);
		mMyShowList.SetItemText(MAIN_PARAM_IP_RAW, 2 + 2 * LIST_MAIN_PARAM_COL, Str);
	}

	if (memcmp(gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.MainURL, ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.MainURL, sizeof(ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.MainURL)))
	{
		memcpy(ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.MainURL, gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.MainURL, sizeof(ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.MainURL));
		MultiByteToWideChar(CP_ACP, 0, (char *)ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.MainURL, strlen(ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.MainURL) + 1, wbuf, strlen(ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.MainURL) + 1);
		Str.Format(_T("%s"), wbuf);
		mMyShowList.SetItemText(MAIN_PARAM_URL_RAW, 2 + 2 * LIST_MAIN_PARAM_COL, Str);
	}

	if (gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.TCPPort != ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.TCPPort)
	{
		ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.TCPPort = gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.TCPPort;
		Str.Format(_T("%d"), ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.TCPPort);
		mMyShowList.SetItemText(MAIN_PARAM_TCP_PORT_RAW, 2 + 2 * LIST_MAIN_PARAM_COL, Str);
	}

	if (gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UDPPort != ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UDPPort)
	{
		ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UDPPort = gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UDPPort;
		Str.Format(_T("%d"), ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.UDPPort);
		mMyShowList.SetItemText(MAIN_PARAM_UDP_PORT_RAW, 2 + 2 * LIST_MAIN_PARAM_COL, Str);
	}

	if (gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.CustCode != ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.CustCode)
	{
		ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.CustCode = gSys.DevData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.CustCode;
		Str.Format(_T("%d"), ShowData.nParam[PARAM_TYPE_MAIN].Data.MainInfo.CustCode);
		mMyShowList.SetItemText(MAIN_PARAM_USER_CODE_RAW, 2 + 2 * LIST_MAIN_PARAM_COL, Str);
	}

	for (i = 0; i < PARAM_SYS_MAX; i++)
	{
		if (gSys.DevData.nParam[PARAM_TYPE_SYS].Data.ParamDW.Param[i] != ShowData.nParam[PARAM_TYPE_SYS].Data.ParamDW.Param[i])
		{
			ShowData.nParam[PARAM_TYPE_SYS].Data.ParamDW.Param[i] = gSys.DevData.nParam[PARAM_TYPE_SYS].Data.ParamDW.Param[i];
			Str.Format(_T("%d"), ShowData.nParam[PARAM_TYPE_SYS].Data.ParamDW.Param[i]);
			mMyShowList.SetItemText(i, 2 + 2 * LIST_SYS_PARAM_COL, Str);
		}
	}

	for (i = 0; i < PARAM_SYS_MAX; i++)
	{
		if (gSys.DevData.nParam[PARAM_TYPE_SYS].Data.ParamDW.Param[i] != ShowData.nParam[PARAM_TYPE_SYS].Data.ParamDW.Param[i])
		{
			ShowData.nParam[PARAM_TYPE_SYS].Data.ParamDW.Param[i] = gSys.DevData.nParam[PARAM_TYPE_SYS].Data.ParamDW.Param[i];
			Str.Format(_T("%d"), ShowData.nParam[PARAM_TYPE_SYS].Data.ParamDW.Param[i]);
			mMyShowList.SetItemText(i, 2 + 2 * LIST_SYS_PARAM_COL, Str);
		}
	}

	for (i = 0; i < PARAM_GPS_MAX; i++)
	{
		if (gSys.DevData.nParam[PARAM_TYPE_GPS].Data.ParamDW.Param[i] != ShowData.nParam[PARAM_TYPE_GPS].Data.ParamDW.Param[i])
		{
			ShowData.nParam[PARAM_TYPE_GPS].Data.ParamDW.Param[i] = gSys.DevData.nParam[PARAM_TYPE_GPS].Data.ParamDW.Param[i];
			Str.Format(_T("%d"), ShowData.nParam[PARAM_TYPE_GPS].Data.ParamDW.Param[i]);
			mMyShowList.SetItemText(i, 2 + 2 * LIST_GPS_PARAM_COL, Str);
		}
	}

	for (i = 0; i < PARAM_MONITOR_MAX; i++)
	{
		if (gSys.DevData.nParam[PARAM_TYPE_MONITOR].Data.ParamDW.Param[i] != ShowData.nParam[PARAM_TYPE_MONITOR].Data.ParamDW.Param[i])
		{
			ShowData.nParam[PARAM_TYPE_MONITOR].Data.ParamDW.Param[i] = gSys.DevData.nParam[PARAM_TYPE_MONITOR].Data.ParamDW.Param[i];
			Str.Format(_T("%d"), ShowData.nParam[PARAM_TYPE_MONITOR].Data.ParamDW.Param[i]);
			mMyShowList.SetItemText(i, 2 + 2 * LIST_MONITOR_PARAM_COL, Str);
		}
	}

	for (i = 0; i < PARAM_ALARM1_MAX; i++)
	{
		if (gSys.DevData.nParam[PARAM_TYPE_ALARM1].Data.ParamDW.Param[i] != ShowData.nParam[PARAM_TYPE_ALARM1].Data.ParamDW.Param[i])
		{
			ShowData.nParam[PARAM_TYPE_ALARM1].Data.ParamDW.Param[i] = gSys.DevData.nParam[PARAM_TYPE_ALARM1].Data.ParamDW.Param[i];
			Str.Format(_T("%d"), ShowData.nParam[PARAM_TYPE_ALARM1].Data.ParamDW.Param[i]);
			mMyShowList.SetItemText(i, 2 + 2 * LIST_ALARM1_PARAM_COL, Str);
		}
	}

	for (i = 0; i < PARAM_ALARM2_MAX; i++)
	{
		if (gSys.DevData.nParam[PARAM_TYPE_ALARM2].Data.ParamDW.Param[i] != ShowData.nParam[PARAM_TYPE_ALARM2].Data.ParamDW.Param[i])
		{
			ShowData.nParam[PARAM_TYPE_ALARM2].Data.ParamDW.Param[i] = gSys.DevData.nParam[PARAM_TYPE_ALARM2].Data.ParamDW.Param[i];
			Str.Format(_T("%d"), ShowData.nParam[PARAM_TYPE_ALARM2].Data.ParamDW.Param[i]);
			mMyShowList.SetItemText(i, 2 + 2 * LIST_ALARM2_PARAM_COL, Str);
		}
	}

	if (memcmp(gSys.DevData.nParam[PARAM_TYPE_APN].Data.APN.APNName, ShowData.nParam[PARAM_TYPE_MAIN].Data.APN.APNName, sizeof(ShowData.nParam[PARAM_TYPE_MAIN].Data.APN.APNName)))
	{
		memcpy(ShowData.nParam[PARAM_TYPE_APN].Data.APN.APNName, gSys.DevData.nParam[PARAM_TYPE_APN].Data.APN.APNName, sizeof(ShowData.nParam[PARAM_TYPE_MAIN].Data.APN.APNName));
		MultiByteToWideChar(CP_ACP, 0, (char *)ShowData.nParam[PARAM_TYPE_APN].Data.APN.APNName, strlen(ShowData.nParam[PARAM_TYPE_APN].Data.APN.APNName) + 1, wbuf, strlen(ShowData.nParam[PARAM_TYPE_APN].Data.APN.APNName) + 1);
		Str.Format(_T("%s"), wbuf);
		mMyShowList.SetItemText(APN_NAME_RAW, 2 + 2 * LIST_OTHER_PARAM_COL, Str);
	}

	if (memcmp(gSys.DevData.nParam[PARAM_TYPE_APN].Data.APN.APNUser, ShowData.nParam[PARAM_TYPE_MAIN].Data.APN.APNUser, sizeof(ShowData.nParam[PARAM_TYPE_MAIN].Data.APN.APNName)))
	{
		memcpy(ShowData.nParam[PARAM_TYPE_APN].Data.APN.APNUser, gSys.DevData.nParam[PARAM_TYPE_APN].Data.APN.APNUser, sizeof(ShowData.nParam[PARAM_TYPE_MAIN].Data.APN.APNName));
		MultiByteToWideChar(CP_ACP, 0, (char *)ShowData.nParam[PARAM_TYPE_APN].Data.APN.APNUser, strlen(ShowData.nParam[PARAM_TYPE_APN].Data.APN.APNUser) + 1, wbuf, strlen(ShowData.nParam[PARAM_TYPE_APN].Data.APN.APNUser) + 1);
		Str.Format(_T("%s"), wbuf);
		mMyShowList.SetItemText(APN_USER_RAW, 2 + 2 * LIST_OTHER_PARAM_COL, Str);
	}

	if (memcmp(gSys.DevData.nParam[PARAM_TYPE_APN].Data.APN.APNPassword, ShowData.nParam[PARAM_TYPE_MAIN].Data.APN.APNPassword, sizeof(ShowData.nParam[PARAM_TYPE_MAIN].Data.APN.APNName)))
	{
		memcpy(ShowData.nParam[PARAM_TYPE_APN].Data.APN.APNPassword, gSys.DevData.nParam[PARAM_TYPE_APN].Data.APN.APNPassword, sizeof(ShowData.nParam[PARAM_TYPE_MAIN].Data.APN.APNName));
		MultiByteToWideChar(CP_ACP, 0, (char *)ShowData.nParam[PARAM_TYPE_APN].Data.APN.APNPassword, strlen(ShowData.nParam[PARAM_TYPE_APN].Data.APN.APNPassword) + 1, wbuf, strlen(ShowData.nParam[PARAM_TYPE_APN].Data.APN.APNPassword) + 1);
		Str.Format(_T("%s"), wbuf);
		mMyShowList.SetItemText(APN_PWD_RAW, 2 + 2 * LIST_OTHER_PARAM_COL, Str);
	}

	for (i = 0; i < 7; i++)
	{
		if (memcmp(gSys.DevData.nParam[PARAM_TYPE_NUMBER].Data.Number.Phone[i].Num, ShowData.nParam[PARAM_TYPE_NUMBER].Data.Number.Phone[i].Num, sizeof(ShowData.nParam[PARAM_TYPE_NUMBER].Data.Number.Phone[i].Num)))
		{
			memcpy(ShowData.nParam[PARAM_TYPE_NUMBER].Data.Number.Phone[i].Num, gSys.DevData.nParam[PARAM_TYPE_NUMBER].Data.Number.Phone[i].Num, sizeof(ShowData.nParam[PARAM_TYPE_NUMBER].Data.Number.Phone[i].Num));
			if (ShowData.nParam[PARAM_TYPE_NUMBER].Data.Number.Phone[i].Num[0] <= 12)
			{
				memcpy(PhoneBCD, ShowData.nParam[PARAM_TYPE_NUMBER].Data.Number.Phone[i].Num + 1, 6);
				PhoneBCD[5] >>= 4;
				Str.Format(_T("%02d%02d%02d%02d%02d%01d"), PhoneBCD[0], PhoneBCD[1], PhoneBCD[2], PhoneBCD[3], PhoneBCD[4], PhoneBCD[5]);
				mMyShowList.SetItemText(CAR_OWN0_RAW + i, 2 + 2 * LIST_OTHER_PARAM_COL, Str);
			}
			else
			{
				memcpy(PhoneBCD, ShowData.nParam[PARAM_TYPE_NUMBER].Data.Number.Phone[i].Num + 1, 7);
				PhoneBCD[6] >>= 4;
				Str.Format(_T("%02d%02d%02d%02d%02d%02d%01d"), PhoneBCD[0], PhoneBCD[1], PhoneBCD[2], PhoneBCD[3], PhoneBCD[4], PhoneBCD[5], PhoneBCD[6]);
				mMyShowList.SetItemText(CAR_OWN0_RAW + i, 2 + 2 * LIST_OTHER_PARAM_COL, Str);
			}
		}
	}

	return 0;
}

LRESULT CMyProjectDlg::OnUpdateVersionMessage(WPARAM wParam, LPARAM lParam)
{
	int i;
	CString Str;
	ShowData.MainVersion = gSys.DevData.MainVersion;
	Str.Format(_T("%04x"), ShowData.MainVersion);
	mMyShowList.SetItemText(MAIN_VERSION_RAW, 2 + 2 * LIST_OTHER_PARAM_COL, Str);
	return 0;
}

LRESULT CMyProjectDlg::OnUpdateDLMessage(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

void CMyProjectDlg::OnComCtrl()
{
	// TODO:  在此添加命令处理程序代码
	mComCtrl.UpdateProfile();
	mComCtrl.ShowWindow(SW_SHOW);
}

void CMyProjectDlg::Reset()
{
	int i, j;

	for (i = 0; i < 100; i++)
	{
		for (j = 0; j < LIST_COL_MAX; j++)
		{
			mMyShowList.SetItemText(i, j * 2 + 2, L"");
		}
		mMyShowList.SetItemText(i, 1 + 2 * LIST_GPS_COL, L"");
		mMyShowList.SetItemText(i, 1 + 2 * LIST_BD_COL, L"");
	}
	memset((u8 *)&ShowData, 0xff, sizeof(ShowData));
}

void CMyProjectDlg::OnUpgradeCtrl()
{
	// TODO:  在此添加命令处理程序代码
	mDevUpgrade.ShowWindow(SW_SHOW);
}

LRESULT CMyProjectDlg::OnComRxMessage(WPARAM wParam, LPARAM lParam)
{
	mComCtrl.OnComRxMessage();
	return 0; 
}

LRESULT CMyProjectDlg::OnComStateMessage(WPARAM wParam, LPARAM lParam)
{
	mComCtrl.OnComStateMessage();
	return 0;
}