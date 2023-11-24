#ifndef __GLOBAL_H_
#define __GLOBAL_H_

#include "register.h"

#define MAX_DEVICE_COUNT					8				// Don't Modify

//---------------------------------------------------------------------------------
// Configuring Build Options

#define		D_USE_CHECK_BUTTON_TEST

//#define 	D_USE_CHECK_MPTOOL_REVISION						// Don't Modify
//#define	D_PRODUCT_SOLUTION_ALLROUNDER					// Don't Modify
//#define	D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT

#define		D_USE_FW_CHINA_FRAMEWORK

#define		D_DISPLAY_NOT_USE_TEST_TO_RESERVED

// [CAUTION] Need to change hardware and firmware of MPTool
#define		D_USE_GET_VOLTAGE_ADC

//---------------------------------------------------------------------------------
// Debugging Test

#ifdef _DEBUG
//#define		D_MEASURE_ELAPSED_TIME

#define			D_DEBUG_IUM_ACCESS
#endif

//#define		D_DEBUG_FW_UPGRADE_TEST				// Aging Test

//#define		HV_USE_FILE_DEBUG
#define			IUM_SAVE_FILE
//---------------------------------------------------------------------------------

#define STRING_INVALID_ASIC_ID		"INVALID_ASIC_ID"

//---------------------------------------------------------------------------------
typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;
typedef	unsigned int	u32;
typedef	int	s32;
typedef	unsigned short int	u16;
typedef	short int	s16;
typedef	unsigned char	u8;
typedef	char	s8;
//---------------------------------------------------------------------------------

#define ZT_COLOR_BLACK 			RGB(0, 0, 0)
#define ZT_COLOR_RED 			RGB(255, 0, 0)
#define ZT_COLOR_GREEN 			RGB(0, 215, 0)
#define ZT_COLOR_YELLOW			RGB(255, 255, 0)
#define ZT_COLOR_GRAY 			RGB(128, 128, 128)

#define ZT_COLOR_HIGHLIHGT 		RGB(42, 41, 255)
#define ZT_COLOR_SELECTED 		RGB(125, 200, 56)

#define	BARCODE_VALID_TIME		(10 * 1000)		// 10 sec

#define ZT_EEPROM_LAST_PAGE		383		// (49,152 / 128) - 1

enum
{
	ITEM_STATE_INFO = 0,
	ITEM_STATE_DISABLE,
	ITEM_STATE_ENABLE,
	ITEM_STATE_TEST_SUCCESS,
	ITEM_STATE_TEST_FAIL
};

enum
{
	MODULE_VENDOR_NONE = 0,

	MODULE_VENDOR_VISIONOX = 1,

	MODULE_VENDOR_TRULY = 11,			//ADD 2021.08.12 IFE OFF

	MODULE_VENDOR_JING_ZHUO = 21,		//ADD 2021.08.12 IFE OFF

	MODULE_VENDOR_AUO = 31,		//ADD 2021.08.12 IFE OFF

	MODULE_VENDOR_XIAOMI = 41,		//ADD 2022.03.31 IFE OFF

	MODULE_VENDOR_EDO = 51,		//ADD 2022.09.01 MPtool_Ver05
};

class CSampleData
{
public:
	CSampleData()
	{
		m_item = "";
		m_info = "";
	}

	CString m_item;
	CString m_info;
	int m_image;
};

struct vector3D
{
	float x;
	float y;
	float v;
};

struct vector2D
{
	float x;
	float y;
};

struct color
{
	float r;
	float g;
	float b;
};

struct vector5D
{
	float x;
	float y;
	float z;
	float p;
	float q;
};

struct _area_info{
	u8 start_x;
	u8 end_x;
	u8 start_y;
	u8 end_y;
};


typedef enum {
	NONE,
	POINT_TYPE,
	LINE_TYPE,
}emPointType;

typedef enum {
	TOUCH_FINGER,
	TOUCH_STYLUS,
	TOUCH_HOVER,
	TOUCH_GLOVE,
}emTouchType;

typedef enum {
	COMM_USB,
	COMM_TCPIP,
	COMM_ADB,
	COMM_HID,
}emCommMode;

#define MAX_GRAPH_NUM	10

