
#include "stdafx.h"
#include "global.h"

stMyCapInfo	gCapInfo;
HANDLE g_hSemaphore;
HANDLE g_hInfoSemaphore;

BOOL g_bPowerState = FALSE;
BOOL g_bAppExit = FALSE;
BOOL g_bUseMyDelayMs = TRUE;

BOOL g_bRootTestMod = FALSE;
int g_nIcType = IC_TYPE_2;

CWnd * g_pTestModeViewWnd = NULL;

int m_nLogLimitNumOfChar_0 = 0;

BOOL g_nSlaveAddress = 0;

//==============================================================================================
//IC Signature

u16 IC_SIGNATURE[IC_TYPE_MAX] =
{
	0xE628,		// ZT2628 / ZTW523 
	0xE532,		// ZT7532s / ZT7528 / ZTW522
	0xE538,		// ZT7538
	0xE548,		// ZT7548 / ZTL545
	0xE628,		// ZTM620
	0xE622,		// ZT2717 / ZTW622	/ GTW622 / FZ01
	0xE630,		// ZTM630
	0xE650,		// ZTL650
	0x650E,		// ZTL650M

	0xE623,		// GTW623
	0xE730,		// ZTM730
	0xE760,		// ZTL760
	0xE137,		// GTY137
	0xE722,		// ZTW722
};

BYTE I2C_SLAVE_ADDRESS[2] =
{
	I2C_SLAVE_ADDRESS_20,
	I2C_SLAVE_ADDRESS_40
};

T_FW_DOWNLOAD_INFO g_FwDownloadInfo = {0, };	

//==============================================================================================
// IUM
T_IUM_CMD_INFO ZT_GET_IUM_CMD_INFO(u16 nIcSignature)
{
	T_IUM_CMD_INFO tIumCmdInfo;

	switch (nIcSignature)
	{
	case 0xE548:// ZT7548 / ZTL545
	case 0xE538:// ZT7538
	case 0xE532:// ZT7532s / ZT7528 / ZTW522
	case 0xE628:// ZTM620 / ZTW523 / ZT2628
		tIumCmdInfo.nIumCmdCustomAddr = 0xF0A0;
		tIumCmdInfo.nIumCmdEnter = 0xF0F6;
		tIumCmdInfo.nIumCmdExit = 0xF0FA;
		tIumCmdInfo.nIumCmdSave = 0xF0F8;
		tIumCmdInfo.nIumAreaCount = 1;
		tIumCmdInfo.nIumCmdSelectPage = 0;
		break;

	case 0xE630:
	case 0xE730:
		tIumCmdInfo.nIumCmdCustomAddr = 0xF0A0;
		tIumCmdInfo.nIumCmdEnter = 0xFAF6;
		tIumCmdInfo.nIumCmdExit = 0xFAFA;
		tIumCmdInfo.nIumCmdSave = 0xFAF8;

		if (nIcSignature == 0xE730)
		{
			tIumCmdInfo.nIumAreaCount = 3;
			tIumCmdInfo.nIumCmdSelectPage = 0xFAFC;
		}
		else if (nIcSignature == 0xE630)
		{
			tIumCmdInfo.nIumAreaCount = 1;
			tIumCmdInfo.nIumCmdSelectPage = 0;
		}
		break;

	default:
		tIumCmdInfo.nIumCmdCustomAddr = 0xB000;
		tIumCmdInfo.nIumCmdEnter = 0xF0F6;
		tIumCmdInfo.nIumCmdExit = 0xF0FA;
		tIumCmdInfo.nIumCmdSave = 0xF0F8;
		tIumCmdInfo.nIumAreaCount = 1;
		tIumCmdInfo.nIumCmdSelectPage = 0;
		break;
	}

	return tIumCmdInfo;
}

//==============================================================================================
// Vendor Command

