// DevUpgrade.cpp : 实现文件
//

#include "stdafx.h"
#include "MyProject.h"
#include "DevUpgrade.h"
#include "afxdialogex.h"


// CDevUpgrade 对话框

IMPLEMENT_DYNAMIC(CDevUpgrade, CDialogEx)

CDevUpgrade::CDevUpgrade(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDevUpgrade::IDD, pParent)
{

}

CDevUpgrade::~CDevUpgrade()
{
	SaveParam();
}

void CDevUpgrade::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DL_BUTTON, mDLButton);
	DDX_Control(pDX, IDC_LOAD_BIN_BUTTON, mLoadBinButton);
	DDX_Control(pDX, IDC_LOAD_BIN_EDIT, mLoadBinEdit);
	DDX_Control(pDX, IDC_LOAD_LOD_BUTTON, mLoadLODButton);
	DDX_Control(pDX, IDC_LOAD_LOD_EDIT, mLoadLODEdit);
	DDX_Control(pDX, IDC_LOAD_UPDATE_BIN_BUTTON, mLoadUpgradeBinButton);
	DDX_Control(pDX, IDC_LOAD_UPDATE_BIN_EDIT, mLoadUpgradeBinEdit);
	DDX_Control(pDX, IDC_MAIN_VERSION_EDIT, mMainVersionEdit);
	DDX_Control(pDX, IDC_MAKE_BIN_BUTTON, mMakeBinButton);
	DDX_Control(pDX, IDC_MAKE_UPDATE_BIN_BUTTON, mMakeUpgradeBinButton);
	DDX_Control(pDX, IDC_PROC_PROGRESS, mProcProgress);
	DDX_Control(pDX, IDC_RESET_BUTTON, mResetButton);
	DDX_Control(pDX, IDC_UPDATE_VERSION_EDIT, mUpgradeVersionEdit);
}


BEGIN_MESSAGE_MAP(CDevUpgrade, CDialogEx)
	ON_BN_CLICKED(IDC_LOAD_LOD_BUTTON, &CDevUpgrade::OnBnClickedLoadLodButton)
	ON_BN_CLICKED(IDC_LOAD_BIN_BUTTON, &CDevUpgrade::OnBnClickedLoadBinButton)
	ON_BN_CLICKED(IDC_LOAD_UPDATE_BIN_BUTTON, &CDevUpgrade::OnBnClickedLoadUpgradeBinButton)
	ON_BN_CLICKED(IDC_MAKE_BIN_BUTTON, &CDevUpgrade::OnBnClickedMakeBinButton)
	ON_BN_CLICKED(IDC_MAKE_UPDATE_BIN_BUTTON, &CDevUpgrade::OnBnClickedMakeUpgradeBinButton)
	ON_BN_CLICKED(IDC_DL_BUTTON, &CDevUpgrade::OnBnClickedDlButton)
	ON_BN_CLICKED(IDC_RESET_BUTTON, &CDevUpgrade::OnBnClickedResetButton)
END_MESSAGE_MAP()


// CDevUpgrade 消息处理程序
void CDevUpgrade::LoadParam()
{
	ProfileRSString(PROFILE_NAME, L"SLIP PARAM", L"Lod File Dir", L"", LodFileDir.GetBuffer(FILE_PATH_LEN), FILE_PATH_LEN);
	LodFileDir.ReleaseBuffer(FILE_PATH_LEN);
	mLoadLODEdit.SetWindowTextW(LodFileDir);

	ProfileRSString(PROFILE_NAME, L"SLIP PARAM", L"Bin File Dir", L"", BinFileDir.GetBuffer(FILE_PATH_LEN), FILE_PATH_LEN);
	BinFileDir.ReleaseBuffer(FILE_PATH_LEN);
	mLoadBinEdit.SetWindowTextW(BinFileDir);

	ProfileRSString(PROFILE_NAME, L"SLIP PARAM", L"Upgrade File Dir", L"", UpgradeBinFileDir.GetBuffer(FILE_PATH_LEN), FILE_PATH_LEN);
	UpgradeBinFileDir.ReleaseBuffer(FILE_PATH_LEN);
	mLoadUpgradeBinEdit.SetWindowTextW(UpgradeBinFileDir);
}


