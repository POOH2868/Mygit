#ifndef _REGISTER_H
#define	_REGISTER_H

#define MAX_REGISTER_COUNT	2048		// E700 -> 2Kword

#define	MAX_SUPPORTED_FINGER_NUM	10
#define	MAX_BUTTON_NUM	8

#define DEF_MAX_X_LINE_NUM	40
#define DEF_MAX_Y_LINE_NUM	30


#define REG_START	0x10

// Group Id
#define INVALIDGROUP		0x00
#define NORMALGROUP			0x01
#define SENSITIVITYGROUP    0x02
#define BASELINEGROUP       0x03
#define NOISEGROUP	        0x04
#define LOWPALMNOISEGROUP   0x05
#define AUTOLEVELNOISEGROUP   0x06
#define NORMALNOISEGROUP   0x07
#define SMALLPALMNOISEGROUP   0x08
#define LARGEPALMNOISEGROUP   0x09
#define SMALLNOISEGROUP   0x0A
#define BSRNOISEGROUP   0x0B
#define ESDNOISEGROUP   0x0C
#define SELFFILTERGROUP   115
#define WATERMODEGROUP   116
#define GLOVEMODEGROUP	117

#define SCANAREAGROUP          0x0D
#define COORDINFOGROUP          0x0E
#define POWERSAVINGGROUP          0x0F
#define CHENNALINFOGROUP			0x10
#define BUTTONINFOGROUP          0x11
#define INTERRUPTINFOGROUP          0x12
#define CALIBRATIONINFOGROUP          0x13
#define FILTERGROUP          0x14
#define AFEFREQINFOGROUP	0x15
#define AFEOPERATIONINFOGROUP	0x16
#define AFETESTGROUP	0x17
#define	AFECONFIGGROUP	114
#define	FREQTABLEGROUP	0x18
#define	JIGMODEGROUP	0x19
#define	JIGMODEGROUP01	0x20
#define	JIGMODEGROUP02	0x21
#define	EXFREQTABLEGROUP	0x22
#define SVIEWGROUP		0x23
#define PSENSITIVEMODEGROUP		118
#define SENSITIVEMODEGROUP		119

#define POWERSAVINGSUBGROUP		120
#define GESTUREGROUP		121
#define DSP_RAWDATA_FILTERGROUP		122

#define NORMAL_ID_GROUP		123
#define NORMAL_INTERNAL_FLAG_GROUP		124
#define NORMAL_CHECKSUM_GROUP		125
#define NORMAL_DELAY_GROUP		126
#define NORMAL_OTHER_INFO_GROUP		127

#define POINTFILTER_GROUP		129
#define SINARIO_FILTER_GROUP		128
#define SENSITIVEMODE_INTERNALFLAG_GROUP		130

#define BSR_MP_GROUP		131
#define BSR_FLATNESS_GROUP		132
#define BSR_REFFLATNESS_GROUP		133
#define PALM_FILTER_GROUP		134

#define NOISE_MONITOR_GROUP		135
#define AUTOLEVEL_ENTERCONDITION_GROUP		136
#define AUTOLEVEL_OPERATON_GROUP		137

#define NOISE_MUTUAL_MONITOR_GROUP		138
#define NOISE_SELF_MONITOR_GROUP		139

#define DSP_GROUP		140
#define DSP_SEARCH_GROUP		141
#define DSP_COORD_GROUP		142
#define DSP_POINTFILTER_GROUP		143

#define CHENNALINFO_X_GROUP			144
#define CHENNALINFO_Y_GROUP			145

#define HW_OPTION_GROUP			146
#define AFE_X_SELF_GROUP			147
#define AFE_Y_SELF_GROUP			148
#define AFE_MUTUAL_GROUP			149
#define AFE_XY_PORT_GROUP			150

#define WATER_ENTERCONDITION_GROUP		151
#define WATER_OPERATION_GROUP		152
#define WATER01_OPERATION_GROUP		153
#define WATER02_OPERATION_GROUP		154

#define CLOSE_POINT_REMOVE_GROUP		155
#define FREE_SPACE_GROUP		156
#define ESD_IRON_COVER_GROUP		157
#define INTTIME0_AUTO_SCAN_GROUP	158

#define RESERVED_GROUP		47
#define RESERVED_GROUP_00		48
#define RESERVED_GROUP_01		49
#define RESERVED_GROUP_02		50
#define RESERVED_GROUP_03		51
#define RESERVED_GROUP_04		52
#define RESERVED_GROUP_05		53


#define ENDGROUP		0xFF


#define	VALID	TRUE
#define	INVALID	FALSE

#define	EDITABLE	TRUE
#define	INEDITABLE	FALSE


// Group Name
typedef struct
{
	int	id;
	char bRoot;
	char name[128];
	char authorityDepth;
}stRegGroupInfo;

typedef	enum
{
	HEX_FORMAT = 0,
	SIGNED_FORMAT,
	UNSIGNED_FORMAT,
	CAP_FORMAT,
	FREQUENCY_FORMAT,
	OSC_FREQUENCY_FORMAT,
	BIT_TYPE0_FORMAT,	// interrupt
	BIT_TYPE1_FORMAT,	// internal flag
	BIT_TYPE2_FORMAT,
	BIT_TYPE3_FORMAT,
	BIT_TYPE4_FORMAT,
}_input_type;


#define TREE_DEPTH_0	1
#define TREE_DEPTH_1	0
#define TREE_DEPTH_2	2
#define TREE_DEPTH_3	3
#define TREE_DEPTH_4	4

#define NORMAL_AUTHORITY	0
#define USER_AUTHORITY	1
#define ROOT_AUTHORITY	2

typedef	struct
{
	BOOL bValid;
	int	 nGroupID;
	int	nOrder;		//순서 : 중간이 빠져 있으면 안됨
	char name[128];
	char help[512];
	BOOL bEditable;
	char inputType;
	char authorityDepth;
	int  Chip_Type;
}tsRegNameInfo;

#define All_ZT75XX	0
#define ZT7554		1
#define ZT7545_7548	2
#define ZT7538		3
#define	ZT7532		4
#define	ZT7532S		5
#define	ZT7527		6
#define	ZT7522		7

#if(0)
extern stRegGroupInfo *gRegGroupInfo;
extern stRegGroupInfo g3rdRegGroupInfo[];
extern stRegGroupInfo g4thRegGroupInfo[];
extern stRegGroupInfo g5thRegGroupInfo[];
extern tsRegNameInfo *gRegNameInfo;
extern tsRegNameInfo g3rdRegNameInfo[];
extern tsRegNameInfo g4thRegNameInfo[];
extern tsRegNameInfo g5thRegNameInfo[];
#endif

//=======================================================

#define CMD_SW_RESET						0x0000
#define CMD_WAKE_UP							0x0001
#define CMD_NVM_AUTO_LPM_OFF				0x0002
#define	CMD_CLEAR_INTERRUPT					0x0003

#define CMD_GOTO_DEEP_SLEEP					0x0004
#define CMD_GOTO_IDLE_CHINA					0x0004		// CHINA

#define CMD_GOTO_SLEEP						0x0005