typedef enum {
	GRAPH_NONE,
	GRAPH_REGISTER,
	GRAPH_MAX_VARIATION,
	GRAPH_NOISE_RMS,
	GRAPH_MAX_VAL,
	GRAPH_MIN_VAL,
	GRAPH_SPECIFIC_NODE,
	GRAPH_FULL_NODE,
	GRAPH_MOVING_DISTANCE,
	GRAPH_MOVING_VELOCITY,
	GRAPH_CURRENT_CONSUMPTION,
	GRAPH_SCRIPT,
}emGraphType;

typedef	struct {
	BOOL use;
	emGraphType type;
	u16 regNum;
	COLORREF color;
	s16 val;
}stGraphInfo;

typedef union __sub_status{
	u8 val;
	struct _bit{
		u8	exist : 1;
		u8	down : 1;
		u8	move : 1;
		u8	up : 1;
		u8	type : 2;
		u8	reaction_cnt_full : 1;
		u8	width_change : 1;
	}bit;
}_sub_status;

typedef	struct
{
	u16	x;
	u16	y;

	u8	width;
	_sub_status	sub_status;

}_stCoordinfo;

typedef	struct
{
	u16	x;
	u16	y;

	u8	width;
	_sub_status	sub_status;

	u8	minorwidth;
	u8	angle;

}_stCoordinfoEx;


typedef	struct
{
	u16	status;
	u8	finger_cnt;
	u8	time_stamp;
	_stCoordinfo	coord[MAX_SUPPORTED_FINGER_NUM];
}stCoordinfo;

typedef	struct
{
	u16	status;
	u8	finger_cnt;
	u8	time_stamp;
	_stCoordinfoEx	coord[MAX_SUPPORTED_FINGER_NUM];
}stCoordinfoEx;

typedef	struct {
	POINT	Point;
	int	width;
	int	minor_width;
	int	angle;
	//CString	strInfo;
	char	strInfo[512];
}stSinglePointInfo;

#define	MAX_RAW_DATA_LEN					DEF_MAX_X_LINE_NUM*DEF_MAX_Y_LINE_NUM + sizeof(stCoordinfo)
#define	MAX_RAWDATA_NUM						DEF_MAX_X_LINE_NUM*DEF_MAX_Y_LINE_NUM*2

#define TRX_SHORT_REFERENCE					1000

#define MAX_MEASURE_CNT						100

#define	MAX_MEASURE_DELTA_RAWDATA_NUM		( (DEF_MAX_X_LINE_NUM*DEF_MAX_Y_LINE_NUM*2)  + (DEF_MAX_Y_LINE_NUM*2) + (DEF_MAX_Y_LINE_NUM*2) + (DEF_MAX_X_LINE_NUM*2) + (DEF_MAX_Y_LINE_NUM*2) + (DEF_MAX_X_LINE_NUM*2) )

typedef	struct
{
	u16 icRevision;
	u16 icSignature;
	u16	icFirmwareVersion;
	u16 usbJigHwVerion;
	u16 usbJigSwVerion;
	s16 icRegData[MAX_REGISTER_COUNT];
}stCapInfo;

typedef	enum {
	COMM_NOT_FOUND,
	COMM_FOUND,
	COMM_INITIALIZED,
	COMM_UPGRADE,
	COMM_OTHER_TASK,
}emCommStatus;

typedef	struct
{
	u8	usbDevStatus;
	char authorityDepth;

	u16 icSignature;

	//int totalRegisterCount;
	//int totalGroupCount;
}stMyCapInfo;

typedef enum {
	SAVE_INFO_EEPROM,
	SAVE_INFO_IUM,
}emSaveInfo;

#if defined(D_USE_CHECK_BUTTON_TEST)
enum
{
	E_BUTTON_TEST_OFF = 0,

	E_BUTTON_TEST_ON,
	E_BUTTON_TEST_REQUEST,
	E_BUTTON_TEST_OK,

	E_BUTTON_TEST_MAX
};

typedef enum {
	eAllRetryStatus_Nothing,
	eAllRetryStatus_Running,
}e_AllRetryStatus;

typedef struct _BUTTON_STATUS
{
	BOOL		bShowDialog;

	int			nStatus[MAX_DEVICE_COUNT];
}BUTTON_STATUS;
#endif

//---------------------------------------------------------------------------------
//IC Signature Á¤ÀÇ
enum
{
	IC_TYPE_1 = 0,		// ZT2628 / ZTW523 			: 0xE628
	IC_TYPE_2,			// ZT7532s / ZT7528 / ZTW522 	: 0xE532
	IC_TYPE_3,			// ZT7538 					: 0xE538
	IC_TYPE_4,			// ZT7548 / ZTL545 			: 0xE548
	IC_TYPE_5,			// ZTM620 					: 0xE628
	IC_TYPE_6,			// ZT2717 / ZTW622				: 0xE622
	IC_TYPE_7,			// ZTM630						: 0xE630
	IC_TYPE_8,			// ZTL650						: 0xE650
	IC_TYPE_9,			// ZTL650M					: 0x650E