void CDevUpgrade::SaveParam()
{
	ProfileWSString(PROFILE_NAME, L"SLIP PARAM", L"Lod File Dir", LodFileDir.GetBuffer(LodFileDir.GetLength()));
	LodFileDir.ReleaseBuffer(LodFileDir.GetLength());

	ProfileWSString(PROFILE_NAME, L"SLIP PARAM", L"Bin File Dir", BinFileDir.GetBuffer(BinFileDir.GetLength()));
	BinFileDir.ReleaseBuffer(BinFileDir.GetLength());

	ProfileWSString(PROFILE_NAME, L"SLIP PARAM", L"Upgrade File Dir", UpgradeBinFileDir.GetBuffer(UpgradeBinFileDir.GetLength()));
	UpgradeBinFileDir.ReleaseBuffer(UpgradeBinFileDir.GetLength());
}

void CDevUpgrade::CreateUpgradeFile()
{
	uint32_t SectionPos, i;
	CString Str;
	Str.Format(_T("gl_%08x_%d.bin"), gSys.UpgradeFileBuf.Head.MainVersion, gSys.UpgradeFileBuf.Head.AppVersion);
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
		File.Write((u8 *)&gSys.UpgradeFileBuf.Head, sizeof(File_HeadStruct));
		for (SectionPos = 0; SectionPos < gSys.UpgradeFileBuf.Head.BinFileLen; SectionPos++)
		{
			for (i = 0; i < 1024; i++)
			{
				File.Write((u8 *)&gSys.UpgradeFileBuf.SectionData[SectionPos].Data[i], 4);
			}
		}
		File.Close();
	}
}


bool CDevUpgrade::ReadUpgradeFile()
{
	uint32_t dwFileLen, i, j;
	CString Str;

	HANDLE hFile = CreateFile((LPCTSTR)UpgradeBinFileDir,
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

		File.Read(&gSys.UpgradeFileBuf.Head, sizeof(File_HeadStruct));
		gDBG.Trace("%s %d:%08x %d %d %d\r\n", __FUNCTION__, __LINE__, gSys.UpgradeFileBuf.Head.CRC32, gSys.UpgradeFileBuf.Head.BinFileLen, gSys.UpgradeFileBuf.Head.MainVersion,
			gSys.UpgradeFileBuf.Head.AppVersion);
		if (dwFileLen != (sizeof(File_HeadStruct) + (gSys.UpgradeFileBuf.Head.BinFileLen * 4096)))
		{
			gDBG.Trace("%s %d:%d %d\r\n", __FUNCTION__, __LINE__, dwFileLen, sizeof(File_HeadStruct)+(gSys.UpgradeFileBuf.Head.BinFileLen * 4096));
			AfxMessageBox(L"所选文件长度错误2");
			File.Close();
			return false;
		}
		for (i = 0; i < gSys.UpgradeFileBuf.Head.BinFileLen; i++)
		{
			for (j = 0; j < 1024; j++)
			{
				File.Read(&gSys.UpgradeFileBuf.SectionData[i].Data[j], 4);
			}
		}
		File.Close();
		return true;
	}
}

void CDevUpgrade::OnBnClickedLoadLodButton()
{
	// TODO:  在此添加控件通知处理程序代码
	if (OpenFilePath(LastFileDir, LodFileDir))
	{
		mLoadLODEdit.SetWindowTextW(LodFileDir);
		LastFileDir = LodFileDir;
	}
}


void CDevUpgrade::OnBnClickedLoadBinButton()
{
	// TODO:  在此添加控件通知处理程序代码
	if (OpenFilePath(LastFileDir, BinFileDir))
	{
		mLoadBinEdit.SetWindowTextW(BinFileDir);
		LastFileDir = BinFileDir;
	}
}


void CDevUpgrade::OnBnClickedLoadUpgradeBinButton()
{
	// TODO:  在此添加控件通知处理程序代码
	if (OpenFilePath(LastFileDir, UpgradeBinFileDir))
	{
		mLoadUpgradeBinEdit.SetWindowTextW(UpgradeBinFileDir);
		LastFileDir = UpgradeBinFileDir;
	}
}