#define	CMD_GET_CALIBRATION_DATA			0x0006
#define	CMD_SAVE_REGISTER					0x0007
#define	CMD_SAVE_CALIBRATION_DATA			0x0008

#define CMD_ENTER_JIG_MODE					0x0009		// IFE
#define	CMD_GOTO_DEEP_SLEEP_CHINA			0x0009		// CHINA

#define CMD_I2C_PACKET_START				0x000A
#define CMD_I2C_PACKET_END					0x000B
#define CMD_DRIVE_INTERRUPT					0x000C
#define	CMD_GOTO_HID_MODE					0x000D
#define CMD_HID_DESCRIPTOR					0x000E
#define RESERVED_000F						0x000F //[Caution!!!]사용금지.


//=======================================================

#define	BIT_PT_CNT_CHANGE			0									//FW:define DEF_BIT_PT_CNT_CHANGE			0
#define	BIT_DOWN					1									//FW:define DEF_BIT_DOWN					1
#define	BIT_MOVE					2									//FW:define DEF_BIT_MOVE					2
#define	BIT_UP						3									//FW:define DEF_BIT_UP						3
#define	BIT_LARGE_PALM_REPORT		4									//FW:define DEF_BIT_LARGE_PALM_SWIPE		4
#define	BIT_LARGE_PALM_REJECT		5									//FW:define DEF_BIT_LARGE_PALM_REPORT		5
#define	BIT_WAKE_UP_GESTURE			6									//FW:define DEF_BIT_WAKE_UP_EVENT			6
#define	BIT_WHEEL_REPROT			7									//FW:define DEF_BIT_WHEEL_REPROT			7
#define	BIT_WIDTH_CHANGE			8									//FW:define DEF_BIT_POCKET_TOUCH			8
#define BIT_RESERVED09				9									//FW:define DEF_BIT_RESERVED09				9
#define	DEF_BIT_INT_PIN_DISABLE		10									//FW:define DEF_BIT_INT_PIN_DISABLE			10
#define	BIT_PT_EXIST				11		// status register only		//FW:define DEF_BIT_PT_EXIST				11
#define	BIT_PROXI_ON				12									//FW:define DEF_BIT_PROXIMITY_REPORT		12
#define	BIT_MUST_ZERO				13									//FW:define DEF_BIT_MUST_ZERO				13
#define	BIT_DEBUG					14									//FW:define DEF_BIT_FW_STATUS				14
#define	BIT_BUTTON_EVENT			15									//FW:define DEF_BIT_BUTTON_EVENT			15

//=======================================================

#define REG_TOUCH_MODE						0x10

	#define TOUCH_POINT_MODE							0
	#define TOUCH_MODE_STRENGTH_DATA					3
	#define TOUCH_RAW_HYBRID_PROCESSED_DATA_MODE		4
	#define TOUCH_DEFAULT_N_DATA_MODE					6
	#define TOUCH_CAL_N_DATA_MODE						7
	#define TOUCH_MODE_CND								7
	#define TOUCH_EEPROM_CALIBRATION_DATA_MODE			8
	#define TOUCH_TS1_MEM_MODE							9
	#define TOUCH_BASELINE_DATA_MODE					10
	#define TOUCH_MODE_DND								11
	#define TOUCH_MODE_DND_HF							12
	#define TOUCH_MODE_TRX_SHORT						13
	#define TOUCH_RAW_CHECK_SHORT_MODE					14
	#define TOUCH_MODE_JITTER							15
	#define TOUCH_SELF_BASELINED_DATA_MODE				16
	#define TOUCH_DIGITAL_WHEEL_DATA_MODE				16
	#define TOUCH_MODE_SELF_DND							17
	#define TOUCH_SELF_CALIBRATION_DATA_MODE			18
	#define TOUCH_SELF_EEPROM_CALIBRATION_DATA_MODE		19
	#define TOUCH_MODE_HYBRID_BASELINED_DATA			20
	#define TOUCH_MODE_HIGH_VOLTAGE						25
	#define TOUCH_MODE_HW_CALIBRATION					33
	#define TOUCH_MODE_SELF_SATURATION					39
	#define TOUCH_MODE_VIRTUAL_DRAW						48
	#define TOUCH_MODE_ONLY_SELF_DND					49

#if 1	// #if defined(D_USE_FW_CHINA_FRAMEWORK)
	#define TOUCH_PP_RX_SHORT_N_DATA_MODE				12
	#define TOUCH_PP_TX_SHORT_N_DATA_MODE				13
#endif

//=======================================================

#define REG_CHIP_REVISION					0x11
#define REG_FIRMWARE_VERSION				0x12
#define REG_DATA_VERSION					0x13
#define REG_HW_ID							0x14	//tsp type
#define REG_SUPPORTED_FINGER_NUM 			0x15
#define REG_MAX_Y_NUM 						0x16

//=======================================================

#define REG_INTERNAL_FLAG_00				0x17

	#define DEF_NOT_USE_1ST_BASELINE				0
	#define DEF_NOT_USE_2ND_BASELINE				1

	// IFE
	#define DEF_USE_DEBUG_FORCE_NOISE_ON			13
	#define DEF_USE_DEBUG_FORCE_NOISE_OFF			14

	// Chine
	#define DEF_USE_AUTO_DETECT_NOISE				13
	#define DEF_USE_POWER_SAVING_MODE				14

//=======================================================

#define REG_EEPROM_INFO 					0x18
#define REG_INITIAL_TOUCH_MODE 				0x19

//=======================================================

#define REG_PROTOCOL_MODE 					0x1A

	typedef enum
	{
		PROTOCOL_MODE_ZINITIX = 0,
		PROTOCOL_MODE_SAMSUNG,
		PROTOCOL_MODE_HID,
		PROTOCOL_MODE_CHINA
	}T_PROTOCOL_MODE;

#define REG_CAL_N_TOTAL_NUM					0x1B
#define REG_REG_IC_VENDOR_ID 				0x1C
#define REG_CUR_SENSITIVITY_TH 				0x1D
#define REG_TSM_MODULE_ID					0x1E	//Module 사 별로 별도관리
/*
;				Module Vendor Code
;				EELY EE
;				ELK EL
;				J-TOUCH JT
;				O-FILM OF
;				SRI SR
;				TOP-TOUCH TT
;				TRULY TR
;				Wintek WT
;				광통신 FI
;				디지텍 DI
;				멜파스 ME
;				모린스 MO
;				세원텔레텍 SE
;				시노펙스 SY
;				에스맥 SM
;				영패스트 YO
;				일진 IL
;				태양기전 TY
*/
#define REG_CUR_RAW_VARIATION 			0x1F
#define REG_JITTER_N_DATA_CNT			0x1F
//=======================================================
#define REG_SENSITIVITY_TH					0x20
#define REG_Y0_SENSITIVITY_TH				0x21
#define REG_LAST_Y_SENSITIVITY_TH			0x22
#define REG_X0_SENSITIVITY_TH				0x23
#define REG_LAST_X_SENSITIVITY_TH			0x24
#define REG_ACTIVE_UP_SENSITIVITY_RATIO		0x25
#define REG_LOW_PALM_SENSITIVITY			0x26	//low palm check를 위해서는 이것보다는 커야 함
#define REG_EACTIVE_UP_SENSITIVITY_RATIO	0x27