	IC_TYPE_10,			// ZTW623						: 0xE623
	IC_TYPE_11,			// ZTM730						: 0xE730
	IC_TYPE_12,			// ZTL760						: 0xE760
	IC_TYPE_13, 			// GTY137						: 0xE137
	IC_TYPE_14, 			// ZTW722						: 0xE722
	IC_TYPE_MAX,
};

#define IC_GROUP_1(name)		(	name == 0xE700 || \
									name == 0xE548 || \
									name == 0xE545 || \
									name == 0xE538 || \
									name == 0xE532 || \
									name == 0xE638 || \
									name == 0xE628 || \
									name == 0xE622 || \
									name == 0xE630 || \
									name == 0xE623 || \
									name == 0xE730 || \
									name == 0xE722 	)

#define IC_GROUP_2(name)		(	name == 0xE650 || \
									name == 0x650E	|| \
									name == 0xE760    || \
									name == 0xE137 	)

typedef	struct _T_IUM_CMD_INFO
{
	u16	nIumCmdEnter;
	u16	nIumCmdSave;
	u16	nIumCmdExit;
	u16	nIumCmdCustomAddr;

	u16 nIumAreaCount;
	u16 nIumCmdSelectPage;
}T_IUM_CMD_INFO;

//---------------------------------------------------------------------------------


#define I2C_SLAVE_ADDRESS_20	(0x20 << 1)
#define I2C_SLAVE_ADDRESS_40	(0x40 << 1)


typedef	struct _T_FW_DOWNLOAD_INFO
{
	// F/W binary info
	BOOL	bFwBinaryLoad;
	s8 *  	pFwBinaryData;
	u32		nFwBinaryDataSize;
	u32		nMemSize;

	// IC info
	u16 	icPageSize;
	u16 	icIumSize;
	u16 	icFuzingDelay;
	u16		icBurstPacketSize;

	u16		icDefaultOscFreq;		// ROM default OSC Frequency
	u32		icNvmDelayRegister;
	u32		icNvmDelayTime;			// erase/program time according to ROM default OSC Frequency

	// Feature
	BOOL	bDontEraseLastPage;
}T_FW_DOWNLOAD_INFO;

extern T_FW_DOWNLOAD_INFO g_FwDownloadInfo;

extern BOOL g_nSlaveAddress;
extern BYTE I2C_SLAVE_ADDRESS[2];

extern stMyCapInfo	gCapInfo;
extern	HANDLE g_hSemaphore;
extern HANDLE g_hInfoSemaphore;

extern BOOL g_bPowerState;
extern BOOL g_bAppExit;
extern BOOL g_bUseMyDelayMs;

extern int g_nIcType;
extern u16 IC_SIGNATURE[IC_TYPE_MAX];

extern T_IUM_CMD_INFO ZT_GET_IUM_CMD_INFO(u16 nIcSignature);

//---------------------------------------------------------------------------------
// VENDOR COMMAND LIST

enum
{
	// System Control

	VCMD_ENABLE				= 0,
	VCMD_NVM_VALID,								// jump F/W
	VCMD_NVM_INIT,
	VCMD_NVM_WRITE_ENABLE,
	VCMD_INTN_CLEAR,

	VCMD_ID,
	VCMD_REG_READ,
	VCMD_REG_WRITE,

	// Firmware Upgrade

	VCMD_FW_UPGRADE_INIT,
	VCMD_FW_UPGRADE_PGM,
	VCMD_FW_UPGRADE_READ,
	VCMD_FW_UPGRADE_PACKET_SIZE,
	VCMD_FW_UPGRADE_REPEAT,
	VCMD_FW_UPGRADE_MODE,
	VCMD_FW_UPGRADE_FLUSH,
	VCMD_FW_UPGRADE_WRITE_MODE,
	VCMD_FW_UPGRADE_MASS_ERASE,
	VCMD_FW_UPGRADE_START_PAGE,

	VCMD_FW_UPGRADE_NVM_CE_SELECT,
	VCMD_FW_UPGRADE_NVM_TMODE_CHANGE,

