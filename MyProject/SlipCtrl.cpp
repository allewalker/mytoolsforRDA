// SlipCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "MyProject.h"
#include "SlipCtrl.h"
#include "afxdialogex.h"

static uint32_t CRC32Table[256];
GDTM_DataStruct gTxData;
Update_FileStruct UpdateFileBuf;
// CSlipCtrl 对话框

IMPLEMENT_DYNAMIC(CSlipCtrl, CDialogEx)

CSlipCtrl::CSlipCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSlipCtrl::IDD, pParent)
{

}

CSlipCtrl::~CSlipCtrl()
{
	SaveParam();
}

void CSlipCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIP_DL_BUTTON, mSlipDLButton);
	DDX_Control(pDX, IDC_SLIP_LOAD_BIN_BUTTON, mSlipLoadBinButton);
	DDX_Control(pDX, IDC_SLIP_LOAD_BIN_EDIT, mSlipLoadBinEdit);
	DDX_Control(pDX, IDC_SLIP_LOAD_LOD_BUTTON, mSlipLoadLodButton);
	DDX_Control(pDX, IDC_SLIP_LOAD_LOD_EDIT, mSlipLoadLodEdit);
	DDX_Control(pDX, IDC_SLIP_LOAD_UPDATE_BIN_BUTTON, mSlipLoadUpdateBinButton);
	DDX_Control(pDX, IDC_SLIP_LOAD_UPDATE_BIN_EDIT, mSlipLoadUpdateBinEdit);
	DDX_Control(pDX, IDC_SLIP_MAKE_BIN_BUTTON, mSlipMakeBinButton);
	DDX_Control(pDX, IDC_SLIP_MAKE_UPDATE_BIN_BUTTON, mSlipMakeUpdateBinButton);
	DDX_Control(pDX, IDC_SLIP_UPDATE_VERSION_EDIT, mSlipUpDateVersionEdit);
	DDX_Control(pDX, IDC_SLIP_MAIN_VERSION_EDIT, mSlipMainVersionEdit);
	DDX_Control(pDX, IDC_SLIP_PROC_PROGRESS, mSlipProcProgress);
	DDX_Control(pDX, IDC_SLIP_RESET_BUTTON, mSlipResetButton);
}


BEGIN_MESSAGE_MAP(CSlipCtrl, CDialogEx)
	ON_BN_CLICKED(IDC_SLIP_LOAD_LOD_BUTTON, &CSlipCtrl::OnBnClickedSlipLoadLodButton)
	ON_BN_CLICKED(IDC_SLIP_LOAD_BIN_BUTTON, &CSlipCtrl::OnBnClickedSlipLoadBinButton)
	ON_BN_CLICKED(IDC_SLIP_LOAD_UPDATE_BIN_BUTTON, &CSlipCtrl::OnBnClickedSlipLoadUpdateBinButton)
	ON_BN_CLICKED(IDC_SLIP_MAKE_BIN_BUTTON, &CSlipCtrl::OnBnClickedSlipMakeBinButton)
	ON_BN_CLICKED(IDC_SLIP_DL_BUTTON, &CSlipCtrl::OnBnClickedSlipDlButton)
	ON_BN_CLICKED(IDC_SLIP_RESET_BUTTON, &CSlipCtrl::OnBnClickedSlipResetButton)
	ON_BN_CLICKED(IDC_SLIP_MAKE_UPDATE_BIN_BUTTON, &CSlipCtrl::OnBnClickedSlipMakeUpdateBinButton)
END_MESSAGE_MAP()


// CSlipCtrl 消息处理程序