//=======================================================
#define REG_BASELINE_UPDATE_DELTA			0x28
#define REG_BASELINE_UPDATE_2ND_DELTA		0x29
#define REG_BASELINE_UPDATE_PERIOD			0x2A
#define REG_BASELINE_UPDATE_2ND_PERIOD		0x2B
#define REG_FORCE_BASELINE_UPDATE_PERIOD	0x2C
#define REG_BASELINE_UPDATE_2ND_STEP		0x2D
#define REG_SENSITIVE_MODE_SENSITIVTY		0x2E
#define REG_RESERVED_2FH					0x2F

//=======================================================
#define REG_FIR_COEFFICIENT					0x30
#define REG_LOW_PALM_MAX_X					0x31
#define REG_LOW_PALM_MAX_Y					0x32
#define REG_SW_FIR_COEFFICIENT				0x33
#define REG_SW_W_FIR_COEFFICIENT			0x34
#define REG_WIDTH_AREA_COEF					0x35
#define REG_LOW_PALM_MAX_AREA				0x36
#define REG_SW_FIR_DISTANCE					0x37
#define REG_SW_ANGLE_FIR_COEFFICIENT		0x38
#define REG_ENTER_LEVEL2_PP_VARIATION		0x39
#define REG_ENTER_LEVEL2_ADC_VARIATION		0x3A

//=======================================================
#define REG_REACTION_COUNT					0x3B
#define REG_SMALL_PALM_AREA_THRESHHOLD		0x3C
#define REG_HYSTERISYS_RATIO				0x3D
#define REG_LARGE_PALM_REJECT_N_CNT			0x3E
#define REG_LARGE_PALM_REJECT_AREA_TH		0x3F
#define REG_LARGE_PALM_REPORT_AREA_TH		0x40
#define REG_LARGE_PALM_UP_SKIP_CNT			0x41
#define REG_SMALL_FINGER_REF_VAL			0x42
#define REG_CUTOFF_IDATA_RATIO				0x43
#define REG_CUTOFF_WIDTH_RATIO				0x44
#define REG_SMALL_PALM_SENSITIVITY_MAGNIFICATION	0x45
#define REG_LARGE_PALM_SENSITIVITY_MAGNIFICATION	0x46
//Low Palm 이 걸렸을 때의 Button Sensitivity -> Low Palm중 가장큰값보다 몇배 이상의 값이 Button에 눌려야 Touch 로 인식
#define REG_LOW_PALM_SENSITIVITY_MAGNIFICATION	0x47
#define REG_LARGE_PALM_TIME					0x48
#define REG_SMALL_REJECT_BASELINE_UPDATE_PERIOD	0x49
#define REG_REJECT_DENSITY_RATIO			0x4A
#define REG_REJECT_SMALL_NOISE_REF_VAL		0x4B
#define REG_REJECT_DENSITY_LIMIT			0x4C
#define REG_REJECT_DENSITY_MIN_LEN 			0x4D
#define REG_LARGE_PALM_BASELINE_UPDATE_PERIOD	0x4E
#define REG_SMALL_PALM_BASELINE_UPDATE_PERIOD	0x4F
//=======================================================
#define REG_AREA_REF_VAL_RATIO				0x50
#define REG_MINUSF_M_COMPS_RATIO			0x51
#define REG_SKIP_MULTI_MAX_X				0x52
#define REG_SKIP_MULTI_MAX_Y				0x53
#define REG_SKIP_MULTI_MAX_AREA				0x54
#define REG_SKIP_MULTI_MIN_VAL				0x55
//옆 Line 과의 접착력이 이 비율이상 되어야 한 Node  이것보다 작은 비율이라면 Multi
#define REG_ADHESION_RATIO					0x56
#define REG_SKIP_MULTI_MIN_AREA				0x57
//Scan된 Area 안에서 Multi 검색을 할 때 그 안에서의 CutOff 조건
#define REG_MULTI_CUTOFF_RATIO				0x58
//가장 큰 값에 대한 비율, 접착력을 계산할 때 현재 Node와 인접노드 모두 이것에 의해 계산된 값보다는 커야함.
#define REG_ADHESION_REF_VAL_RATIO			0x59
#define REG_SMALL_FINGER_MAX_NODE_LEN		0x5A
#define REG_FINGER_MIN_NODE_LEN				0x5B
#define REG_CAL_G_COORD_X_LEN				0x5C
#define REG_CAL_G_COORD_Y_LEN				0x5D
#define REG_ONE_RECT_COEF					0x5E
#define REG_AREA_CNT_REF_VAL_RATIO			0x5F

//=======================================================
#if 0
#define REG_TOTAL_NUMBER_OF_X				0x60
#define REG_TOTAL_NUMBER_OF_Y				0x61
#define REG_X_DRIVE_NUM						0x62
#endif
#define REG_LEVEL_REAL_VARIATION			0x72
#define REG_LEVEL_REAL_MAX					0x73
#define REG_LEVEL_REAL_MIN					0x74
#define REG_FINGER_MAX_NODE_LEN				0x75


//=======================================================
#define REG_CALIBRATION_REF					0x76
#define REG_CALIBRATION_SCAN_CNT			0x77
#define REG_CALIBRATION_DEFAULT_N			0x78
#define REG_CALIBRATION_DEFAULT_C			0x79
#define REG_CALIBRATION_MODE				0x7A
#define REG_SOFT_CALIBARTION_COUNT			0x7B
#define REG_DELAY_FOR_HOST					0x7C
#define REG_SCAN_COUNT_FOR_HOST				0x7D
#define REG_POWER_STATE_FLAG				0x7E			// 0: Auto      1: Active    2: Idle

#define REG_DELAY_RAW_FOR_HOST				0x7F			// IFE
#define REG_MODEL_NAME						0x7F			// CHINA


//=======================================================
#define REG_STATUS							0x80
#define REG_TS_POINTCNT						0x81
#define REG_POINT							0x82

//=======================================================
#define REG_BUTTON							0xAA
#define REG_CUR_COMP_N_MODE					0xAB
#define REG_COMP_N_MP_RATIO					0xAC
#define REG_COMP_N_PM_RATIO					0xAD
#define REG_COM_N_DOWN_CNT					0xAE
#define REG_COMP_N_SHIFT					0xAF


//=======================================================

#define REG_BUTTON_SUPPORTED_NUM			0xB0
#define REG_BUTTON_REACTION_CNT				0xB1
#define REG_BUTTON_SENSITIVITY				0xB2
#define REG_BUTTON_LINE_TYPE				0xB3
#define REG_BUTTON_LINE_NUM					0xB4
#define REG_BUTTON_RANGE 					0xB5
#define REG_BUTTON_NODE						0xB6
#define	REG_BUTTON_OTHER_SIDE_NODE_0		0xB6
#define REG_BUTTON_WIDTH_MUL_RATIO			0xBE
//view area의 max 값 보다 얼마이상이 커야 한다는 조건이 필요하다면 이 옵션을 사용.
#define REG_BUTTON_PRIOR_RATIO				0xBF


