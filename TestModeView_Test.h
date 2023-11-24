// TestModeView_Test.h : 선언 파일입니다.
//

#if !defined(ZT_TEST_MODE_VIEW_TEST_INCLUDED_)
#define ZT_TEST_MODE_VIEW_TEST_INCLUDED_

// Test result value
#define Z_TEST_OK						1
#define Z_TEST_ERR						0
#define Z_TEST_ERR_SPEC_OVER			-1

// List properties
#define IMAGE_LIST_DISABLE		0
#define IMAGE_LIST_ENABLE		1

#define PASS_IMAGE_LIST			-1
#define FAIL_IMAGE_LIST			1

#define FIRST_DEVICE_INDEX		(3)		// 2

#define	STRING_NOT_TEST			"Not Test"
#define	STRING_RESERVED_ITEM	"RESERVED_ITEM"

enum
{
	TEST_DETECTED_DEVICE = 0,
	TEST_FW_DOWNLOAD,
	TEST_CHECK_INT,
	TEST_CHECK_VERSION,

	TEST_DND,						// MAX, MIN
	TEST_DND_GAP,					// SPEC_OVER
	TEST_HF_DND,					// MAX, MIN
	TEST_HF_DND_GAP,				// SPEC_OVER
	TEST_SELF_DND,					// MAX, MIN
	TEST_SELF_DND_GAP,				// SPEC_OVER - display as TX, RX
	TEST_SELF_SATURATION,			// MAX, MIN
	TEST_TRX_SHORT,					// MAX, MIN
	TEST_CURRENT_CONSUMPTION,

	TEST_JITTER,					// SPEC_OVER
	TEST_HW_CALIBRATION,			// MAX, MIN
	TEST_MEASURE_DELTA,				// MAX,	MIN

	TEST_DIGITAL_WHEEL,				// MAX, MIN

#if defined(D_USE_CHECK_BUTTON_TEST)
	TEST_GPIO_BUTTON,
	TEST_TOUCH_BUTTON,
#endif

	TEST_NOISE,						// MAX, MIN
	TEST_ONLY_SELF_DND,				// MAX, MIN
	TEST_HIGH_VOLTAGE,				// MAX,MIN

	TEST_CND,						// MAX, MIN	//add 2022.11.14 for ZTM730 CND
	TEST_VIRTUAL_DRAW,

	TEST_MAX
};

enum
{
	TEST_STEP_CHECK_DATA = 0,
	TEST_STEP_CHECK_H_DIFF,
	TEST_STEP_CHECK_V_DIFF
};

enum
{
	CURRENT_CONSUMPTION_ACTIVE = 0,
	CURRENT_CONSUMPTION_IDLE,
	CURRENT_CONSUMPTION_SLEEP,
	CURRENT_CONSUMPTION_DEEP_SLEEP,

	CURRENT_CONSUMPTION_MAX
};


//---------------------------------------------------------------------------------
// TEST INFO

typedef	struct _T_AFE_SETTING
{
	BOOL 	bUseAfeSetting;

	int  	nFrequency;
	int  	nNCount;
	int  	nUCount;

	// NOISE CHECK
	BOOL 	m_bCheckFrequency1;
	BOOL 	m_bCheckFrequency2;
	BOOL 	m_bCheckFrequency3;
	BOOL 	m_bCheckFrequency4;

	int  	nFrequency1;
	int  	nFrequency2;
	int  	nFrequency3;
	int  	nFrequency4;

	BOOL	bPrintPpVariation;
}T_AFE_SETTING;

typedef	struct _T_AFE_SETTING_MULTI
{
	BOOL 	bUseAfeSetting;
	int  	nFrequency_A;
	int  	nNCount_A;
	int  	nUCount_A;

	int  	nFrequency_B;
	int  	nNCount_B;
	int  	nUCount_B;
}T_AFE_SETTING_MULTI;

typedef	struct _T_AFE_VALUE
{
	u16 reg_Frequency;
	u16 reg_Ncount;
	u16 reg_Ucount;

	s16 val_Frequency;
	s16 val_Ncount;
	s16 val_Ucont;

	u16 reg_Frequency_B;
	u16 reg_Ncount_B;
	u16 reg_Ucount_B;

	s16 val_Frequency_B;
	s16 val_Ncount_B;
	s16 val_Ucont_B;
}T_AFE_VALUE;


typedef	struct _TEST_INFO_DND
{
	BOOL 	bCheckData;
	int  	nDataMin;			// ALL NODE
	int  	nDataMax;			// ALL NODE

	BOOL 	bCheckDataDiff;
	int  	nVDiffCenter;		// ALL NODE
	int  	nHDiffCenter;		// ALL NODE

	// SPEC
	int 	pDataMin[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];
	int 	pDataMax[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];
	int 	pHDiff[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];
	int 	pVDiff[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];

	// AFE
	T_AFE_SETTING	AfeInfo;
}TEST_INFO_DND;

// CND	//2022.11.14 for ZTM730 CND
typedef	struct _TEST_INFO_CND
{
	BOOL 	bCheckData;
	int  	nDataMin;			// ALL NODE
	int  	nDataMax;			// ALL NODE

	// SPEC
	int 	pDataMin[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];
	int 	pDataMax[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];

	//TEST_CND 에서는 TEST_DND 와는 다르게 reg_Frequency,reg_Ncount,reg_Ucount 부분 변경을 하지 않는다.
	T_AFE_SETTING	AfeInfo;	//실제로는 사용하지 않음.
}TEST_INFO_CND;

