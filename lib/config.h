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
#define WM_COM_RX					(WM_MY_MESSAGE + 6)
#define WM_COM_STATE				(WM_MY_MESSAGE + 7)


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