//=======================================================
#define REG_X_RESOLUTION					0xC0
#define REG_Y_RESOLUTION					0xC1
#define REG_COORD_ORIENTATION				0xC2	//BIT 0 X FLIP, BIT 1 Y FLIP, BIT 2 XY SWAP
#define REG_HOLD_THRESHOLD					0xC3
#define REG_WIDTH_HOLD_THRESHOLD			0xC4
#define REG_MT_ADHESION_RATIO				0xC5
#define REG_ASSUME_UP_THRESHHOLD			0xC6
#define REG_ASSUME_UP_SKIP_THRESHHOLD		0xC7
#define REG_X_POINT_SHIFT					0xC8
#define REG_Y_POINT_SHIFT					0xC9
#define REG_VIEW_XF_OFFSET					0xCA
#define REG_VIEW_XL_OFFSET					0xCB
#define REG_VIEW_YF_OFFSET					0xCC
#define REG_VIEW_YL_OFFSET					0xCD
#define REG_SW_ADAPTIVE_LIMIT_LENGTH		0xCE
#define REG_SW_ADAPTIVE_TIMEOUT				0xCF

//=======================================================
#define REG_COEF_X_GAIN						0xD0
#define REG_COEF_Y_GAIN						0xD1
#define REG_FREESPACE_RECOVER_RATIO			0xD2
#define REG_ACTIVE_UP_THRESHHOLD			0xD3
#define REG_POWER_SAVING_TERM				0xD4
#define REG_BDATA_COEF1						0xD5
#define REG_BDATA_COEF2						0xD6
#define REG_BDATA_COEF3						0xD7
#define REG_BSR_DURATION					0xD8
#define REG_BSR_VARIATION_TH				0xD9
#define REG_BSR_MP_RATIO_REF_VAL			0xDA
#define REG_BSR_MP_RATIO_SMALL				0xDB
#define REG_BSR_MP_RATIO_LARGE				0xDC
#define REG_BSR_FLATNESS_VARIATION_TH		0xDD
#define REG_BSR_FLATNESS_LIMIT_TH			0xDE
#define REG_BSR_FLATNESS_REF_VAL			0xDF
#define REG_CUR_FLATNESS					0xE0
#define REG_BSR_REF_BASELINE_LIMIT_TH		0xE1
#define REG_OSC_ONOFF_DELAY					0xE2
#define REG_ESD_NOISE_VARIATION_TH			0xE3
#define REG_ESD_NOISE_VARIATION_CNT_TH		0xE4
#define REG_ESD_NOISE_MP_RATIO				0xE5
#define REG_ESD_NOISE_TH					0xE6
#define REG_ESD_NOISE_MIN_CRT				0xE7
#define REG_ESD_NOISE_MAX_CRT				0xE8
#define REG_ESD_NOISE_CUR_VARIATION_CNT		0xE9
#define REG_POSITIVE_RATIO					0xEA
#define REG_VIEW_EDGE_SCALE_FACTOR			0xEB
#define REG_VIEW_EDGE_XF_OFFSET				0xEC
#define REG_VIEW_EDGE_XL_OFFSET				0xED
#define REG_VIEW_EDGE_YF_OFFSET				0xEE
#define REG_VIEW_EDGE_YL_OFFSET				0xEF

//=======================================================
#define REG_INTERRUPT_FLAG					0xF0
#define REG_ESD_INT_INTERVAL				0xF1
#define REG_SPREAD_COMP_RATIO 				0xF2
#if 0
#define REG_Y_INPUT_NUM						0xF3
#endif
#define REG_ENTER_LEVEL0_PP_VARIATION		0xFC
#define REG_SKIP_RAWDATA_MAX_TH				0xFD
#define REG_LEVEL_2_PP_VARIATION_TH			0xFE
#define REG_INTERRUPT_TIMEOUT				0xFF

//=======================================================
#define REG_AFE_FREQUENCY					0x100
#define REG_CUR_LEVEL_DEPTH					0x101
#define REG_CUR_LEVEL_DOWN_CNT				0x102
#define REG_CUR_ADC_VARITAION				0x103
#define REG_CUR_PP_NOISE_VARIATION			0x104
#define REG_SKIP_RAWDATA_VARIATION_TH		0x105
#define REG_AFE_PP_FREQUENCY				0x106
#define REG_CUR_SKIP_STEP					0x107
#define REG_LEVEL_DOWN_CNT					0x108
#define REG_SKIP_RAWDATA_MIN_TH				0x109
#define REG_ENTER_LEVEL_1_ADC_VARIATION_TH	0x10A
#define REG_ENTER_LEVEL_1_PP_VARIATION_TH	0x10B
#define REG_LEVEL_1_PP_VARIATION_TH			0x10C
#define REG_RAWDATA_BS_VARIATION_TH			0x10D
#define REG_LEVEL_1_ADC_VARIATION_TH		0x10E
#define REG_LEVEL_SENSITIVITY_INC			0x10F
#define REG_LEVEL_REACTION_CNT_INC			0x110
#define REG_DETECT_LEVEL_FIR				0x111
#define REG_LEVEL_SW_FIR					0x112
#define REG_LEVEL_SW_FIR_DIST				0x113
#define REG_LEVEL_CUTOFF_IDATA				0x114

//----------------------------------------------
#define REG_DEBUG_00						0x115
#define REG_DEBUG_01						0x116
#define REG_DEBUG_02						0x117
#define REG_DEBUG_03						0x118
#define REG_DEBUG_04						0x119
#define REG_DEBUG_05						0x11A
#define REG_DEBUG_06						0x11B
#define REG_DEBUG_07						0x11C

//----------------------------------------------
#define REG_INTERNAL_FLAG_01				0x11D
#define REG_INTERNAL_FLAG_02				0x11E
#define REG_INTERNAL_FLAG_03				0x11F
#define REG_CHIP_CODE						0x120
#define REG_MINOR_FW_VERSION				0x121
#define REG_AFE_DEFAULT_N					0x122
#define REG_AFE_DEFAULT_C					0x123
#define REG_LEVEL_1_COMP_N_SHIFT			0x124
#define REG_OSC_FREQ						0x125
#define REG_FM_SET							0x126
#define REG_TSY_DEFAULT						0x127
#define REG_INTAMP_BYPASS_EN				0x128
#define REG_INTAMP_TIME2					0x129
#define REG_INTAMP_TIME3					0x12A
#define REG_AFE_SHIFT_VAL					0x12B

#define REG_CHECKSUM_RESULT					0x12C

	#define CHECKSUM_PATTERN_PASS				0x55AA