void CSlipCtrl::LoadParam()
{
	ProfileRSString(PROFILE_NAME, L"SLIP PARAM", L"Lod File Dir", L"", LodFileDir.GetBuffer(FILE_PATH_LEN), FILE_PATH_LEN);
	LodFileDir.ReleaseBuffer(FILE_PATH_LEN);
	mSlipLoadLodEdit.SetWindowTextW(LodFileDir);

	ProfileRSString(PROFILE_NAME, L"SLIP PARAM", L"Bin File Dir", L"", BinFileDir.GetBuffer(FILE_PATH_LEN), FILE_PATH_LEN);
	BinFileDir.ReleaseBuffer(FILE_PATH_LEN);
	mSlipLoadBinEdit.SetWindowTextW(BinFileDir);

	ProfileRSString(PROFILE_NAME, L"SLIP PARAM", L"Update File Dir", L"", UpdateBinFileDir.GetBuffer(FILE_PATH_LEN), FILE_PATH_LEN);
	UpdateBinFileDir.ReleaseBuffer(FILE_PATH_LEN);
	mSlipLoadUpdateBinEdit.SetWindowTextW(UpdateBinFileDir);
}


void CSlipCtrl::SaveParam()
{
	ProfileWSString(PROFILE_NAME, L"SLIP PARAM", L"Lod File Dir", LodFileDir.GetBuffer(LodFileDir.GetLength()));
	LodFileDir.ReleaseBuffer(LodFileDir.GetLength());

	ProfileWSString(PROFILE_NAME, L"SLIP PARAM", L"Bin File Dir", BinFileDir.GetBuffer(BinFileDir.GetLength()));
	BinFileDir.ReleaseBuffer(BinFileDir.GetLength());

	ProfileWSString(PROFILE_NAME, L"SLIP PARAM", L"Update File Dir", UpdateBinFileDir.GetBuffer(UpdateBinFileDir.GetLength()));
	UpdateBinFileDir.ReleaseBuffer(UpdateBinFileDir.GetLength());
}


BOOL CSlipCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	mSlipProcProgress.SetStep(1);
	mSlipProcProgress.SetBarColor(RGB(0, 255, 0));
	CRC32_CreateTable(CRC32Table, CRC32_GEN);
	LoadParam();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CSlipCtrl::OnBnClickedSlipLoadLodButton()
{
	// TODO:  在此添加控件通知处理程序代码
	if (OpenFilePath(LastFileDir, LodFileDir))
	{
		mSlipLoadLodEdit.SetWindowTextW(LodFileDir);
		LastFileDir = LodFileDir;
	}
}


void CSlipCtrl::OnBnClickedSlipLoadBinButton()
{
	// TODO:  在此添加控件通知处理程序代码
	if (OpenFilePath(LastFileDir, BinFileDir))
	{
		mSlipLoadBinEdit.SetWindowTextW(BinFileDir);
		LastFileDir = BinFileDir;
	}
}


void CSlipCtrl::OnBnClickedSlipLoadUpdateBinButton()
{
	// TODO:  在此添加控件通知处理程序代码
	if (OpenFilePath(LastFileDir, UpdateBinFileDir))
	{
		mSlipLoadUpdateBinEdit.SetWindowTextW(UpdateBinFileDir);
		LastFileDir = UpdateBinFileDir;
	}
}

