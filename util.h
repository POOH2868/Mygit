#ifndef _MY_UTIL_H
#define	_MY_UTIL_H

#include "global.h"
#include ".\CustomUI\CustomButton\ShadeButtonST.h"

#define BIT_SET(val, n)		((val) &= ~(1<<(n)), (val) |= (1<<(n)))
#define BIT_CLR(val, n)		((val) &= ~(1<<(n)))
#define BIT_TEST(val, n)	((val) & (1<<(n)))
#define SWAP_V(a, b, t)		((t) = (a), (a) = (b), (b) = (t))
#define SWAP_16(s) 			(((((s) & 0xff) << 8) | (((s) >> 8) & 0xff)))

#define ZT_DELETE_OBJ(a) if(a) delete(a); a = NULL;

#define S16_MINUS_MAX	-32768
#define S16_PLUS_MAX	32767

int char2dec(char *operand);
BOOL buf_char2dec(char *operand, int *retVal);
int buf_sscanf(char *buf, char *dstbuf);

inline void DividedSleep(DWORD dwMillisecond);
inline BOOL SemaLongWait(HANDLE hHandle);
BOOL SemaWaitImmediate(HANDLE hHandle);

BOOL SemaWait(HANDLE hHandle);

void MyDelayMs(DWORD dwMillisecond);
#define MySleep MyDelayMs

#if 0
void ShowCalculatedRegInfo(u16 nIcSignature, s16 * pIcRegData, int nIdx, int nVal);
CString GetFrequency(u16 nIcSignature, s16 * pIcRegData, int nVal, int *nFreq);
int GetIntCap(u16 nIcSignature, int nVal);
int GetValFromFrequency(u16 nIcSignature, s16 * pIcRegData, int nFreq);
#endif

CString	GetCurrentAppDirectory();
BYTE hex2byte(char hex);
//BOOL CmdRun(LPSTR lpszCmd, LPSTR *lpszBuffer);
void MyMicroDelay(DWORD dwMicrosecond);
//extern void (*pCallbackFunc)();
//void SetCallbackFunc(LPVOID lpCallBackFunc);
BOOL IsDigit(char d);

UINT64 GetTimeInMillis(void);

#if(0)
int GetTotalRegisterCount();
int GetTotalGroupCount();
#endif
void Util_SetIcSignature(u16 icSignature);

int GetTsmVersion(char * cDevicePath);
int GetTsmId(char * cDevicePath);
int GetDevSerial(char * cDevicePath);

int GetFileSize(CString strFileName);

char * hvFillBlankSpaceInString(int nDevIndex, const CString & pstrSrc, int nMaxSpace, char cFillChar);
char * hvFillBlankSpaceInString_0(int nDevIndex, const CString & pstrSrc);
char * hvItoaWithSpace(int nDevIndex, long lValue, int nMaxSpace);

void SetButtonConfig(CShadeButtonST * pButton);
void ChangeCheckboxBgColor(CDC* pDC, BOOL bChecked);

unsigned short Str_to_Hex(unsigned char *string,unsigned char len);

void XlineFilter(s16 nThd, s16 ratio, s16 *BUFF, s16 nRow, s16 nCol);

#if defined (HV_USE_FILE_DEBUG)
void HV_TRACE_INITIALIZE(CString strSaveDirectory);
void HV_TRACE_FINALIZE();
void HV_TRACE(TCHAR* fmt, ...);
#else
#define HV_TRACE_INITIALIZE(s)	((void)0)
#define HV_TRACE_FINALIZE()		((void)0)

#ifdef _DEBUG
#define HV_TRACE(s, ...)		(TRACE(s, __VA_ARGS__))
#else
#define HV_TRACE(s, ...)		((void)0)
#endif	// #ifdef _DEBUG
#endif	// #if defined (HV_USE_FILE_DEBUG)

#endif	// #ifndef _MY_UTIL_H