#define REG_TSX_XA_EN_ADR					0x12D
#define REG_TSX_YIO_EN_ADR					0x12D
#define REG_SYSTEM_CHECKSUM					0x12E
#define REG_TSX_XB_EN_ADR					0x12F
#define REG_TEST_CAP_EN						0x130
#define REG_CODE_TAP						0x131
#define REG_CODE_SIZE						0x132
#define REG_SCANRATE_CNT0					0x133
#define REG_SCANRATE_CNT1					0x134
#define REG_AFE_U_COUNT_N					0x135
#define REG_ADC_MODE						0x136
#define REG_INTAMP_EN						0x137
#define REG_ATEST1_CTRL						0x138
#define REG_ATEST2_CTRL						0x139
#define REG_SEL_TS_MAP						0x13A
#define REG_INTAMP_TIME0					0x13B
#define REG_INTAMP_TIME1					0x13C
#define REG_ADC_TIME						0x13D
#define REG_SF_CAL_RECON					0x13E
#define REG_INTAMP_CHOPPING					0x13F
#define REG_SF_CAL_OFFSET					0x140
#define REG_AFE_SF_MODE						0x141	// 0 = PP Mode, 1 = SF Mode
#define REG_ESD_INT_INTERVAL_PWS			0x142
#define REG_PWS_SCAN_CNT					0x143
#define REG_BSR_REF_BS_LARGE_RATIO			0x144
#define REG_BSR_REF_BS_SKIP_MAX_VAL			0x145
#define REG_BSR_CUR_BS_LIMIT				0x146
#define REG_RESERVED_147H					0x147
#define REG_LEVEL_2_ADC_VARIATION_TH		0x148
#define REG_FREQ_REMAIN_COUNT				0x149
#define REG_FREQ_SEARCH_STATE				0x14A
#define REG_SKIP_ASSUME_UP_SKIP_CNT			0x14B
#define REG_BUFS_EN							0x14C
#define REG_VREF_HL_SEL						0x14D
#define REG_VREF_CM_SEL						0x14E
#define REG_ISRC_CTRL						0x14F

//===========================================
#if 0
#define REG_TM_AFE_FREQUENCY				0x150
#define REG_TM_INTERNAL_FLAG				0x151
#define REG_TM_SENSITIVITY					0x152
#define REG_TM_RAW_AREA_SENSITIVITY_TH		0x153
#define REG_TM_Y_SHORT						0x153
#define REG_TM_Y_SHORT_MAX					0x153
#define REG_TM_MANUAL_POINT_AREA			0x154
#define REG_TM_MANUAL_POINT_RATIO			0x155
#define REG_TM_SOFT_CALIBARTION_COUNT		0x156
#define REG_TM_INTERNAL_FLAG_01				0x157
#define REG_TM_VIEW_EDGE_SCALE_FACTOR		0x158
#define REG_TM_VIEW_EDGE_XF_OFFSET			0x159
#define REG_TM_VIEW_EDGE_XL_OFFSET			0x15A
#define REG_TM_VIEW_EDGE_YF_OFFSET			0x15B
#define REG_TM_VIEW_EDGE_YL_OFFSET			0x15C
#define REG_TM_FIR_COEFFICIENT				0x15D
#define REG_TM_MCOORD_VARIABLE				0x15E	//width에 따라 manual coord를 사용할 지 결정
#define REG_CURRENT_FREQ					0x15F

#define REG_TM_WIDTH_AREA_COEF				0x160
#define REG_TM_BUTTON_SENSITIVITY			0x161
#define REG_TM_REACTION_COUNT				0x162
#define REG_TM_BUTTON_REACTION_CNT			0x163
#define REG_TM_BDATA_COEF1					0x164
#define REG_TM_BDATA_COEF2					0x165
#define REG_TM_BDATA_COEF3					0x166
#define REG_FREQ_TABLE_CNT					0x167
#define REG_LEVEL_FREQUENCY					0x168
#define REG_LEVEL_COMP_N_INC				0x169
#define REG_I2S_CHECKSUM_CNT					0x16A
#define REG_LEVEL_2_COMP_N_SHIFT			0x16B
#define REG_I2S_CHECKSUM_RESULT					0x16C

#define REG_BUTTON_WIDTH					0x16D
#define REG_TM_BTN_WEIGHT_Z					0x175
#define REG_TM_WEIGHT_Z						0x176
#define REG_TM_WEIGHT_Z_GAIN				0x177
#define REG_TM_MAX_WIDTH					0x178
#define REG_TM_X_SHORT						0x178
#define REG_TM_X_SHORT_MAX					0x178
#define REG_TM_REFERENCE_COUNT				0x179
#define REG_TM_REFERENCE_COORD				0x17A

#define REG_REAL_WIDTH						0x1C0
#endif

#define REG_CHECK_RESET_PIN					0x181

#define REG_INIT_FREQ_REMAIN_PERIOD			0x1CA
#define REG_FREQ_SAMPLING_CNT				0x1CB
#define REG_FREQ_BASELINE_INIT_CNT			0x1CC
#define REG_POST_DELAY						0x1CD
#define REG_FREQ_REMAIN_PERIOD				0x1CE
#define REG_FREQ_CHANGE_PP_MIN				0x1CF

//====================================================
// 0x02A0 ~ 0x02D2
#define REG_AFE_CTRL_H		0x02A0
#define REG_AFE_CTRL_L		0x02A1
#define REG_TMR_CTRL_H		0x02A2
#define REG_TMR_CTRL_L		0x02A3
#define REG_DET_CTRL_H		0x02A4
#define REG_DET_CTRL_L		0x02A5
#define REG_FHSS_CTRL		0x02A6
#define REG_FHSS_TABLE_A_H	0x02A7
#define REG_FHSS_TABLE_A_L	0x02A8
#define REG_FHSS_TABLE_B_H	0x02A9
#define REG_FHSS_TABLE_B_L	0x02AA
#define REG_IO_TIME			0x02AB
#define REG_IO_X_DRV_H		0x02AC
#define REG_IO_X_DRV_L		0x02AD
#define REG_IO_Y_DRV_H		0x02AE
#define REG_IO_Y_DRV_L		0x02AF

#define REG_IO_XY_DRV		0x02B0
#define REG_VREF_CTRL_H		0x02B1
#define REG_VREF_CTRL_L		0x02B2
#define REG_AMP_ENABLE_H	0x02B3
#define REG_AMP_ENABLE_L	0x02B4
#define REG_AMP_DISABLE_H	0x02B5
#define REG_AMP_DISABLE_L	0x02B6
#define REG_AMP_PATTERN		0x02B7
#define REG_SH_CTRL_H		0x02B8
#define REG_SH_CTRL_L		0x02B9
#define REG_ADC_CTRL_H		0x02BA
#define REG_ADC_CTRL_L		0x02BB
#define REG_CP_CTRL_H		0x02BC
#define REG_CP_CTRL_L		0x02BD
#define REG_SAMP_CTRL		0x02BE
#define REG_ATEST_A_H		0x02BF