u16 VCMD_IC_GROUP_1[VCMD_MAX + 1] =
{
	// System Control

	0xC000,		// VCMD_ENABLE
	0xC001,		// VCMD_NVM_VALID
	0xC002,		// VCMD_NVM_INIT
	0xC003,		// VCMD_NVM_WRITE_ENABLE
	0xC004,		// VCMD_INTN_CLEAR

	0xCC00,		// VCMD_ID
	0xCC01,		// VCMD_REG_READ
	0xCC02,		// VCMD_REG_WRITE

	// Firmware Upgrade

	0x01D0,		// VCMD_FW_UPGRADE_INIT
	0x01D1,		// VCMD_FW_UPGRADE_PGM
	0x01D2,		// VCMD_FW_UPGRADE_READ
	0x01D3,		// VCMD_FW_UPGRADE_PACKET_SIZE
	0x01D4,		// VCMD_FW_UPGRADE_REPEAT
	0x01D5,		// VCMD_FW_UPGRADE_MODE
	0x01DD,		// VCMD_FW_UPGRADE_FLUSH
	0x01DE,		// VCMD_FW_UPGRADE_WRITE_MODE
	0x01DF,		// VCMD_FW_UPGRADE_MASS_ERASE
	0x01DA,		// VCMD_FW_UPGRADE_START_PAGE

	0x0000,		// VCMD_FW_UPGRADE_NVM_CE_SELECT
	0x0000,		// VCMD_FW_UPGRADE_NVM_TMODE_CHANGE

	0x09D3,		// VCMD_IUM_WRITE_PREV
	0x09D1,		// VCMD_IUM_WRITE_DATA
	0x09D2,		// VCMD_IUM_READ_DATA
	0x09DF,		// VCMD_IUM_ERASE

	VCMD_INVALID_VALUE,		// VCMD_OSC_FREQ_SEL

	VCMD_INVALID_VALUE		// VCMD_MAX
};

u16 VCMD_IC_GROUP_2[VCMD_MAX + 1] =
{
	// System Control

	0x10F0,		// VCMD_ENABLE
	0x11F0,		// VCMD_NVM_VALID
	0x12F0,		// VCMD_NVM_INIT
	0x13F0,		// VCMD_NVM_WRITE_ENABLE
	0x14F0,		// VCMD_INTN_CLEAR

	0x17F0,		// VCMD_ID
	0x18F0,		// VCMD_REG_READ
	0x19F0,		// VCMD_REG_WRITE

	// Firmware Upgrade

	0x20F0,		// VCMD_FW_UPGRADE_INIT
	0x21F0,		// VCMD_FW_UPGRADE_PGM
	0x22F0,		// VCMD_FW_UPGRADE_READ
	0x23F0,		// VCMD_FW_UPGRADE_PACKET_SIZE
	0x24F0,		// VCMD_FW_UPGRADE_REPEAT
	0x25F0,		// VCMD_FW_UPGRADE_MODE
	0x26F0,		// VCMD_FW_UPGRADE_FLUSH
	0x27F0,		// VCMD_FW_UPGRADE_WRITE_MODE
	0x28F0,		// VCMD_FW_UPGRADE_MASS_ERASE
	0x29F0,		// VCMD_FW_UPGRADE_START_PAGE

	0x54F0,		// VCMD_FW_UPGRADE_NVM_CE_SELECT
	0x19F0,		// VCMD_FW_UPGRADE_NVM_TMODE_CHANGE

	0x30F0,		// VCMD_IUM_WRITE_PREV
	0x31F0,		// VCMD_IUM_WRITE_DATA
	0x32F0,		// VCMD_IUM_READ_DATA
	0x33F0,		// VCMD_IUM_ERASE

	VCMD_INVALID_VALUE,		// VCMD_OSC_FREQ_SEL

	VCMD_INVALID_VALUE		// VCMD_MAX
};


u16 ZT_VCMD(u16 index)
{
	if( index >= VCMD_MAX )
		return 0x0000;

	u16 ic_signature = IC_SIGNATURE[g_nIcType];

	if(index == VCMD_OSC_FREQ_SEL)
	{
		if(ic_signature == 0xE532 || ic_signature == 0xE538 || ic_signature == 0xE548)
			return 0xC201;
		else if(IC_GROUP_2(IC_SIGNATURE[g_nIcType]))
			return 0x1AF0;
		else
			return 0xC011;
	}
	else if(ic_signature == 0xE650 && index >= VCMD_IUM_WRITE_PREV && index <= VCMD_IUM_ERASE)	// ZTL650
	{
		if(index == VCMD_IUM_READ_DATA)
		{
			T_IUM_CMD_INFO cmd_info = ZT_GET_IUM_CMD_INFO(ic_signature);

			return cmd_info.nIumCmdCustomAddr;
		}
		else
			return VCMD_INVALID_VALUE;
	}
	else
	{
		if( IC_GROUP_2(IC_SIGNATURE[g_nIcType]) )
		{
			return VCMD_IC_GROUP_2[index];
		}
		else
		{
			return VCMD_IC_GROUP_1[index];
		}
	}
}

CStringArray	g_strModuleIdList;
CUIntArray		g_nModuleIdList;

CString m_strIumDir;
double m_vih_ref;
double g_vcca_ref;
double g_voltage;

