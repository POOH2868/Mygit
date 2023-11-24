
// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// MFCApplication1.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

#define _GetTargetName VER2(TARGET_NAME)
#define VER2(x) VER3(x)
#define VER3(a) #a

CString g_strZtdVersion = "";

CString GetTargetName()
{
	CString strTargetName = (CString)_GetTargetName;

	int nIndex = strTargetName.ReverseFind('_');

	if (nIndex != -1)
	{
		g_strZtdVersion = strTargetName.Right(strTargetName.GetLength() - nIndex - 1);
	}

	return strTargetName;
}

#ifdef _DEBUG
bool _trace(TCHAR *format, ...)
{
	TCHAR buffer[1000];

	va_list argptr;
	va_start(argptr, format);
	vsprintf(buffer, format, argptr);
	va_end(argptr);

	OutputDebugString(buffer);

	return true;
}

bool _trace(CString strFormat, ...)
{
	TCHAR *format = (TCHAR*)(LPCTSTR)strFormat;
	TCHAR buffer[1000];

	va_list argptr;
	va_start(argptr, format);
	vsprintf(buffer, format, argptr);
	va_end(argptr);

	OutputDebugString(buffer);

	return true;
}
#endif