void CSlipCtrl::OnBnClickedSlipMakeBinButton()
{
	// TODO:  在此添加控件通知处理程序代码
	uint32_t dwReadLen, i, j;
	u8 FileData[128], FindEnd, FindStart;
	u32 dwTemp;
	u32 SectionPos, CachePos;
	GetHexFromEdit(&mSlipMainVersionEdit, &UpdateFileBuf.Head.MainVersion);
	GetIntFromEdit(&mSlipUpDateVersionEdit, &UpdateFileBuf.Head.UpdateVersion);
	if (!UpdateFileBuf.Head.MainVersion || !UpdateFileBuf.Head.UpdateVersion)
	{
		AfxMessageBox(L"没有主版本号和升级版本号");
		return;
	}
	HANDLE hFile = CreateFile((LPCTSTR)LodFileDir,
		GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(L"所选文件打开失败");
		return;
	}
	else
		
	{
		//读取烧录文件并存到文件缓存中
		CFile File(hFile);
		dwReadLen = File.GetLength();
		i = 0;
		FindEnd = 0;
		FindStart = 0;
		SectionPos = 0;
		CachePos = 0;
		mSlipProcProgress.SetRange(0, 3);
		mSlipProcProgress.SetPos(0);
		while (i < dwReadLen)
		{
			if (FindStart)
			{
				File.Read(FileData, 9);
				i += 9;
				for (j = 0; j < 8; j++)
				{
					if (!IsHexDigit(FileData[j]))
					{
						gDBG.Trace("find end @ %d all %d\r\n", i, SectionPos * 1024 + CachePos);
						FindEnd = 1;
						mSlipProcProgress.SetPos(2);
						break;
					}
				}
				if (FindEnd)
				{
					break;
				}
				FileData[8] = 0;
				dwTemp = strtoul((CONST CHAR *)FileData, NULL, 16);
				UpdateFileBuf.SectionData[SectionPos].Data[CachePos] = dwTemp;
				CachePos++;
				
				if (CachePos >= 1024)
				{
					FindEnd = 1;
					for (j = 0; j < 1024; j++)
					{
						if (UpdateFileBuf.SectionData[SectionPos].Data[j] != 0xffffffff)
						{
							gDBG.Trace("%s %d:section %d has data\r\n", __FUNCTION__, __LINE__, SectionPos);
							SectionPos++;
							CachePos = 0;
							FindEnd = 0;
							break;
						}
					}
				}
				if (FindEnd)
				{
					break;
				}
			} 
			else
			{
				File.Read(FileData, 1);
				if (FileData[0] == '@')
				{
					i++;
					File.Read(FileData, 9);
					i += 9;
					if (!memcmp(FileData, "08360000\n", 9))
					{
						FindStart = 1;
						gDBG.Trace("find start @ %d\r\n", i);
						mSlipProcProgress.SetPos(1);
						continue;
					}

				}
				else
				{
					i++;
				}
			}

		}
		File.Close();
		UpdateFileBuf.Head.CRC32 = 0;
		if (SectionPos)
		{
			UpdateFileBuf.Head.MaigcNum = RDA_UPGRADE_MAGIC_NUM;
			UpdateFileBuf.Head.BinFileLen = SectionPos;
			UpdateFileBuf.Head.CRC32 = CRC32_Cal(CRC32Table, (u8 *)&UpdateFileBuf.SectionData[0].Data[0], UpdateFileBuf.Head.BinFileLen * 4096, CRC32_START);
			gDBG.Trace("%s %d:%08x %d %08x %d\r\n", __FUNCTION__, __LINE__, UpdateFileBuf.Head.CRC32, UpdateFileBuf.Head.BinFileLen, UpdateFileBuf.Head.MainVersion,
				UpdateFileBuf.Head.UpdateVersion);
		}
		
		CreateUpdateFile();
		mSlipProcProgress.SetPos(3);
	}
}


void CSlipCtrl::CreateUpdateFile()
{
	uint32_t SectionPos, i;
	CString Str;
	Str.Format(_T("gl_%08x_%d.bin"), UpdateFileBuf.Head.MainVersion, UpdateFileBuf.Head.UpdateVersion);
	HANDLE hFile = CreateFile((LPCTSTR)Str,
		GENERIC_WRITE, FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(L"所选文件打开失败");
		return;
	}
	else
	{
		//读取烧录文件并存到文件缓存中
		CFile File(hFile);
		File.Write((u8 *)&UpdateFileBuf.Head, sizeof(File_HeadStruct));
		for (SectionPos = 0; SectionPos < UpdateFileBuf.Head.BinFileLen; SectionPos++)
		{
			for (i = 0; i < 1024; i++)
			{
				File.Write((u8 *)&UpdateFileBuf.SectionData[SectionPos].Data[i], 4);
			}
		}
		File.Close();
	}
}


void CSlipCtrl::OnBnClickedSlipDlButton()
{
	SLIP_ReqStruct Req;
	// TODO:  在此添加控件通知处理程序代码
	if (!ReadUpdateFile())
	{
		return;
	}
	Req.File = &UpdateFileBuf;
	Req.Req = SLIP_REQ_DOWNLOAD;
	SLIP_Req(&Req);
}