typedef	struct _TEST_INFO_SELF
{
	BOOL 	bCheckData;

	int  	nTxDataMin;			// ALL NODE
	int  	nTxDataMax;			// ALL NODE
	int  	nRxDataMin;			// ALL NODE
	int  	nRxDataMax;			// ALL NODE

	BOOL 	bCheckDataDiff;
	int  	nTxHDiffCenter;		// ALL NODE
	int  	nRxHDiffCenter;		// ALL NODE

	// SPEC
	int 	pTxDataMin[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];
	int 	pTxDataMax[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];
	int 	pTxHDiff[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];

	int 	pRxDataMin[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];
	int 	pRxDataMax[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];
	int 	pRxHDiff[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];

	// AFE
	T_AFE_SETTING_MULTI		AfeInfo;
}TEST_INFO_SELF;

typedef	struct _TEST_INFO_ONLYSELF
{
	BOOL 	bCheckData;

	int  	nTxDataMin;			// ALL NODE
	int  	nTxDataMax;			// ALL NODE
	int  	nRxDataMin;			// ALL NODE
	int  	nRxDataMax;			// ALL NODE

	// SPEC
	int 	pTxDataMin[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];
	int 	pTxDataMax[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];

	int 	pRxDataMin[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];
	int 	pRxDataMax[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];
}TEST_INFO_ONLYSELF;

typedef	struct _TEST_INFO_HIGH_VOLTAGE
{
	BOOL 	bCheckData;

	int 	pDataMax;
	int 	pDataMin;

/*
	//SPEC
	int 	pDataMin[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];
	int 	pDataMax[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];
*/
}TEST_INFO_HIGH_VOLTAGE;

typedef	struct _TEST_INFO_MEASURE_DELTA
{
	BOOL 	bCheckData;
	BOOL	bCheckAutoStart;

	int 	nAutoStartThreshold;
	int 	nCount;					// frame count

	int 	nDataMax;
	int 	nDataMin;

	int 	m_nSelfRxDataMax;
	int 	m_nSelfRxDataMin;
	int 	m_nSelfTxDataMax;
	int 	m_nSelfTxDataMin;

	// SPEC
	int 	pDataMin[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];
	int 	pDataMax[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];
	int 	pSelfRxDataMin[DEF_MAX_Y_LINE_NUM];
	int 	pSelfRxDataMax[DEF_MAX_Y_LINE_NUM];
	int 	pSelfTxDataMin[DEF_MAX_X_LINE_NUM];
	int 	pSelfTxDataMax[DEF_MAX_X_LINE_NUM];

	// AFE
	T_AFE_SETTING	AfeInfo;
}TEST_INFO_MEASURE_DELTA;

typedef	struct _TEST_INFO_HW_CALIBRATION
{
	BOOL 	bCheckData;
	int 	nHwCalibrationMax;

	// AFE
	T_AFE_SETTING		AfeInfo;
}TEST_INFO_HW_CALIBRATION;

typedef	struct _TEST_INFO_JITTER
{
	BOOL 	bCheckData;
	int 	nJitterDataMax;

	BOOL	bCheckDataByMpztd;
	int 	nJitterReadCount;

	// AFE
	T_AFE_SETTING		AfeInfo;
}TEST_INFO_JITTER;

typedef	struct _TEST_INFO_CURRENT_CONSUMPTION_ITEM
{
	BOOL 	bCheckData;
	int 	nThreshould_Max;
	int 	nThreshould_Min;
	int 	nAvgMs;
}TEST_CURRENT_CONSUMPTION_ITEM;

typedef	struct _TEST_INFO_CURRENT_CONSUMPTION
{
	BOOL 	bEnable;				// MPTool REV_03? REV_05?

	TEST_CURRENT_CONSUMPTION_ITEM 	Active;

	TEST_CURRENT_CONSUMPTION_ITEM	Idle;
	TEST_CURRENT_CONSUMPTION_ITEM	Sleep;
	TEST_CURRENT_CONSUMPTION_ITEM	DeepSleep;
}TEST_INFO_CURRENT_CONSUMPTION;

typedef	struct _TEST_INFO_DIGITAL_WHEEL
{
	BOOL 	bCheckData;
	int  	nDataMin;			// ALL NODE
	int  	nDataMax;			// ALL NODE

	// SPEC
	int 	pDataMin[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];
	int 	pDataMax[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];
}TEST_INFO_DIGITAL_WHEEL;


typedef	struct _TEST_INFO_NOISE
{
	BOOL m_bCheckNoise;
	BOOL m_bCheckNoiseUserAFE;
	int m_nCheckNoise_Threshold;
	int m_nCheckNoise_ReadTimes;
	int m_nCheckNoise_Interval;

	int m_nCheckNoise_ReadTime_Org;
	int m_nCheckNoise_ReadTimes_ScalerCoef;

	// AFE
	T_AFE_SETTING	AfeInfo;
}TEST_INFO_NOISE;

#endif	// #if !defined(ZT_TEST_MODE_VIEW_TEST_INCLUDED_)