#define REG_ATEST_A_L		0x02C0
#define REG_ATEST_B_H		0x02C1
#define REG_ATEST_B_L		0x02C2
#define REG_TEST_MODE		0x02C3
#define REG_IO_X_DEF_H		0x02C4
#define REG_IO_X_DEF_L		0x02C5
#define REG_IO_X_EVEN_H		0x02C6
#define REG_IO_X_EVEN_L		0x02C7
#define REG_IO_X_ODD_H		0x02C8
#define REG_IO_X_ODD_L		0x02C9
#define REG_IO_XY_DEF		0x02CA
#define REG_IO_XY_EVEN		0x02CB
#define REG_IO_XY_ODD		0x02CC
#define REG_IO_Y_DEF_H		0x02CD
#define REG_IO_Y_DEF_L		0x02CE
#define REG_IO_Y_EVEN_H		0x02CF

#define REG_IO_Y_EVEN_L		0x02D0
#define REG_IO_Y_ODD_H		0x02D1
#define REG_IO_Y_ODD_L		0x02D2


#define REG_TOTAL_NUMBER_OF_X	0x060
#define REG_TOTAL_NUMBER_OF_Y	0x061
#define REG_CUSTOM_BUTTON_CHANNEL	0x065
#define REG_CUSTOM_BUTTON_Y_CHANNEL	0x164
// 0540H ~ 0550H
#define REG_X_DRIVE_NUM		0x240
#define REG_Y_INPUT_NUM		0x250

// 0x02D3 ~ 0570H
#define REG_CFG_CTRL	0x02D3
#define REG_CFG_USE_SELF_XY	0x02D4

#define REG_CFG_SX_SWAP			0x02D6
#define REG_CFG_SX_V_FORCE		0x02D7
#define REG_CFG_SX_U_COUNT		0x02D8
#define REG_CFG_SX_N_COUNT		0x02D9
#define REG_CFG_SX_FREQUENCY	0x02DA
#define REG_CFG_SX_RST0_TIME	0x02DB
#define REG_CFG_SX_RST1_TIME	0x02DC
#define REG_CFG_SX_RST2_TIME	0x02DD
#define REG_CFG_SX_RBG_SEL		0x02DE
#define REG_CFG_SX_AMP_V_SEL	0x02DF
#define REG_CFG_SX_SUB_V_SEL	0x02E0
#define REG_CFG_SX_AMP_CAP		0x02E1
#define REG_CFG_SX_MOVING_AVG	0x02E2


#define REG_CFG_SY_SWAP			0x02E3
#define REG_CFG_SY_V_FORCE		0x02E4
#define REG_CFG_SY_U_COUNT		0x02E5
#define REG_CFG_SY_N_COUNT		0x02E6
#define REG_CFG_SY_FREQUENCY	0x02E7
#define REG_CFG_SY_RST0_TIME	0x02E8
#define REG_CFG_SY_RST1_TIME	0x02E9
#define REG_CFG_SY_RST2_TIME	0x02EA
#define REG_CFG_SY_RBG_SEL		0x02EB
#define REG_CFG_SY_AMP_V_SEL	0x02EC
#define REG_CFG_SY_SUB_V_SEL	0x02ED
#define REG_CFG_SY_AMP_CAP		0x02EE
#define REG_CFG_SY_MOVING_AVG	0x02EF

#define REG_CFG_M_SWAP			0x02F0
#define REG_CFG_M_V_FORCE		0x02F1
#define REG_CFG_M_U_COUNT		0x02F2
#define REG_CFG_M_N_COUNT		0x02F3
#define REG_CFG_M_FREQUENCY		0x0100
#define REG_CFG_M_RST0_TIME		0x02F5
#define REG_CFG_M_RST1_TIME		0x02F6
#define REG_CFG_M_RST2_TIME		0x02F7
#define REG_CFG_M_RBG_SEL		0x02F8
#define REG_CFG_M_AMP_V_SEL		0x02F9
#define REG_CFG_M_SUB_V_SEL		0x02FA
#define REG_CFG_M_AMP_CAP		0x02FB
#define REG_CFG_M_MOVING_AVG	0x02FC

#if 0
//=========================================================================================================
#define REG_AUTO_SENSITIVE_MODE					0x220
#define REG_SENSITIVE_N_INC					0x221
#define REG_SENSITIVE_INTERNAL_FLAG_00			0x222
#define REG_SENSITIVE_INTERNAL_FLAG_01			0x223
#define REG_SENSITIVE_INTERNAL_FLAG_02			0x224
#define REG_SENSITIVE_INTERNAL_FLAG_03			0x225

#define REG_SENSITIVE_SEARCH_TH			0x226
#define REG_SENSITIVE_HYSTERISYS_RATIO			0x227
#define REG_SENSITIVE_GLOVE_SW_MIN_FIR_COEF			0x228
#define REG_SENSITIVE_GLOVE_CUTOFF_RATIO			0x229

#define REG_SENSITIVE_GLOVE_MIN_IN_NORMAL				0x22A
#define REG_SENSITIVE_GLOVE_MAX_IN_NORMAL				0x22B
#define REG_SENSITIVE_GLOVE_SX_SENSITIVITY			0x22C
#define REG_SENSITIVE_GLOVE_SY_SENSITIVITY			0x22D
#define REG_SENSITIVE_GLOVE_S_AREA_RATIO			0x22E
#define REG_SENSITIVE_GLOVE_S_MIN_LEN			0x22F
#define REG_SENSITIVE_GLOVE_S_MAX_LEN			0x230

#define REG_SENSITIVE_OUT_GLOVE_MAX_VALUE				0x231
#define REG_SENSITIVE_AUTO_OUT_FORCE_PERIOD			0x232
#define REG_SENSITIVE_AUTO_OUT_INITIAL_SENSITIVITY			0x233

#define REG_SENSITIVE_REJECT_SMALL_NOISE_REF_VAL		0x234
#define REG_SENSITIVE_REJECT_DENSITY_LIMIT			0x235
#define REG_SENSITIVE_GLOVE_MAX_IN_SENSITIVE			0x236
#define REG_SENSITIVE_GLOVE_M_MIN_LEN					0x237
#define REG_SENSITIVE_GLOVE_M_MAX_LEN					0x238
#define REG_SENSITIVE_NOT_GLOVE_SENSITIVITY_TH			0x239

#define REG_SENSITIVE_BTN_SENSITIVITY_TH			0x174

#define REG_CUR_FILL_30PER_RATIO								0x175
#define REG_SENSITIVE_GLOVE_FILL_30PER_RATIO_TH				0x23A

#define REG_SVIEW_START_NODE				0x23C	//ba
#define REG_SVIEW_END_NODE					0x23D
#define	REG_SENSITIVE_SMALL_FINGER_REF_VAL		0x23B

#define	REG_SENSITIVE_GLOVE_MAX_AREA_SZ		0x23F

//=====================================================================================

#define REG_CHECK_S_MU_RATIO_FLAG		0x0260		// normal whole : BIT1 : y self 사용 , BIT0 : x self 사용
// normal node : BIT2 : x self, BIT3 : y self
// btn node : BIT4 : x self, BIT5 : y self
#define REG_INIT_WHOLE_M_S_X_RATIO		0x026C		//self x ratio
#define REG_INIT_WHOLE_M_S_Y_RATIO		0x026D		//self y ratio
#define REG_WHOLE_M_S_X_RATIO		0x0261		//self x ratio
#define REG_WHOLE_M_S_Y_RATIO		0x0262		//self y ratio