bool CSlipCtrl::ReadUpdateFile()
{
	uint32_t dwFileLen, i,j;
	CString Str;
	
	HANDLE hFile = CreateFile((LPCTSTR)UpdateBinFileDir,
		GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(L"所选文件打开失败");
		return false;
	}
	else
	{
		CFile File(hFile);
		dwFileLen = File.GetLength();
		if (dwFileLen < sizeof(File_HeadStruct))
		{
			AfxMessageBox(L"所选文件长度错误1");
			File.Close();
			return false;
		}
		File.SeekToBegin();
// 		File.Read(&UpdateFileBuf.Head.CRC32, 4);
// 		File.Read(&UpdateFileBuf.Head.MainVersion, 4);
// 		File.Read(&UpdateFileBuf.Head.UpdateVersion, 4);
// 		File.Read(&UpdateFileBuf.Head.BinFileLen, 4);
		File.Read(&UpdateFileBuf.Head, sizeof(File_HeadStruct));
		gDBG.Trace("%s %d:%08x %d %d %d\r\n", __FUNCTION__, __LINE__, UpdateFileBuf.Head.CRC32, UpdateFileBuf.Head.BinFileLen, UpdateFileBuf.Head.MainVersion,
			UpdateFileBuf.Head.UpdateVersion);
		if (dwFileLen != (sizeof(File_HeadStruct) + (UpdateFileBuf.Head.BinFileLen * 4096)))
		{
			gDBG.Trace("%s %d:%d %d\r\n", __FUNCTION__, __LINE__, dwFileLen, sizeof(File_HeadStruct) + (UpdateFileBuf.Head.BinFileLen * 4096));
			AfxMessageBox(L"所选文件长度错误2");
			File.Close();
			return false;
		}
		for (i = 0; i < UpdateFileBuf.Head.BinFileLen; i++)
		{
			for (j = 0; j < 1024; j++)
			{
				File.Read(&UpdateFileBuf.SectionData[i].Data[j], 4);
			}
		}
		File.Close();
		return true;
	}
}


void CSlipCtrl::OnBnClickedSlipResetButton()
{
	// TODO:  在此添加控件通知处理程序代码
	SLIP_ReqStruct Req;
	Req.Req = SLIP_REQ_RESET;
	SLIP_Req(&Req);
}


void CSlipCtrl::OnBnClickedSlipMakeUpdateBinButton()
{
	// TODO:  在此添加控件通知处理程序代码
	u8 *OrgData;
	u32 OrgFileLen = 0;
	DWORD CopyLen;
	u32 DummyLen;
	File_HeadStruct Head;
	Head.MaigcNum = 0;
	Head.MainVersion = 0;
	GetHexFromEdit(&mSlipMainVersionEdit, &Head.MainVersion);
	GetIntFromEdit(&mSlipUpDateVersionEdit, &Head.UpdateVersion);
	HANDLE hFile = CreateFile((LPCTSTR)BinFileDir,
		GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(L"所选文件打开失败");
		return ;
	}
	else
	{
		CFile File(hFile);
		OrgFileLen = File.GetLength();
		OrgData = (u8 *)malloc(OrgFileLen);
		ReadFile(hFile, OrgData, OrgFileLen, &CopyLen, NULL);
		CloseHandle(hFile);
		Head.BinFileLen = OrgFileLen;
		Head.CRC32 = CRC32_Cal(CRC32Table, OrgData, OrgFileLen, CRC32_START);
	}
	CString Str;
	Str.Format(_T("ble_%08x.bin"), Head.MainVersion);

	hFile = CreateFile((LPCTSTR)Str,
		GENERIC_WRITE, FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(L"所选文件打开失败");
		free(OrgData);
		return;
	}
	else
	{
		//读取烧录文件并存到文件缓存中
		CFile File2(hFile);
		File2.Write((u8 *)&Head, sizeof(File_HeadStruct));
		CopyLen = 0;
		gDBG.Trace("%s %d:%08x %d %08x\r\n", __FUNCTION__, __LINE__, Head.CRC32, Head.BinFileLen, Head.MainVersion);
		while (CopyLen < Head.BinFileLen)
		{
			DummyLen = ((CopyLen + 64) >= Head.BinFileLen) ? 64 : (Head.BinFileLen - CopyLen);
			File2.Write(OrgData + CopyLen, DummyLen);
			CopyLen += DummyLen;
		}
		File2.Close();
	}
	free(OrgData);
}
