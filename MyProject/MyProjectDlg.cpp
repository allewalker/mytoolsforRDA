
// MyProjectDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyProject.h"
#include "MyProjectDlg.h"
#include "afxdialogex.h"
#include "AES.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CMyProjectDlg �Ի���
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


// CMyProjectDlg ��Ϣ�������

BOOL CMyProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CString Str;

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	mMyShowEdit.SetLimitText(UINT32_MAX);
	gDBG.SetWnd(m_hWnd);
	MyDeviceSetWnd(m_hWnd);
	mComCtrl.Create(IDD_COM_CTRL_DIALOG, this);
	mComCtrl.ShowWindow(SW_HIDE);
	mDevUpgrade.Create(IDD_DEV_UPGRADE_DIALOG, this);
	mDevUpgrade.ShowWindow(SW_HIDE);
	//��ʼ�����
	DWORD dwStyle = mMyShowList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;// ѡ��ĳ��ʹ���и�����ֻ������report ����listctrl ��
	dwStyle |= LVS_EX_GRIDLINES;// �����ߣ�ֻ������report ����listctrl ��
	mMyShowList.SetExtendedStyle(dwStyle); // ������չ��� 
	mMyShowList.InsertColumn(0, L"���", LVCFMT_CENTER, 40);// ������
	mMyShowList.InsertColumn(1 + 2 * LIST_VAR_COL, L"����", LVCFMT_CENTER, 80);// ������
	mMyShowList.InsertColumn(2 + 2 * LIST_VAR_COL, L"", LVCFMT_CENTER, 80);// ������
	mMyShowList.InsertColumn(1 + 2 * LIST_GPS_INFO_COL, L"GPS��Ϣ", LVCFMT_CENTER, 60);// ������
	mMyShowList.InsertColumn(2 + 2 * LIST_GPS_INFO_COL, L"", LVCFMT_CENTER, 80);// ������
	mMyShowList.InsertColumn(1 + 2 * LIST_MAIN_PARAM_COL, L"��Ҫ����", LVCFMT_CENTER, 90);// ������
	mMyShowList.InsertColumn(2 + 2 * LIST_MAIN_PARAM_COL, L"", LVCFMT_CENTER, 80);// ������
	mMyShowList.InsertColumn(1 + 2 * LIST_SYS_PARAM_COL, L"ϵͳ����", LVCFMT_CENTER, 90);// ������
	mMyShowList.InsertColumn(2 + 2 * LIST_SYS_PARAM_COL, L"", LVCFMT_CENTER, 40);// ������
	mMyShowList.InsertColumn(1 + 2 * LIST_GPS_PARAM_COL, L"GPS����", LVCFMT_CENTER, 90);// ������
	mMyShowList.InsertColumn(2 + 2 * LIST_GPS_PARAM_COL, L"", LVCFMT_CENTER, 40);// ������
	mMyShowList.InsertColumn(1 + 2 * LIST_MONITOR_PARAM_COL, L"��ز���", LVCFMT_CENTER, 90);// ������
	mMyShowList.InsertColumn(2 + 2 * LIST_MONITOR_PARAM_COL, L"", LVCFMT_CENTER, 40);// ������
	mMyShowList.InsertColumn(1 + 2 * LIST_ALARM1_PARAM_COL, L"��������1", LVCFMT_CENTER, 90);// ������
	mMyShowList.InsertColumn(2 + 2 * LIST_ALARM1_PARAM_COL, L"", LVCFMT_CENTER, 40);// ������
	mMyShowList.InsertColumn(1 + 2 * LIST_ALARM2_PARAM_COL, L"��������2", LVCFMT_CENTER, 90);// ������
	mMyShowList.InsertColumn(2 + 2 * LIST_ALARM2_PARAM_COL, L"", LVCFMT_CENTER, 40);// ������
	mMyShowList.InsertColumn(1 + 2 * LIST_OTHER_PARAM_COL, L"��������", LVCFMT_CENTER, 90);// ������
	mMyShowList.InsertColumn(2 + 2 * LIST_OTHER_PARAM_COL, L"", LVCFMT_CENTER, 200);// ������
	mMyShowList.InsertColumn(1 + 2 * LIST_GPS_COL, L"GPS����", LVCFMT_CENTER, 60);// ������
	mMyShowList.InsertColumn(2 + 2 * LIST_GPS_COL, L"", LVCFMT_CENTER, 40);// ������
	mMyShowList.InsertColumn(1 + 2 * LIST_BD_COL, L"BD����", LVCFMT_CENTER, 60);// ������
	mMyShowList.InsertColumn(2 + 2 * LIST_BD_COL, L"", LVCFMT_CENTER, 40);// ������
	for (int i = 0; i < 100; i++)
	{
		Str.Format(_T("%d"), i);
		mMyShowList.InsertItem(i, Str);
	}
	mMyShowList.SetItemText(SYS_TIME, 1 + 2 * LIST_VAR_COL, L"����ʱ��");
	mMyShowList.SetItemText(VBAT, 1 + 2 * LIST_VAR_COL, L"��ѹ");
	mMyShowList.SetItemText(UTC_DATE, 1 + 2 * LIST_VAR_COL, L"UTC����");
	mMyShowList.SetItemText(UTC_TIME, 1 + 2 * LIST_VAR_COL, L"UTCʱ��");
	mMyShowList.SetItemText(IO_VAL, 1 + 2 * LIST_VAR_COL, L"VACC");
	mMyShowList.SetItemText(GSENSOR_VAL, 1 + 2 * LIST_VAR_COL, L"ʵʱGS");
	mMyShowList.SetItemText(GSENSOR_ALARM_VAL, 1 + 2 * LIST_VAR_COL, L"����GS");
	mMyShowList.SetItemText(GSENSOR_MONITOR_VAL, 1 + 2 * LIST_VAR_COL, L"���GS");
	mMyShowList.SetItemText(GSENSOR_KEEP_VAL, 1 + 2 * LIST_VAR_COL, L"����GS");
	mMyShowList.SetItemText(CELL_ID, 1 + 2 * LIST_VAR_COL, L"��ǰ��վ");
	mMyShowList.SetItemText(SHUTDOWN_TIME, 1 + 2 * LIST_VAR_COL, L"�ػ�ʱ��");
	mMyShowList.SetItemText(SOFTWARE_VERSION, 1 + 2 * LIST_VAR_COL, L"����汾");
	mMyShowList.SetItemText(MAIN_FREQ, 1 + 2 * LIST_VAR_COL, L"��Ƶ");
	mMyShowList.SetItemText(TRACE_TO, 1 + 2 * LIST_VAR_COL, L"TRACE TO");

	mMyShowList.SetItemText(SYSTEM_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"ϵͳ״̬");
	mMyShowList.SetItemText(SIM_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"SIM��״̬");
	mMyShowList.SetItemText(SMS_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"SMS״̬");
	mMyShowList.SetItemText(REG_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"ע��״̬");
	mMyShowList.SetItemText(GPRS_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"GPRS״̬");
	mMyShowList.SetItemText(GPRS_ATTACH_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"GPRS����");
	mMyShowList.SetItemText(GPRS_ACT_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"GPRS����");
	mMyShowList.SetItemText(MONITOR_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"�������");
	mMyShowList.SetItemText(RSSI_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"�ź�");
	mMyShowList.SetItemText(GPS_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"GPS״̬");
	mMyShowList.SetItemText(ALARM_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"����״̬");
	mMyShowList.SetItemText(CRASH_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"��ײ");
	mMyShowList.SetItemText(MOVE_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"�ƶ�");
	mMyShowList.SetItemText(CUTLINE_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"����");
	mMyShowList.SetItemText(OVERSPEED_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"����");
	mMyShowList.SetItemText(PRINT_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"��ӡģʽ");
	mMyShowList.SetItemText(FIRST_LOCAT_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"�״ζ�λ");
	mMyShowList.SetItemText(TRACE_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"TRACE״̬");
	mMyShowList.SetItemText(WDG_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"WDG VAL");
	mMyShowList.SetItemText(LED_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"�̵�");
	mMyShowList.SetItemText(LED_STATE + 1 + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"���");
	mMyShowList.SetItemText(REBOOT_STATE + VAR_MAX, 1 + 2 * LIST_VAR_COL, L"����");


	mMyShowList.SetItemText(SIM_ERROR + VAR_MAX + STATE_MAX, 1 + 2 * LIST_VAR_COL, L"SIM������");
	mMyShowList.SetItemText(GPRS_ERROR + VAR_MAX + STATE_MAX, 1 + 2 * LIST_VAR_COL, L"GPRS����");
	mMyShowList.SetItemText(GPS_ERROR + VAR_MAX + STATE_MAX, 1 + 2 * LIST_VAR_COL, L"GPS����");
	mMyShowList.SetItemText(ANT_SHORT_ERROR + VAR_MAX + STATE_MAX, 1 + 2 * LIST_VAR_COL, L"���߶�·");
	mMyShowList.SetItemText(ANT_BREAK_ERROR + VAR_MAX + STATE_MAX, 1 + 2 * LIST_VAR_COL, L"���߶�·");
	mMyShowList.SetItemText(SENSOR_ERROR + VAR_MAX + STATE_MAX, 1 + 2 * LIST_VAR_COL, L"GS����");
	mMyShowList.SetItemText(LOW_POWER_ERROR + VAR_MAX + STATE_MAX, 1 + 2 * LIST_VAR_COL, L"�͵����");

	mMyShowList.SetItemText(GPS_INFO_UTC_DATE_RAW, 1 + 2 * LIST_GPS_INFO_COL, L"GPS����");
	mMyShowList.SetItemText(GPS_INFO_UTC_TIME_RAW, 1 + 2 * LIST_GPS_INFO_COL, L"GPSʱ��");
	mMyShowList.SetItemText(GPS_INFO_LOCAT_RAW, 1 + 2 * LIST_GPS_INFO_COL, L"��λ");
	mMyShowList.SetItemText(GPS_INFO_LAT_NS_RAW, 1 + 2 * LIST_GPS_INFO_COL, L"NS");
	mMyShowList.SetItemText(GPS_INFO_LAT_RAW, 1 + 2 * LIST_GPS_INFO_COL, L"γ��");
	mMyShowList.SetItemText(GPS_INFO_LGT_EW_RAW, 1 + 2 * LIST_GPS_INFO_COL, L"EW");
	mMyShowList.SetItemText(GPS_INFO_LGT_RAW, 1 + 2 * LIST_GPS_INFO_COL, L"����");
	mMyShowList.SetItemText(GPS_INFO_SPEED_RAW, 1 + 2 * LIST_GPS_INFO_COL, L"�ٶȽ�");


	mMyShowList.SetItemText(MAIN_PARAM_UID0_RAW, 1 + 2 * LIST_MAIN_PARAM_COL, L"UID0");
	mMyShowList.SetItemText(MAIN_PARAM_UID1_RAW, 1 + 2 * LIST_MAIN_PARAM_COL, L"UID1");
	mMyShowList.SetItemText(MAIN_PARAM_UID2_RAW, 1 + 2 * LIST_MAIN_PARAM_COL, L"UID2");
	mMyShowList.SetItemText(MAIN_PARAM_IP_RAW, 1 + 2 * LIST_MAIN_PARAM_COL, L"��IP");
	mMyShowList.SetItemText(MAIN_PARAM_URL_RAW, 1 + 2 * LIST_MAIN_PARAM_COL, L"������");
	mMyShowList.SetItemText(MAIN_PARAM_TCP_PORT_RAW, 1 + 2 * LIST_MAIN_PARAM_COL, L"TCP�˿�");
	mMyShowList.SetItemText(MAIN_PARAM_UDP_PORT_RAW, 1 + 2 * LIST_MAIN_PARAM_COL, L"UDP�˿�");
	mMyShowList.SetItemText(MAIN_PARAM_USER_CODE_RAW, 1 + 2 * LIST_MAIN_PARAM_COL, L"�û�����");

	mMyShowList.SetItemText(PARAM_DETECT_PERIOD, 1 + 2 * LIST_SYS_PARAM_COL, L"�������");
	mMyShowList.SetItemText(PARAM_SENSOR_EN, 1 + 2 * LIST_SYS_PARAM_COL, L"���ʹ��");
	mMyShowList.SetItemText(PARAM_COM_BR, 1 + 2 * LIST_SYS_PARAM_COL, L"ͨѶBR");
	mMyShowList.SetItemText(PARAM_GPS_BR, 1 + 2 * LIST_SYS_PARAM_COL, L"GPSBR");
	mMyShowList.SetItemText(PARAM_STOP_VBAT, 1 + 2 * LIST_SYS_PARAM_COL, L"ֹͣ��ѹ");
	mMyShowList.SetItemText(PARAM_LOW_VBAT, 1 + 2 * LIST_SYS_PARAM_COL, L"�͵��ѹ");
	mMyShowList.SetItemText(PARAM_NORMAL_VBAT, 1 + 2 * LIST_SYS_PARAM_COL, L"������ѹ");
	mMyShowList.SetItemText(PARAM_SMS_ALARM, 1 + 2 * LIST_SYS_PARAM_COL, L"���ű���");
	mMyShowList.SetItemText(PARAM_CALL_AUTO_GET, 1 + 2 * LIST_SYS_PARAM_COL, L"�Զ�����");
	mMyShowList.SetItemText(PARAM_SIM_TO, 1 + 2 * LIST_SYS_PARAM_COL, L"SIM����ʱ");
	mMyShowList.SetItemText(PARAM_GPRS_TO, 1 + 2 * LIST_SYS_PARAM_COL, L"GPRS��ʱ");

	mMyShowList.SetItemText(PARAM_GS_WAKEUP_GPS, 1 + 2 * LIST_GPS_PARAM_COL, L"�𶯻���");
	mMyShowList.SetItemText(PARAM_VACC_WAKEUP_GPS, 1 + 2 * LIST_GPS_PARAM_COL, L"ACC����");
	mMyShowList.SetItemText(PARAM_GPS_NODATA_TO, 1 + 2 * LIST_GPS_PARAM_COL, L"������ʱ��");
	mMyShowList.SetItemText(PARAM_GPS_V_TO, 1 + 2 * LIST_GPS_PARAM_COL, L"����λʱ��");
	mMyShowList.SetItemText(PARAM_GPS_KEEP_TO, 1 + 2 * LIST_GPS_PARAM_COL, L"���ѱ���ʱ��");
	mMyShowList.SetItemText(PARAM_GPS_SLEEP_TO, 1 + 2 * LIST_GPS_PARAM_COL, L"��������");
	mMyShowList.SetItemText(PARAM_AGPS_EN, 1 + 2 * LIST_GPS_PARAM_COL, L"����AGPS");
	mMyShowList.SetItemText(PARAM_GPS_ONLY_ONCE, 1 + 2 * LIST_GPS_PARAM_COL, L"ֻ��λһ��");

	mMyShowList.SetItemText(PARAM_GS_WAKEUP_MONITOR, 1 + 2 * LIST_MONITOR_PARAM_COL, L"GS����");
	mMyShowList.SetItemText(PARAM_GS_JUDGE_RUN, 1 + 2 * LIST_MONITOR_PARAM_COL, L"GS�ж��г�");
	mMyShowList.SetItemText(PARAM_UPLOAD_RUN_PERIOD, 1 + 2 * LIST_MONITOR_PARAM_COL, L"�г��ش�");
	mMyShowList.SetItemText(PARAM_UPLOAD_STOP_PERIOD, 1 + 2 * LIST_MONITOR_PARAM_COL, L"ͣ���ش�");
	mMyShowList.SetItemText(PARAM_UPLOAD_HEART_PERIOD, 1 + 2 * LIST_MONITOR_PARAM_COL, L"�������");
	mMyShowList.SetItemText(PARAM_MONITOR_NET_TO, 1 + 2 * LIST_MONITOR_PARAM_COL, L"TCP���ͳ�ʱ");
	mMyShowList.SetItemText(PARAM_MONITOR_KEEP_TO, 1 + 2 * LIST_MONITOR_PARAM_COL, L"����ʱ��");
	mMyShowList.SetItemText(PARAM_MONITOR_SLEEP_TO, 1 + 2 * LIST_MONITOR_PARAM_COL, L"˯��ʱ��");
	mMyShowList.SetItemText(PARAM_MONITOR_RECONNECT_MAX, 1 + 2 * LIST_MONITOR_PARAM_COL, L"��������");
	mMyShowList.SetItemText(PARAM_MONITOR_ADD_MILEAGE, 1 + 2 * LIST_MONITOR_PARAM_COL, L"�������");
	mMyShowList.SetItemText(PARAM_MONITOR_ACC_UPLOAD, 1 + 2 * LIST_MONITOR_PARAM_COL, L"ACC�仯�ϴ�");

	mMyShowList.SetItemText(PARAM_VACC_CTRL_ALARM, 1 + 2 * LIST_ALARM1_PARAM_COL, L"ACC����");
	mMyShowList.SetItemText(PARAM_ALARM_ON_DELAY, 1 + 2 * LIST_ALARM1_PARAM_COL, L"��������ӳ�");
	mMyShowList.SetItemText(PARAM_CRASH_GS, 1 + 2 * LIST_ALARM1_PARAM_COL, L"�𶯷�ֵ");
	mMyShowList.SetItemText(PARAM_CRASH_JUDGE_TO, 1 + 2 * LIST_ALARM1_PARAM_COL, L"�𶯱���ʱ��");
	mMyShowList.SetItemText(PARAM_CRASH_JUDGE_CNT, 1 + 2 * LIST_ALARM1_PARAM_COL, L"�𶯴�����ֵ");

	mMyShowList.SetItemText(PARAM_CRASH_ALARM_WAIT_TO, 1 + 2 * LIST_ALARM1_PARAM_COL, L"�𶯱����ӳ�");
	mMyShowList.SetItemText(PARAM_CRASH_ALARM_FLUSH_TO, 1 + 2 * LIST_ALARM1_PARAM_COL, L"�𶯱������");
	mMyShowList.SetItemText(PARAM_CRASH_ALARM_REPEAT, 1 + 2 * LIST_ALARM1_PARAM_COL, L"����������");
	mMyShowList.SetItemText(PARAM_CRASH_WAKEUP_MOVE, 1 + 2 * LIST_ALARM1_PARAM_COL, L"��ʹ���ƶ�");
	mMyShowList.SetItemText(PARAM_MOVE_RANGE, 1 + 2 * LIST_ALARM1_PARAM_COL, L"�ƶ������뾶");

	mMyShowList.SetItemText(PARAM_MOVE_JUDGE_TO, 1 + 2 * LIST_ALARM1_PARAM_COL, L"�ƶ����ʱ��");
	mMyShowList.SetItemText(PARAM_MOVE_ALARM_FLUSH_TO, 1 + 2 * LIST_ALARM1_PARAM_COL, L"�ƶ��������");
	mMyShowList.SetItemText(PARAM_MOVE_ALARM_REPEAT, 1 + 2 * LIST_ALARM1_PARAM_COL, L"�ƶ���������");

	mMyShowList.SetItemText(PARAM_VACC_WAKEUP_CUTLINE_TO, 1 + 2 * LIST_ALARM2_PARAM_COL, L"ACC���ӳ�");
	mMyShowList.SetItemText(PARAM_CUTLINE_ALARM_FLUSH_TO, 1 + 2 * LIST_ALARM2_PARAM_COL, L"����ˢ��ʱ��");
	mMyShowList.SetItemText(PARAM_CUTLINE_ALARM_DELAY, 1 + 2 * LIST_ALARM2_PARAM_COL, L"���߱����ӳ�");
	mMyShowList.SetItemText(PARAM_OVERSPEED_ALARM_VAL, 1 + 2 * LIST_ALARM2_PARAM_COL, L"���ٱ�����ֵ");
	mMyShowList.SetItemText(PARAM_OVERSPEED_ALARM_DELAY, 1 + 2 * LIST_ALARM2_PARAM_COL, L"���ٱ����ӳ�");
	mMyShowList.SetItemText(PARAM_ALARM_ENABLE, 1 + 2 * LIST_ALARM2_PARAM_COL, L"����ʹ��");

	mMyShowList.SetItemText(MAIN_VERSION_RAW, 1 + 2 * LIST_OTHER_PARAM_COL, L"���İ汾");

	mMyShowList.SetItemText(APN_NAME_RAW, 1 + 2 * LIST_OTHER_PARAM_COL, L"APN����");
	mMyShowList.SetItemText(APN_USER_RAW, 1 + 2 * LIST_OTHER_PARAM_COL, L"APN�û���");
	mMyShowList.SetItemText(APN_PWD_RAW, 1 + 2 * LIST_OTHER_PARAM_COL, L"APN����");

	mMyShowList.SetItemText(IMEI_RAW, 1 + 2 * LIST_OTHER_PARAM_COL, L"IMEI");
	mMyShowList.SetItemText(IMSI_RAW, 1 + 2 * LIST_OTHER_PARAM_COL, L"IMSI");
	mMyShowList.SetItemText(ICCID_RAW, 1 + 2 * LIST_OTHER_PARAM_COL, L"ICCID");

	mMyShowList.SetItemText(CAR_OWN0_RAW, 1 + 2 * LIST_OTHER_PARAM_COL, L"��������1");
	mMyShowList.SetItemText(CAR_OWN0_RAW + 1, 1 + 2 * LIST_OTHER_PARAM_COL, L"��������2");
	mMyShowList.SetItemText(CAR_OWN0_RAW + 2, 1 + 2 * LIST_OTHER_PARAM_COL, L"��������3");
	mMyShowList.SetItemText(CAR_OWN0_RAW + 3, 1 + 2 * LIST_OTHER_PARAM_COL, L"��������4");
	mMyShowList.SetItemText(CAR_OWN0_RAW + 4, 1 + 2 * LIST_OTHER_PARAM_COL, L"��������5");
	mMyShowList.SetItemText(CAR_OWN0_RAW + 5, 1 + 2 * LIST_OTHER_PARAM_COL, L"��������6");
	mMyShowList.SetItemText(CAR_OWN0_RAW + 6, 1 + 2 * LIST_OTHER_PARAM_COL, L"��������7");
	
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
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMyProjectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMyProjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//���յ�������Ϣ
void CMyProjectDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (MessageBox(L"��ȷ��Ҫ�˳�������", L"��ʾ", MB_ICONINFORMATION|MB_YESNO) == IDNO)
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	// TODO:  �ڴ���������������
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
	// TODO:  �ڴ���������������
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