#define REG_NODE_M_S_X_RATIO		0x0263		//self x ratio
#define REG_NODE_M_S_Y_RATIO		0x0264		//self y ratio
#define REG_NODE_CHK_AREA_RANGE		0x026B		//node check area sz

#define REG_BTN_M_S_X_RATIO		0x0265		//self button x ratio
#define REG_BTN_M_S_Y_RATIO		0x0266		//self button y ratio

#define REG_SELF_Y_CUR_FLATNESS		0x0267		// self flatness max
#define REG_SELF_X_CUR_FLATNESS		0x0268		// self flatness max

#define REG_SELF_X_COMPENSATION		0x0269		// Self Compenstion Ratio : recommend : 1200
#define REG_SELF_Y_COMPENSATION		0x026A		// Self Y Compenstion Ratio : recommend : 1200


#define REG_SELF_Y_MAX_VALUE		0x0270
#define REG_SELF_Y_MIN_VALUE		0x0271
#define REG_SELF_Y_MAX_VARIATION		0x0272

#define REG_SELF_X_MAX_VALUE		0x0273
#define REG_SELF_X_MIN_VALUE		0x0274
#define REG_SELF_X_MAX_VARIATION		0x0275


//======================================================
#define REG_WATER_CUR_MODE		0x0280		// water mode
#define REG_WATER_USE_FLAG		0x0281		// BIT0 : In Noise, BIT 1 : In Power Saving, BIT 4: USE Invalid Ref Data

#define REG_WATER_CHECK_MIN_VARIATION_TH		0x0282		// variation
#define REG_WATER_MODE_CHECK_COUNT		0x0293		// Water Mode Scan Count

#define REG_WATER_ENTER_MODE0_MU_FLATNESS		0x0284		// water check : out mutual flatness
#define REG_WATER_MODE_S_FLATNESS_LIMIT		0x0285		// water check : out self flatness

#define REG_WATER_ENTER_MODE1_MU_FLATNESS		0x0286		// water check : enter 1 mutual flatness max
#define REG_WATER_ENTER_MODE1_RUNTIME_TIMEOUT		0x0287		// water check : enter 1 mutual flatness max

#define REG_WATER_MODE1_SUPPORT_FINGER_CNT		0x0288		// Self Compenstion Ratio : recommend : 1200
#define REG_WATER_MODE1_M_SENSITIVITYTH		0x0289		// water mode 1 sensitivity
#define REG_WATER_MODE1_M_CUTOFF_RATIO		0x028A		// Water Mode 1 Cutoff Ratio
#define REG_WATER_MODE1_SW_MIN_FIR_COEF		0x028B		// water 1 mode pt iir mul
#define REG_WATER_MODE1_CHECK_S_MU_RATIO_FLAG	0x028C		// BIT1 : y self 사용 , BIT0 : x self 사용
#define REG_WATER_MODE1_M_S_X_RATIO		0x028D		//self x ratio
#define REG_WATER_MODE1_M_S_Y_RATIO		0x028E		//self y ratio
#define REG_WATER_MODE1_M_CUTOFF_WIDTH		0x028F
#define REG_WATER_M_ADHESION_RATIO		0x27D
#define REG_WATER_S_ADHESION_RATIO		0x27C

//#define	REG_WATER_INTERNAL_FLAG_00		0x27A
//#define	REG_WATER_INTERNAL_FLAG_01		0x27B

#define REG_WATER_ENTER_MODE2_MU_FLATNESS		0x0290		// water check : enter 2 mutual flatness max

#define REG_WATER_MODE2_SUPPORT_FINGER_CNT		0x0292		// Self Compenstion Ratio : recommend : 1200

#define REG_WATER_MODE2_M_SENSITIVITYTH		0x0293		// water mode 2 sensitivity
#define REG_WATER_MODE_SX_SENSITIVITY_TH		0x0294
#define REG_WATER_MODE_SY_SENSITIVITY_TH		0x0295
#define	REG_WATER_MODE_BTN_S_SENSITIVITY_TH		0x0296
#define REG_WATER_MODE2_M_CUTOFF_RATIO		0x0297		// Water Mode 2 Cutoff Ratio
#define REG_WATER_MODE_S_CUTOFF_RATIO		0x0298		// Water Mode 2 Cutoff Ratio

#define REG_WATER_MODE2_SW_MIN_FIR_COEF		0x0299		// water 2 mode pt iir mul

#define REG_WATER_MODE2_CHECK_S_MU_RATIO_FLAG	0x029A		// BIT1 : y self 사용 , BIT0 : x self 사용
#define REG_WATER_MODE2_M_S_X_RATIO		0x029B		//self x ratio
#define REG_WATER_MODE2_M_S_Y_RATIO		0x029C		//self y ratio

#define REG_WATER_MODE_SMALL_PALM_TH		0x029D
#define REG_WATER_MODE_LARGE_PALM_TH		0x029E
#define REG_WATER_MODE_OUT_TIMEOUT_TH		0x029F

#define	REG_WATER_RUNTIME_VAL_TH		0x27E
#define	REG_WATER_RUNTIME_LEN_TH		0x27F
#define	REG_WATER_RUNTIME_M_FLATNESS_TH		0x25F

#define REG_WATER_SELF_COORD_LEN_TH		0x1EB
#define REG_WATER_SELF_COMP_RATIO		0x1EC


#if SUPPORT_PALM_UP_SENSITIVITY
#define	REG_PALM_NODE_VAL_TH		0x2BC
#define	REG_PALM_NODE_CNT			0x2BD
#define	REG_PALM_UP_SENSITIVITY_RM	0x2BE
#endif

#define REG_DEBUG_7FF	0x07FF

// Gesture Reg
#define	REG_GESTURE_MOTION_RESULT			0x0126
#define REG_GESTURE_STATUS                  0x0127
#define REG_GESTURE_WAKEUP_MODE             0x0128

#define REG_GESTURE_MOTION_TIME             0x0129
#define REG_GESTURE_TOTAL_DISTANCE          0x012A
#define REG_GESTURE_MOVE_TIME               0x013B

#define REG_GESTURE_LIMIT_DISTANCE			0x012D  // 10
#define REG_GESTURE_DOUBLE_LIMIT_TIME		0x012F  // 25
#define REG_GESTURE_MOVE_LIMIT_TIME			0x0130  // 2
#define REG_GESTURE_DIRECTION_DISTANCE      0x0131  // 50
#define REG_GESTURE_HIGH_LIMIT_DISTANCE		0x0132  // 200

#define REG_LCD_OFF_ACTIVE_DELAY         0x0133
#define REG_LCD_OFF_IDLE_WAKEUP_SX_TH         0x0134
#define REG_LCD_OFF_IDLE_WAKEUP_SY_TH         0x0138
#define REG_LCD_OFF_IDLE_WAKEUP_M_TH         0x0139
#define REG_SAMSUMG_TEST					0x013A
#define REG_GESTURE_DEBUG_00				0x013C

#define REG_GESTURE_INIT_DETECT_ACTIVE_TIME		0x013D