void CDevUpgrade::OnBnClickedMakeBinButton()
{
	// TODO:  在此添加控件通知处理程序代码
	uint32_t dwReadLen, i, j;
	u8 FileData[128], FindEnd, FindStart;
	u32 dwTemp;
	u32 SectionPos, CachePos;
	GetHexFromEdit(&mMainVersionEdit, &gSys.UpgradeFileBuf.Head.MainVersion);
	GetIntFromEdit(&mUpgradeVersionEdit, &gSys.UpgradeFileBuf.Head.AppVersion);
	if (!gSys.UpgradeFileBuf.Head.MainVersion || !gSys.UpgradeFileBuf.Head.AppVersion)
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
		mProcProgress.SetRange(0, 3);
		mProcProgress.SetPos(0);
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
						mProcProgress.SetPos(2);
						break;
					}
				}
				if (FindEnd)
				{
					break;
				}
				FileData[8] = 0;
				dwTemp = strtoul((CONST CHAR *)FileData, NULL, 16);
				gSys.UpgradeFileBuf.SectionData[SectionPos].Data[CachePos] = dwTemp;
				CachePos++;

				if (CachePos >= 1024)
				{
					FindEnd = 1;
					for (j = 0; j < 1024; j++)
					{
						if (gSys.UpgradeFileBuf.SectionData[SectionPos].Data[j] != 0xffffffff)
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
						mProcProgress.SetPos(1);
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
		gSys.UpgradeFileBuf.Head.CRC32 = 0;
		if (SectionPos)
		{
			gSys.UpgradeFileBuf.Head.MaigcNum = RDA_UPGRADE_MAGIC_NUM;
			gSys.UpgradeFileBuf.Head.BinFileLen = SectionPos;
			gSys.UpgradeFileBuf.Head.CRC32 = CRC32_Cal(gSys.CRC32Table, (u8 *)&gSys.UpgradeFileBuf.SectionData[0].Data[0], gSys.UpgradeFileBuf.Head.BinFileLen * 4096, CRC32_START);
			gDBG.Trace("%s %d:%08x %d %08x %d\r\n", __FUNCTION__, __LINE__, gSys.UpgradeFileBuf.Head.CRC32, gSys.UpgradeFileBuf.Head.BinFileLen, gSys.UpgradeFileBuf.Head.MainVersion,
				gSys.UpgradeFileBuf.Head.AppVersion);
		}

		CreateUpgradeFile();
		mProcProgress.SetPos(3);
	}
}


void CDevUpgrade::OnBnClickedMakeUpgradeBinButton()
{
	// TODO:  在此添加控件通知处理程序代码
	u8 *OrgData;
	u32 OrgFileLen = 0;
	DWORD CopyLen;
	u32 DummyLen;
	File_HeadStruct Head;
	Head.MaigcNum = 0;
	Head.MainVersion = 0;
	GetHexFromEdit(&mMainVersionEdit, &Head.MainVersion);
	GetIntFromEdit(&mUpgradeVersionEdit, &Head.AppVersion);
	HANDLE hFile = CreateFile((LPCTSTR)BinFileDir,
		GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(L"所选文件打开失败");
		return;
	}
	else
	{
		CFile File(hFile);
		OrgFileLen = File.GetLength();
		OrgData = (u8 *)malloc(OrgFileLen);
		ReadFile(hFile, OrgData, OrgFileLen, &CopyLen, NULL);
		CloseHandle(hFile);
		Head.BinFileLen = OrgFileLen;
		Head.CRC32 = CRC32_Cal(gSys.CRC32Table, OrgData, OrgFileLen, CRC32_START);
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


void CDevUpgrade::OnBnClickedDlButton()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!ReadUpgradeFile())
	{
		return;
	}
	MyDeviceOperationReq(REQ_UPGRADE, 0, 0, NULL);

}


void CDevUpgrade::OnBnClickedResetButton()
{
	// TODO:  在此添加控件通知处理程序代码
	MyDeviceOperationReq(REQ_RESET, 0, 0, NULL);
}