	VCMD_IUM_WRITE_PREV,
	VCMD_IUM_WRITE_DATA,
	VCMD_IUM_READ_DATA,
	VCMD_IUM_ERASE,

	VCMD_OSC_FREQ_SEL,

	VCMD_MAX
};

#define	VCMD_INVALID_VALUE		0xFFFF

extern u16 ZT_VCMD(u16 index);
//---------------------------------------------------------------------------------


#define RESULT_NONE	0
#define RESULT_PASS	1
#define RESULT_FAIL	2

//---------------------------------------------------------------------------------
// WM_USER
//---------------------------------------------------------------------------------
#define WM_USER_PROGRESS_UPDATE						(WM_USER + 300)

#define WM_USER_TSM_TEST_START						(WM_USER + 2000)
#define WM_USER_TSM_TEST_START_BY_FW_AUTO_DOWNLOAD	(WM_USER + 2001)
#define WM_USER_LIST_CTRL_HEADER_CLICK				(WM_USER + 2002)
#define WM_USER_LIST_CTRL_LOG_CLICK					(WM_USER + 2003)

#define WM_USER_SHOW_BUTTON_TEST					(WM_USER + 2004)
#define WM_USER_UPDATE_BUTTON_STATUS				(WM_USER + 2005)

#ifdef _DEBUG
#define WM_USER_UPDATE_BUTTON_VALUE					(WM_USER + 2006)
#endif

#define WM_USER_EXECUTE_ASIC_ID_MODE				(WM_USER + 2007)
#define WM_USER_EXECUTE_GRAPH_MODE					(WM_USER + 2008)

#define WM_USER_BARCODE_RUN_DIALOG					(WM_USER + 2009)
#define WM_USER_BARCODE_SHOW_DIALOG					(WM_USER + 2010)
#define WM_USER_BARCODE_TEST_COMPLETED				(WM_USER + 2011)
#define WM_USER_BARCODE_DEVICE_CHANGED				(WM_USER + 2012)
#define WM_USER_BARCODE_CALLBACK_EVENT				(WM_USER + 2013)

#define WM_USER_USB_HID_DEVICE_DISCONNECT			(WM_USER + 2014)

#define WM_USER_SET_FOCUS_TO_LIST					(WM_USER + 2015)

#define WM_USER_DEV_SAVE_IUM_EVENT					(WM_USER + 3000)
#define WM_USER_SAVE_ASIC_ID_MODE					(WM_USER + 3001)
#ifdef _DEBUG
#define WM_USER_MY_TEST_IUM_DEBUGGING_EVENT			(WM_USER + 6518)
#endif

//---------------------------------------------------------------------------------
// TIMER
//---------------------------------------------------------------------------------
#define TIMER_CHECK_HID_DEVICE						(3000 + 1)
#define TIMER_SHOW_CONNECT_INFO						(3000 + 2)
#define TIMER_CHECK_START_PIN_STATUS				(3000 + 3)
#define TIMER_GPIO_BUTTON_TEST_REMAIN_TIME			(3000 + 4)
#define TIMER_GOTO_STANDBY							(3000 + 5)
#define TIMER_BARCODE_AUTO_RESET					(3000 + 6)
#define TIMER_DEBUG_TEST_AGAIN						(3000 + 7)
#define TIMER_SOLUTION_RESULT_NOTIFY				(3000 + 8)

#if defined(D_DEBUG_FW_UPGRADE_TEST)
#define TIMER_DEBUG_FW_UPGRADE_TEST					(3000 + 50)
#endif

#define TIMER_CLEAR_BARCODE_INFO_0					(4000 + 0)
#define TIMER_CLEAR_BARCODE_INFO_1					(4000 + 1)
#define TIMER_CLEAR_BARCODE_INFO_2					(4000 + 2)
#define TIMER_CLEAR_BARCODE_INFO_3					(4000 + 3)
#define TIMER_CLEAR_BARCODE_INFO_4					(4000 + 4)
#define TIMER_CLEAR_BARCODE_INFO_5					(4000 + 5)
#define TIMER_CLEAR_BARCODE_INFO_6					(4000 + 6)
#define TIMER_CLEAR_BARCODE_INFO_7					(4000 + 7)

extern CWnd * g_pTestModeViewWnd;

extern int m_nLogLimitNumOfChar_0;

extern CStringArray		g_strModuleIdList;
extern CUIntArray		g_nModuleIdList;
extern CString m_strIumDir;
extern double m_vih_ref;
extern double g_vcca_ref;
extern double g_voltage;
#endif