#define REG_TEST_SLEEP_X_LIMIT				0x013E
#define REG_TEST_SLEEP_Y_LIMIT				0x013F

// Inttime 0
#define REG_GOOD_INTTIME0			0x1E2
#define REG_CUR_INTTIME0_SAMPLING_COUNT		0x1E3
#define REG_CUR_INTTIME0			0x1E4

#define REG_INTTIME0_SCAN_TRIGGER_VARIATION	0x1E5	// sensitivity의 50%
#define REG_INTTIME0_SCAN_RANGE		0x1E6	//+- 5
#define REG_INTTIME0_SCAN_SAMPLE_COUNT	0x1E7
#define REG_INTTIME0_NEXT_SCAN_PERIOD		0x1E8	// 5sec


// TM MODE
//===========================================
#define REG_TM_AFE_FREQUENCY				0x300
#define REG_TM_INTERNAL_FLAG				0x301
#define REG_TM_SENSITIVITY					0x302
#define REG_TM_Y_SHORT						0x303
#define REG_TM_Y_SHORT_MAX					0x303
#define REG_TM_MANUAL_POINT_AREA			0x304
#define REG_TM_MANUAL_POINT_RATIO			0x305
#define REG_TM_SOFT_CALIBARTION_COUNT		0x306	//----------------------------------------------------------no use
#define REG_TM_INTERNAL_FLAG_01				0x307
#define REG_TM_VIEW_EDGE_SCALE_FACTOR		0x308
#define REG_TM_VIEW_EDGE_XF_OFFSET			0x309
#define REG_TM_VIEW_EDGE_XL_OFFSET			0x30A
#define REG_TM_VIEW_EDGE_YF_OFFSET			0x30B
#define REG_TM_VIEW_EDGE_YL_OFFSET			0x30C
#define REG_TM_FIR_COEFFICIENT				0x30D
#define TM_CAL_DIFFERENTIAL_TX_SHORT		0x30E	//width에 따라 manual coord를 사용할 지 결정

#define REG_TM_SHORT_HIGH_FREQUENCY			0x2FE
#define REG_TM_SHORT_LOW_FREQUENCY			0x2FF

#define REG_TM_WIDTH_AREA_COEF				0x30F
#define REG_TM_BUTTON_SENSITIVITY			0x310
#define REG_TM_REACTION_COUNT				0x311
#define REG_TM_BUTTON_REACTION_CNT			0x312
#define REG_TM_BDATA_COEF1					0x313
#define REG_TM_BDATA_COEF2					0x314
#define REG_TM_BDATA_COEF3					0x315

#define REG_BUTTON_WIDTH					0x316

#define REG_TM_BTN_WEIGHT_Z					0x31E
#define REG_TM_WEIGHT_Z						0x31F
#define REG_TM_WEIGHT_Z_GAIN				0x320
#define REG_TM_X_SHORT						0x321
#define REG_TM_X_SHORT_MAX					0x321
#define REG_TM_REFERENCE_COUNT				0x322
//#define REG_X_SHORT_ORDER					0x17A
//#define REG_TM_REFERENCE_COORD				0x1D4

#define REG_TM_VIEW_XF_OFFSET				0x3A0
#define REG_TM_VIEW_XL_OFFSET				0x3A1
#define REG_TM_VIEW_YF_OFFSET				0x3A2
#define REG_TM_VIEW_YL_OFFSET				0x3A3
#define REG_TM_INTERNAL_FLAG_02				0x3A4
#define REG_TM_INTERNAL_FLAG_03				0x3A5

#define REG_REAL_WIDTH						0x3A6
#define REG_TM_REFERENCE_COORD				0x323
#endif

//short
#define REG_TM_SHORT_METHOD				0x0285		// if set to 1, for self short(bit 0 : TX,  bit 1 : RX)
#define REG_TM_SHORT_TX_FREQUENCY		0x0286
#define REG_TM_SHORT_TX_HFREQUENCY		0x0287
#define REG_TM_SHORT_TX_N_COUNT			0x0288
#define REG_TM_SHORT_TX_U_COUNT			0x0289
#define REG_TM_SHORT_TX_RST0_TIME		0x028A
#define REG_TM_SHORT_TX_RBG_SEL			0x028B
#define REG_TM_SHORT_TX_AMP_V_SEL		0x028C
#define REG_TM_SHORT_TX_SUB_V_SEL		0x028D
#define REG_TM_SHORT_RX_FREQUENCY		0x028E
#define REG_TM_SHORT_RX_HFREQUENCY		0x028F
#define REG_TM_SHORT_RX_N_COUNT			0x0290
#define REG_TM_SHORT_RX_U_COUNT			0x0291
#define REG_TM_SHORT_RX_RST0_TIME		0x0292
#define REG_TM_SHORT_RX_RBG_SEL			0x0293
#define REG_TM_SHORT_RX_AMP_V_SEL		0x0294
#define REG_TM_SHORT_RX_SUB_V_SEL		0x0295

//===========================================
// AFE Setting

#define REG_DND_FREQUENCY					0x03F7
#define REG_DND_N_COUNT						0x03F8
#define REG_DND_U_COUNT						0x03F9

#define REG_HF_DND_FREQUENCY				0x03FA
#define REG_HF_DND_N_COUNT					0x03FB
#define REG_HF_DND_U_COUNT					0x03FC

#define REG_SELF_DND_FREQUENCY_A			0x03E7
#define REG_SELF_DND_N_COUNT_A				0x03E8
#define REG_SELF_DND_U_COUNT_A				0x03E9

#define REG_SELF_DND_FREQUENCY_B			0x03EE
#define REG_SELF_DND_N_COUNT_B				0x03EF
#define REG_SELF_DND_U_COUNT_B				0x03F0

#define REG_SELF_SATURATION_FREQUENCY_A		0x03E0
#define REG_SELF_SATURATION_N_COUNT_A		0x03E1
#define REG_SELF_SATURATION_U_COUNT_A		0x03E2

#define REG_SELF_SATURATION_FREQUENCY_B		0x03D0
#define REG_SELF_SATURATION_N_COUNT_B		0x03D1
#define REG_SELF_SATURATION_U_COUNT_B		0x03D2

#define REG_DEFAULT_FREQUENCY				REG_AFE_FREQUENCY
#define REG_DEFAULT_N_COUNT					REG_CFG_M_N_COUNT
#define REG_DEFAULT_U_COUNT					REG_CFG_M_U_COUNT

#define REG_DEFAULT_N_COUNT_CHINA			REG_AFE_DEFAULT_N
#define REG_DEFAULT_U_COUNT_CHINA			REG_AFE_U_COUNT_N

// for virtual draw test
#define	REG_TM_DRAW_MODE_TEST_RESULT			0x0296
#define	REG_TM_DRAW_MODE_TEST_DESCRIPTION_1		0x0297
#define	REG_TM_DRAW_MODE_TEST_DESCRIPTION_2		0x0298
#define	REG_TM_DRAW_MODE_TEST_DESCRIPTION_3		0x0299
#define	REG_TM_DRAW_MODE_TEST_DESCRIPTION_4		0x029A

#endif

