
#include "stdafx.h"
#include "shlwapi.h"
#include "util.h"
#include "global.h"
#include "resource.h"

#define		BIN				2
#define		DEC				10
#define		HEX				16
#define		WRONG_DIGIT		-1

#if 0
float FREQ_Table_7554[256] = {
	6.9, 7.1, 7.3, 7.6, 7.8, 8.0, 8.3, 8.5, 8.7, 8.9, 9.2,
	9.4, 9.6, 9.9, 10.1, 10.3, 10.5, 10.8, 11.0, 11.2, 11.4,
	11.7, 11.9, 12.1, 12.3, 12.6, 12.8, 13.0, 13.2, 13.4, 13.7,
	13.9, 14.1, 14.3, 14.5, 14.7, 15.0, 15.2, 15.4, 15.6, 15.8,
	16.0, 16.3, 16.5, 16.7, 16.9, 17.1, 17.3, 17.5, 17.7, 18.0,
	18.2, 18.4, 18.6, 18.8, 19.0, 19.2, 19.4, 19.6, 19.8, 20.0,
	20.3, 20.5, 20.7, 20.9, 21.1, 21.3, 21.5, 21.7, 21.9, 22.1,
	22.3, 22.5, 22.7, 22.9, 23.1, 23.3, 23.5, 23.7, 23.9, 24.1,
	24.3, 24.5, 24.7, 24.9, 25.1, 25.3, 25.5, 25.7, 25.9, 26.1,
	26.3, 26.5, 26.7, 26.9, 27.1, 27.3, 27.5, 27.6, 27.8, 28.0,
	28.2, 28.4, 28.6, 28.8, 29.0, 29.2, 29.4, 29.6, 29.8, 30.0,
	30.1, 30.3, 30.5, 30.7, 30.9, 31.1, 31.3, 31.5, 31.7, 31.8,
	32.0, 32.2, 32.4, 32.6, 32.8, 33.0, 33.1, 33.3, 33.5, 33.7,
	33.9, 34.1, 34.2, 34.4, 34.6, 34.8, 35.0, 35.2, 35.3, 35.5,
	35.7, 35.9, 36.1, 36.2, 36.4, 36.6, 36.8, 37.0, 37.1, 37.3,
	37.5, 37.7, 37.9, 38.0, 38.2, 38.4, 38.6, 38.8, 38.9, 39.1,
	39.3, 39.5, 39.6, 39.8, 40.0, 40.2, 40.3, 40.5, 40.7, 40.9,
	41.0, 41.2, 41.4, 41.6, 41.7, 41.9, 42.1, 42.2, 42.4, 42.6,
	42.8, 42.9, 43.1, 43.3, 43.5, 43.6, 43.8, 44.0, 44.1, 44.3,
	44.5, 44.6, 44.8, 45.0, 45.2, 45.3, 45.5, 45.7, 45.8, 46.0,
	46.2, 46.3, 46.5, 46.7, 46.8, 47.0, 47.2, 47.3, 47.5, 47.7,
	47.9, 48.0, 48.2, 48.4, 48.5, 48.7, 48.8, 49.0, 49.2, 49.3,
	49.5, 49.7, 49.8, 50.0, 50.2, 50.3, 50.5, 50.7, 50.8, 51.0,
	51.2, 51.3, 51.5, 51.7, 51.8, 52.0, 52.1, 52.3, 52.5, 52.6,
	52.8, 53.0, 53.1, 53.3, 53.5, 53.6, 53.8, 53.9, 54.1, 54.3,
	54.4, 54.6, 54.7, 54.9, 55.1
};

float FREQ_Table_75XX[256] = {		//ZT7548 & ZT7538 &	ZT7532
	6.4, 6.6, 6.9, 7.1, 7.3, 7.5, 7.7, 8.0, 8.2, 8.4, 8.6,
	8.8, 9.1, 9.3, 9.5, 9.7, 9.9, 10.2, 10.4, 10.6, 10.8,
	11.0, 11.3, 11.5, 11.7, 11.9, 12.1, 12.3, 12.6, 12.8, 13.0,
	13.2, 13.4, 13.6, 13.9, 14.1, 14.3, 14.5, 14.7, 14.9, 15.1,
	15.3, 15.6, 15.8, 16.0, 16.2, 16.4, 16.6, 16.8, 17.0, 17.3,
	17.5, 17.7, 17.9, 18.1, 18.3, 18.5, 18.7, 18.9, 19.1, 19.4,
	19.6, 19.8, 20.0, 20.2, 20.4, 20.6, 20.8, 21.0, 21.2, 21.4,
	21.6, 21.8, 22.0, 22.3, 22.5, 22.7, 22.9, 23.1, 23.3, 23.5,
	23.7, 23.9, 24.1, 24.3, 24.5, 24.7, 24.9, 25.1, 25.3, 25.5,
	25.7, 25.9, 26.1, 26.3, 26.5, 26.7, 26.9, 27.1, 27.3, 27.5,
	27.7, 27.9, 28.1, 28.3, 28.5, 28.7, 28.9, 29.1, 29.3, 29.5,
	29.7, 29.9, 30.1, 30.3, 30.5, 30.7, 30.9, 31.1, 31.3, 31.5,
	31.7, 31.9, 32.1, 32.3, 32.5, 32.7, 32.9, 33.1, 33.3, 33.5,
	33.7, 33.9, 34.1, 34.3, 34.5, 34.7, 34.9, 35.0, 35.2, 35.4,
	35.6, 35.8, 36.0, 36.2, 36.4, 36.6, 36.8, 37.0, 37.2, 37.4,
	37.6, 37.8, 38.0, 38.1, 38.3, 38.5, 38.7, 38.9, 39.1, 39.3,
	39.5, 39.7, 39.9, 40.1, 40.3, 40.5, 40.6, 40.8, 41.0, 41.2,
	41.4, 41.6, 41.8, 42.0, 42.2, 42.4, 42.5, 42.7, 42.9, 43.1,
	43.3, 43.5, 43.7, 43.9, 44.1, 44.3, 44.4, 44.6, 44.8, 45.0,
	45.2, 45.4, 45.6, 45.8, 46.0, 46.2, 46.3, 46.5, 46.7, 46.9,
	47.1, 47.3, 47.5, 47.7, 47.9, 48.0, 48.2, 48.4, 48.6, 48.8,
	49.0, 49.2, 49.4, 49.6, 49.7, 49.9, 50.1, 50.3, 50.5, 50.7,
	50.9, 51.1, 51.2, 51.4, 51.6, 51.8, 52.0, 52.2, 52.4, 52.6,
	52.8, 53.0, 53.1, 53.3, 53.5, 53.7, 53.9, 54.1, 54.3, 54.5,
	54.7, 54.9, 55.0, 55.2, 55.4, 55.6, 55.8, 56.0, 56.2, 56.4,
	56.6, 56.8, 57.0, 57.1, 57.3
};

float FREQ_Table_E628[256] = {			// ZT2628 / W523 / M620 ==> 0xE628
	9.9, 10.1, 10.3, 10.5, 10.6, 10.8, 11.0, 11.2, 11.4, 11.6,		//000~009
	11.8, 12.0, 12.2, 12.4, 12.6, 12.8, 13.0, 13.2, 13.4, 13.6,		//010~019
	13.8, 14.0, 14.1, 14.3, 14.5, 14.7, 14.9, 15.1, 15.3, 15.5,		//020~029
	15.7, 15.8, 16.0, 16.2, 16.4, 16.6, 16.8, 17.0, 17.2, 17.4,		//030~039
	17.6, 17.7, 17.9, 18.1, 18.3, 18.5, 18.7, 18.9, 19.1, 19.2,		//040~049
	19.4, 19.6, 19.8, 20.0, 20.2, 20.3, 20.6, 20.7, 20.9, 21.1,		//050~059
	21.3, 21.5, 21.6, 21.9, 22.0, 22.2, 22.4, 22.5, 22.7, 22.9,		//060~069
	23.1, 23.3, 23.4, 23.6, 23.8, 24.0, 24.1, 24.3, 24.5, 24.7,		//070~079
	24.8, 25.0, 25.2, 25.4, 25.6, 25.8, 26.0, 26.1, 26.3, 26.5,		//080~089
	26.6, 26.9, 27.0, 27.2, 27.4, 27.6, 27.7, 27.9, 28.0, 28.2,		//090~099
	28.4, 28.6, 28.7, 28.9, 29.1, 29.3, 29.4, 29.6, 29.8, 29.9,		//100~109
	30.1, 30.2, 30.5, 30.6, 30.8, 31.0, 31.2, 31.3, 31.5, 31.6,		//110~119
	31.8, 32.0, 32.2, 32.3, 32.5, 32.7, 32.9, 33.0, 33.2, 33.3,		//120~129
	33.5, 33.7, 33.8, 34.0, 34.2, 34.4, 34.5, 34.7, 34.9, 35.0,		//130~139
	35.2, 35.4, 35.5, 35.7, 35.8, 36.0, 36.2, 36.4, 36.5, 36.7,		//140~149
	36.9, 37.1, 37.2, 37.4, 37.6, 37.7, 37.8, 38.0, 38.2, 38.4,		//150~159
	38.5, 38.7, 38.8, 39.0, 39.2, 39.4, 39.5, 39.7, 39.8, 40.0,		//160~169
	40.1, 40.3, 40.5, 40.6, 40.8, 41.0, 41.1, 41.3, 41.5, 41.6,		//170~179
	41.8, 41.9, 42.1, 42.2, 42.4, 42.6, 42.7, 42.8, 43.0, 43.2,		//180~189
	43.3, 43.5, 43.7, 43.8, 44.0, 44.1, 44.3, 44.5, 44.6, 44.7,		//190~199
	44.9, 45.1, 45.2, 45.4, 45.5, 45.7, 45.9, 46.0, 46.1, 46.3,		//200~209
	46.4, 46.6, 46.8, 46.9, 47.1, 47.2, 47.4, 47.5, 47.7, 47.8,		//210~219
	48.0, 48.1, 48.3, 48.4, 48.5, 48.7, 48.8, 49.0, 49.1, 49.3,		//220~229
	49.4, 49.6, 49.7, 49.8, 50.0, 50.2, 50.3, 50.5, 50.7, 50.8,		//230~239
	50.9, 51.2, 51.2, 51.4, 51.6, 51.7, 51.9, 52.1, 52.2, 52.4,		//240~249
	52.5, 52.6, 52.8, 53.6, 54.3, 53.3								//250~255
};

float FREQ_Table_E622[256] = {			// ZTW622 = ZT2717
	9.86, 10.06, 10.25, 10.45, 10.65, 10.85, 11.05, 11.24, 11.44, 11.64,  //000~009
	11.83, 12.03, 12.23, 12.42, 12.62, 12.81, 13.01, 13.20, 13.40, 13.59,  //010~019
	13.78, 13.98, 14.17, 14.36, 14.55, 14.75, 14.94, 15.13, 15.32, 15.51,  //020~029
	15.70, 15.89, 16.09, 16.27, 16.46, 16.65, 16.84, 17.03, 17.22, 17.41,  //030~039
	17.60, 17.78, 17.97, 18.16, 18.35, 18.53, 18.72, 18.90, 19.09, 19.28,  //040~049
	19.46, 19.65, 19.83, 20.02, 20.20, 20.38, 20.57, 20.75, 20.93, 21.12,  //050~059
	21.30, 21.48, 21.66, 21.84, 22.03, 22.21, 22.39, 22.57, 22.75, 22.93,  //060~069
	23.11, 23.29, 23.47, 23.65, 23.83, 24.00, 24.18, 24.36, 24.54, 24.72,  //070~079
	24.89, 25.07, 25.25, 25.42, 25.60, 25.77, 25.95, 26.13, 26.30, 26.48,  //080~089
	26.65, 26.83, 27.00, 27.17, 27.35, 27.52, 27.69, 27.87, 28.04, 28.21,  //090~099
	28.38, 28.56, 28.73, 28.90, 29.08, 29.24, 29.41, 29.59, 29.76, 29.93,  //100~109
	30.10, 30.27, 30.44, 30.61, 30.78, 30.94, 31.11, 31.28, 31.45, 31.62,  //110~119
	31.79, 31.95, 32.12, 32.29, 32.46, 32.62, 32.79, 32.96, 33.12, 33.29,  //120~129
	33.46, 33.62, 33.79, 33.95, 34.12, 34.28, 34.45, 34.61, 34.77, 34.94,  //130~139
	35.10, 35.27, 35.43, 35.59, 35.76, 35.92, 36.08, 36.24, 36.41, 36.57,  //140~149
	36.73, 36.89, 37.06, 37.22, 37.38, 37.54, 37.70, 37.86, 38.02, 38.18,  //150~159
	38.34, 38.50, 38.66, 38.82, 38.98, 39.14, 39.30, 39.46, 39.62, 39.78,  //160~169
	39.93, 40.09, 40.25, 40.41, 40.57, 40.72, 40.88, 41.04, 41.19, 41.35,  //170~179
	41.51, 41.66, 41.82, 41.98, 42.13, 42.29, 42.45, 42.60, 42.76, 42.91,  //180~189
	43.07, 43.22, 43.38, 43.53, 43.69, 43.84, 44.00, 44.15, 44.30, 44.46,  //190~199
	44.61, 44.76, 44.92, 45.07, 45.22, 45.38, 45.53, 45.68, 45.83, 45.98,  //200~209
	46.13, 46.29, 46.44, 46.59, 46.74, 46.89, 47.05, 47.20, 47.35, 47.50,  //210~219
	47.65, 47.80, 47.95, 48.10, 48.25, 48.40, 48.55, 48.70, 48.85, 49.00,  //220~229
	49.14, 49.29, 49.44, 49.59, 49.74, 49.89, 50.04, 50.19, 50.33, 50.48,  //230~239
	50.63, 50.78, 50.92, 51.07, 51.22, 51.37, 51.51, 51.66, 51.80, 51.95,  //240~249
	52.10, 52.24, 52.39, 52.53, 52.68, 52.83 							   //250~255
};
#endif

int	char2dec(char *operand)
{
	int dec = 0;
	int i = 0;
	int ntype = WRONG_DIGIT;
	int mul = 1;
	int nVal = 0;
	int	minus=0;
	char	cValue[256];
	char	cOperand[256];
	memset(cValue, 0, sizeof(cValue));

	strcpy(cOperand, operand);

	int n = (int)(strlen(cOperand));
	if( n == 0 )	{
		return -1;
	}

	if( *(cOperand+0) == '0' && ((*(cOperand+1) == 'X')||(*(cOperand+1) == 'x'))) // hex (이경우엔 라벨이면 무조건 에러 라벨명앞에 0x가 올수없다.)
	{
		for(i=2; i<n; i++)	cValue[i-2] = *(cOperand+i);
		ntype = HEX;
	}
	else
	{
		if(*(cOperand+0) == '-')
		{
			minus = 1;
			for(i=1; i<n; i++)	cOperand[i-1] = *(cOperand+i);
			cOperand[i-1] = '\0';
			n--;
		}

		if( *(cOperand+n-1) == 'B' )
		{
			for(i=0; i<n-1; i++)	cValue[i] = *(cOperand+i);
			ntype = BIN;
		}
		else if( *(cOperand+n-1) == 'D' )
		{
			for(i=0; i<n-1; i++)	cValue[i] = *(cOperand+i);
			ntype = DEC;
		}
		else
		{
			strcpy(cValue, cOperand);
			ntype = DEC; // 'b','d'가 아니면 십진수로 처리
		}
	}

	switch(ntype)
	{
		case BIN: // 'B' binary
			for(i=(int)(strlen(cValue)-1); i>=0; i--)
			{
				if( cValue[i] == '0' || cValue[i] == '1' )
				{
					nVal = cValue[i] - '0';
					dec += nVal*mul;
					mul *= BIN;
				}
				else	return -1; // ERROR
			}
			break;
		case DEC: // 'D' decimal
			for(i=(int)(strlen(cValue)-1); i>=0; i--)
			{
				if( cValue[i] >= '0' && cValue[i] <= '9' )
				{
					nVal = cValue[i] - '0';
					dec += nVal*mul;
					mul *= DEC;
				}
				else	return -1;
			}
			break;
		case HEX: // 'H' hex
			for(i=(int)(strlen(cValue)-1); i>=0; i--)
			{
				if( cValue[i] >= '0' && cValue[i] <= '9' )		nVal = cValue[i] - '0';
				else if( cValue[i] >= 'A' && cValue[i] <= 'F' )	nVal = cValue[i] - 'A' + 10;
				else if( cValue[i] >= 'a' && cValue[i] <= 'f' )	nVal = cValue[i] - 'a' + 10;
				else	return -1;

				dec += nVal*mul;
				mul *= HEX;
			}
			break;
		default : return -1;
	}
	if(minus)
		dec *= -1;
	return dec;
}


int buf_sscanf(char *buf, char *dstbuf)
{
	int i;
	for(i = 0; i < 255; i++) {
		if( (buf[i] >= '0' && buf[i] <= '9') || buf[i] == '-')
			dstbuf[i] = buf[i];
		else
			break;
	}
	dstbuf[i] = 0;
	return i;
}

BOOL buf_char2dec(char *operand, int *retVal)
{
	int dec = 0;
	int i = 0;
	int ntype = WRONG_DIGIT;
	int mul = 1;
	int nVal = 0;
	int	minus=0;
	char	cValue[256];
	char	cOperand[256];
	memset(cValue, 0, sizeof(cValue));

	strcpy(cOperand, operand);

	int n = (int)(strlen(cOperand));
	if( n == 0 )	{
		return false;
	}

	if( *(cOperand+0) == '0' && ((*(cOperand+1) == 'X')||(*(cOperand+1) == 'x'))) // hex (이경우엔 라벨이면 무조건 에러 라벨명앞에 0x가 올수없다.)
	{
		for(i=2; i<n; i++)	cValue[i-2] = *(cOperand+i);
		ntype = HEX;
	}
	else
	{
		if(*(cOperand+0) == '-')
		{
			minus = 1;
			for(i=1; i<n; i++)	cOperand[i-1] = *(cOperand+i);
			cOperand[i-1] = '\0';
			n--;
		}

		if( *(cOperand+n-1) == 'B' )
		{
			for(i=0; i<n-1; i++)	cValue[i] = *(cOperand+i);
			ntype = BIN;
		}
		else if( *(cOperand+n-1) == 'D' )
		{
			for(i=0; i<n-1; i++)	cValue[i] = *(cOperand+i);
			ntype = DEC;
		}
		else
		{
			strcpy(cValue, cOperand);
			ntype = DEC; // 'b','d'가 아니면 십진수로 처리
		}
	}

	switch(ntype)
	{
		case BIN: // 'B' binary
			for(i=(int)(strlen(cValue)-1); i>=0; i--)
			{
				if( cValue[i] == '0' || cValue[i] == '1' )
				{
					nVal = cValue[i] - '0';
					dec += nVal*mul;
					mul *= BIN;
				}
				else	return false;
			}
			break;
		case DEC: // 'D' decimal
			for(i=(int)(strlen(cValue)-1); i>=0; i--)
			{
				if( cValue[i] >= '0' && cValue[i] <= '9' )
				{
					nVal = cValue[i] - '0';
					dec += nVal*mul;
					mul *= DEC;
				}
				else	return false;
			}
			break;
		case HEX: // 'H' hex
			for(i=(int)(strlen(cValue)-1); i>=0; i--)
			{
				if( cValue[i] >= '0' && cValue[i] <= '9' )		nVal = cValue[i] - '0';
				else if( cValue[i] >= 'A' && cValue[i] <= 'F' )	nVal = cValue[i] - 'A' + 10;
				else if( cValue[i] >= 'a' && cValue[i] <= 'f' )	nVal = cValue[i] - 'a' + 10;
				else	return false;

				dec += nVal*mul;
				mul *= HEX;
			}
			break;
		default :
			return false;
	}
	if(minus)
		dec *= -1;
	*retVal = dec;
	return true;
}

void MyMicroDelay(DWORD dwMicrosecond)
{
#if 0
	// 시작, 끝, 주기 진동수를 얻을 구조체를 선언한다.
	LARGE_INTEGER start, end, f;
	int tryCount = 0;
	__int64 micro_interval;
	// 고해상도 타이머의 주기를 얻는다.
	QueryPerformanceFrequency(&f);

	// 시작 시점의 진동수를 얻는다.
	QueryPerformanceCounter(&start);

	while (TRUE) {
		// 끝 시점의 진동수를 얻는다.
		QueryPerformanceCounter(&end);

		// 끝 시점의 진동수에서 시작 시점의 진동수를 뺀 후 진동수에서 1000000으로 나눈 값을 나눈다.
		// 1초 기준의 주기를 1000000 으로 나누었기 때문에 1마이크로초 동안 발생하는 진동수로 나눈 셈이다.
		micro_interval = (end.QuadPart - start.QuadPart) / (f.QuadPart / 1000000);
		if (micro_interval > dwMicrosecond)	break;
		//if (tryCount++ > dwMicrosecond)	break;
	inline }
#endif
}


void MyDelayMs(DWORD dwMillisecond)
{
	MSG	msg;
	//BOOL AllowIdle = FALSE;

	DWORD dwStart = GetTickCount();

	if (g_bAppExit)
		return;

	if (g_bUseMyDelayMs == FALSE)
	{// It fixed option dialog enter bug while FindUsbHid() is in progress
		Sleep(dwMillisecond);
		return;
	}

	while(TRUE)	{

		if ((GetTickCount() - dwStart) > dwMillisecond)
			break;

		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			//if (msg.message == WM_PAINT) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			//	AllowIdle = TRUE;
			//}
			//if (!AfxGetApp()->PumpMessage())
			//{
			//	bDoingBackgroundProcessing = FALSE;
			//	::PostQuitMessage();
			//	break;
			//}
		}

		//if (AllowIdle) {
		//	AfxGetApp()->OnIdle(idle++);
		//	AllowIdle = FALSE;
		//	//TRACE("MY SLEEP %d\n", idle);

		//}

		//Sleep(1);
		//MyMicroDelay(10);
	}
}

void DividedSleep(DWORD dwMillisecond)
{
//	int i;
//	for (i = 0; i < dwMillisecond; i++)
//		MySleep(1);

	MSG msg;
	unsigned long t0, t1, diff;

	t0 = GetTickCount();
	while(TRUE)
	{
		t1 = GetTickCount();
		diff = t1 - t0;

		if( diff > dwMillisecond )
			break;

		while( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			if (msg.message == WM_PAINT)	break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Sleep(1);
	}


#if 0
	HANDLE          hTimer;
	__int64         qwDueTime;


	#define _MSECOND	10000

	if (hTimer = CreateWaitableTimer(NULL, TRUE, NULL))           // Name of waitable timer
	{
		__try
		{
			LARGE_INTEGER DueTime;
			qwDueTime = -dwMillisecond * _MSECOND;
			DueTime.LowPart = (DWORD)(qwDueTime & 0xFFFFFFFF);
			DueTime.HighPart = (LONG)(qwDueTime >> 32);
			//DueTime.QuadPart = -dwMillisecond * _MSECOND;
			SetWaitableTimer(hTimer, &DueTime, 0, NULL, NULL, 0);
			WaitForSingleObject(hTimer, INFINITE);

		}
		__finally
		{
			CloseHandle(hTimer);
		}
	}
	else {
		MySleep(dwMillisecond);
	}
#endif
}


BOOL SemaWaitImmediate(HANDLE hHandle)
{
	DWORD  nRet;

	nRet = ::WaitForSingleObject(hHandle, 0);	//INFINITE );

	if( nRet == WAIT_ABANDONED )
	{
		return FALSE;
	}
	else if( nRet == WAIT_OBJECT_0 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
    }
}


BOOL SemaWait(HANDLE hHandle)
{
	DWORD  nRet;
	int	retry = 0;

	//TRACE("SEMA HANDLE : %d\n", hHandle);

	while (TRUE)
	{
		nRet = ::WaitForSingleObject(hHandle, 5000);

		if (nRet == WAIT_ABANDONED) {
			ResetEvent(hHandle);
			//TRACE("SEMA HANDLE EXIT (AB): %d\n", hHandle);
			return FALSE;
		}
		else if (nRet == WAIT_OBJECT_0) {
			//TRACE("SEMA HANDLE EXIT (OK): %d\n", hHandle);
			return TRUE;
		}
		else {
			ResetEvent(hHandle);
			//TRACE("SEMA HANDLE EXIT (ELSE): %d\n", hHandle);
			return TRUE;
		}
	}
	return TRUE;
}



BOOL SemaLongWait(HANDLE hHandle)
{
	DWORD  nRet;

	while (TRUE)
	{
		nRet = ::WaitForSingleObject(hHandle, 60000);

		if (nRet == WAIT_ABANDONED)
		{
			ResetEvent(hHandle);
			return FALSE;
		}
		else if (nRet == WAIT_OBJECT_0)
		{
			return TRUE;
		}
		else
		{
			ResetEvent(hHandle);
			return TRUE;
		}
	}
	return TRUE;
}

#if 0
CString GetFrequency(u16 nIcSignature, s16 * pIcRegData, int nVal, int *nFreq)
{
	CString strTmp;
	float Cpu_Osc = 1.0f;

	if (nIcSignature == 0xE200) {
		if (pIcRegData[REG_OSC_FREQ] == 0) {
			Cpu_Osc = 14*1000;
			strTmp.Format("%d KHz", (int)(Cpu_Osc/(nVal + 1)));
		}
		else if (pIcRegData[REG_OSC_FREQ] == 1) {
			Cpu_Osc = 18*1000;
			strTmp.Format("%d KHz", (int)(Cpu_Osc/(nVal + 1)));
		} else{
			strTmp.Format("CPU OSC : Unknown MHz!! You Must set Cpu Osc Frequency 0 or 21.");
		}
	}
	else if (nIcSignature == 0xF400) {
		if (pIcRegData[REG_OSC_FREQ] == 0) {
			Cpu_Osc = 16.6*1000;
			strTmp.Format("%d KHz", (int)(Cpu_Osc/(nVal + 1)));
		}
		else if (pIcRegData[REG_OSC_FREQ] == 21) {
			Cpu_Osc = 33.3*1000;
			strTmp.Format("%d KHz", (int)(Cpu_Osc/(nVal + 1)));
		}
		else if (pIcRegData[REG_OSC_FREQ] == 4) {
			Cpu_Osc = 24*1000;
			strTmp.Format("%d KHz", (int)(Cpu_Osc/(nVal + 1)));
		}
		else if (pIcRegData[REG_OSC_FREQ] == 11) {
			Cpu_Osc = 40*1000;
			strTmp.Format("%d KHz", (int)(Cpu_Osc/(nVal + 1)));
		}
		else{
			strTmp.Format("CPU OSC : Unknown MHz!! You Must set Cpu Osc Frequency 0, 4, 11 or 21.");
		}
	}
	else if (nIcSignature == 0xE240) {
		Cpu_Osc = (24 - (155 - pIcRegData[REG_OSC_FREQ])*0.124) * 1000;
		strTmp.Format("%d KHz", (int)(Cpu_Osc / (nVal + 1)));
	}
	else if (nIcSignature == 0xE700) {
//		Cpu_Osc = (50 - (255 - pIcRegData[REG_OSC_FREQ])*0.124) * 1000;
//		strTmp.Format("%d KHz", (int)(Cpu_Osc / (nVal + 1)));
		Cpu_Osc = FREQ_Table_7554[pIcRegData[REG_OSC_FREQ]];
		strTmp.Format("%d KHz", (int)(Cpu_Osc));

	}
	else if (nIcSignature == 0xE750 || nIcSignature == 0xE548
		|| nIcSignature == 0xE545 || nIcSignature == 0xE538
		|| nIcSignature == 0xE532) {
//		Cpu_Osc = (50 - (255 - pIcRegData[REG_OSC_FREQ])*0.124) * 1000;
//		strTmp.Format("%d KHz", (int)(Cpu_Osc / (nVal + 1)));
		Cpu_Osc = FREQ_Table_75XX[pIcRegData[REG_OSC_FREQ]];
		strTmp.Format("%d KHz", (int)(Cpu_Osc));
	}
	else if (nIcSignature == 0xE628) {
		//		Cpu_Osc = (50 - (255 - pIcRegData[REG_OSC_FREQ])*0.124) * 1000;
		//		strTmp.Format("%d KHz", (int)(Cpu_Osc / (nVal + 1)));
		Cpu_Osc = FREQ_Table_E628[pIcRegData[REG_OSC_FREQ]];
		strTmp.Format("%d KHz", (int)(Cpu_Osc));
	}
	else if (nIcSignature == 0xE622) {
		//		Cpu_Osc = (50 - (255 - pIcRegData[REG_OSC_FREQ])*0.124) * 1000;
		//		strTmp.Format("%d KHz", (int)(Cpu_Osc / (nVal + 1)));
		Cpu_Osc = FREQ_Table_E622[pIcRegData[REG_OSC_FREQ]];
		strTmp.Format("%d KHz", (int)(Cpu_Osc));
	}
	*nFreq = (int)((Cpu_Osc*1000)/(nVal + 1));

	return strTmp;
}

void ShowCalculatedRegInfo(u16 nIcSignature, s16 * pIcRegData, int nIdx, int nVal)
{
	float Cpu_Osc;
	CString strTmp;

	if(gRegNameInfo[nIdx].inputType == FREQUENCY_FORMAT) {

		if (nIcSignature == 0xE200) {
			if(pIcRegData[REG_OSC_FREQ] == 0) {
				Cpu_Osc = 14*1000;
				strTmp.Format("CPU OSC : 14 MHz\n");
				strTmp.AppendFormat("DRIVING FREQUENCY = %d KHz.", (int)(Cpu_Osc/(nVal + 1)));
			} else if(pIcRegData[REG_OSC_FREQ] == 1) {
				Cpu_Osc = 18*1000;
				strTmp.Format("CPU OSC : 18 MHz\n");
				strTmp.AppendFormat("DRIVING FREQUENCY = %d KHz.", (int)(Cpu_Osc/(nVal + 1)));
			} else{
				strTmp.Format("CPU OSC : Unknown MHz!! You Must set Cpu Osc Frequency 0 or 21.");
			}
			AfxMessageBox(strTmp);
		}
		else if (nIcSignature == 0xF400){
			if (pIcRegData[REG_OSC_FREQ] == 0) {
				Cpu_Osc = 16.6*1000;
				strTmp.Format("CPU OSC : 16.6 MHz\n");
				strTmp.AppendFormat("DRIVING FREQUENCY = %d KHz.", (int)(Cpu_Osc/(nVal + 1)));
			}
			else if (pIcRegData[REG_OSC_FREQ] == 21) {
				Cpu_Osc = 33.3*1000;
				strTmp.Format("CPU OSC : 33.3 MHz\n");
				strTmp.AppendFormat("DRIVING FREQUENCY = %d KHz.", (int)(Cpu_Osc/(nVal + 1)));
			}
			else if (pIcRegData[REG_OSC_FREQ] == 4) {
				Cpu_Osc = 24*1000;
				strTmp.Format("CPU OSC : 24 MHz\n");
				strTmp.AppendFormat("DRIVING FREQUENCY = %d KHz.", (int)(Cpu_Osc/(nVal + 1)));
			}
			else if (pIcRegData[REG_OSC_FREQ] == 11) {
				Cpu_Osc = 40*1000;
				strTmp.Format("CPU OSC : 40 MHz\n");
				strTmp.AppendFormat("DRIVING FREQUENCY = %d KHz.", (int)(Cpu_Osc/(nVal + 1)));
			}
			else{
				strTmp.Format("CPU OSC : Unknown MHz!! You Must set Cpu Osc Frequency 0, 4, 11 or 21.");
			}
			AfxMessageBox(strTmp);
		}
		else if (nIcSignature == 0xE240){
			Cpu_Osc = (24 - (155 - pIcRegData[REG_OSC_FREQ])*0.124f) * 1000.0f;
			strTmp.Format("CPU OSC : %.1f MHz)\n", Cpu_Osc);
			strTmp.AppendFormat("DRIVING FREQUENCY = %d KHz.", (int)(Cpu_Osc/(nVal + 1)));
			AfxMessageBox(strTmp);
		}
		else if (nIcSignature == 0xE700){
//			Cpu_Osc = (50 - (255 - pIcRegData[REG_OSC_FREQ])*0.124f) * 1000.0f;
			Cpu_Osc = FREQ_Table_7554[pIcRegData[REG_OSC_FREQ]];
			strTmp.Format("CPU OSC : %.1f MHz)\n", Cpu_Osc);
			strTmp.AppendFormat("DRIVING FREQUENCY = %d KHz.", (int)((Cpu_Osc * 1000) / (nVal + 1)));
			AfxMessageBox(strTmp);
		}
		else if (nIcSignature == 0xE750 || nIcSignature == 0xE548
			|| nIcSignature == 0xE545 || nIcSignature == 0xE538
			|| nIcSignature == 0xE532){
//			Cpu_Osc = (50 - (255 - pIcRegData[REG_OSC_FREQ])*0.124f) * 1000.0f;
			Cpu_Osc = FREQ_Table_75XX[pIcRegData[REG_OSC_FREQ]];
			strTmp.Format("CPU OSC : %.1f MHz)\n", Cpu_Osc);
			strTmp.AppendFormat("DRIVING FREQUENCY = %d KHz.", (int)((Cpu_Osc * 1000) / (nVal + 1)));
			AfxMessageBox(strTmp);
		}
		else if (nIcSignature == 0xE628){
			//			Cpu_Osc = (50 - (255 - pIcRegData[REG_OSC_FREQ])*0.124f) * 1000.0f;
			Cpu_Osc = FREQ_Table_E628[pIcRegData[REG_OSC_FREQ]];
			strTmp.Format("CPU OSC : %.1f MHz)\n", Cpu_Osc);
			strTmp.AppendFormat("DRIVING FREQUENCY = %d KHz.", (int)((Cpu_Osc * 1000) / (nVal + 1)));
			AfxMessageBox(strTmp);
		}
		else if (nIcSignature == 0xE622){
			//			Cpu_Osc = (50 - (255 - pIcRegData[REG_OSC_FREQ])*0.124f) * 1000.0f;
			Cpu_Osc = FREQ_Table_E622[pIcRegData[REG_OSC_FREQ]];
			strTmp.Format("CPU OSC : %.1f MHz)\n", Cpu_Osc);
			strTmp.AppendFormat("DRIVING FREQUENCY = %d KHz.", (int)((Cpu_Osc * 1000) / (nVal + 1)));
			AfxMessageBox(strTmp);
		}
	}
	else if(gRegNameInfo[nIdx].inputType == OSC_FREQUENCY_FORMAT)
	{
		if (nIcSignature == 0xE200) {
			if(nVal == 0) {
				strTmp.Format("CPU OSC : 14 MHz\n");
				strTmp.AppendFormat("You must set REG_INTAMP_TIME2=0x0022, REG_INTAMP_TIME3=0x0A0A, REG_ADC_TIME=0x0415");
			} else if(nVal == 1) {
				strTmp.Format("CPU OSC : 18 MHz\n");
				strTmp.AppendFormat("You must set REG_INTAMP_TIME2=0x0022, REG_INTAMP_TIME3=0x0B0B, REG_ADC_TIME=0x0515");
			} else {
				strTmp.Format("CPU OSC : Unknown MHz!! You Must set Osc Frequency 0 or 1.");
			}
			AfxMessageBox(strTmp);
		}
		else if (nIcSignature == 0xF400){
			if(nVal == 0) {
				strTmp.Format("CPU OSC : 16.6 MHz\n");
				strTmp.AppendFormat("You must set REG_INTAMP_TIME2 = 0x0022, REG_INTAMP_TIME3 = 0x0A0A, REG_ADC_TIME = 0x0415");
			} else if(nVal == 4) {
				strTmp.Format("CPU OSC : 24 MHz.\n");
				strTmp.AppendFormat("You must set REG_INTAMP_TIME2 = 0x0022, REG_INTAMP_TIME3 = 0x1010, REG_ADC_TIME = 0x0515");
			} else if(nVal == 21) {
				strTmp.Format("CPU OSC : 33.3 MHz\n");
				strTmp.AppendFormat("You must set REG_INTAMP_TIME2 = 0x0022, REG_INTAMP_TIME3 = 0x1414, REG_ADC_TIME = 0x0815");
			} else if(nVal == 11) {
				strTmp.Format("CPU OSC : 40 MHz\n");
				strTmp.AppendFormat("You must set REG_INTAMP_TIME2 = 0x0028, REG_INTAMP_TIME3 = 0x1818, REG_ADC_TIME = 0x0B15");
			} else {
				strTmp.Format("CPU OSC : Unknown MHz!! You Must set Cpu Osc Frequency 0, 4, 11 or 21.");
			}
			AfxMessageBox(strTmp);
		}
		else if (nIcSignature == 0xE240){
			Cpu_Osc = 24 - (155 - nVal)*0.124;
			strTmp.Format("CPU OSC : %.1f MHz", Cpu_Osc);
			AfxMessageBox(strTmp);
		}
		else if (nIcSignature == 0xE700){
			//	Cpu_Osc = 50 - (255 - nVal)*0.124;
			Cpu_Osc = FREQ_Table_7554[pIcRegData[REG_OSC_FREQ]];
			strTmp.Format("CPU OSC : %.1f MHz", Cpu_Osc);
			AfxMessageBox(strTmp);
		}
		else if (nIcSignature == 0xE750 || nIcSignature == 0xE548
			|| nIcSignature == 0xE545 || nIcSignature == 0xE538
			|| nIcSignature == 0xE532){
		//	Cpu_Osc = 50 - (255 - nVal)*0.124;
			Cpu_Osc = FREQ_Table_75XX[pIcRegData[REG_OSC_FREQ]];
			strTmp.Format("CPU OSC : %.1f MHz", Cpu_Osc);
			AfxMessageBox(strTmp);
		}
		else if (nIcSignature == 0xE628){
			//	Cpu_Osc = 50 - (255 - nVal)*0.124;
			Cpu_Osc = FREQ_Table_E628[pIcRegData[REG_OSC_FREQ]];
			strTmp.Format("CPU OSC : %.1f MHz", Cpu_Osc);
			AfxMessageBox(strTmp);
		}
		else if (nIcSignature == 0xE622){
			//	Cpu_Osc = 50 - (255 - nVal)*0.124;
			Cpu_Osc = FREQ_Table_E622[pIcRegData[REG_OSC_FREQ]];
			strTmp.Format("CPU OSC : %.1f MHz", Cpu_Osc);
			AfxMessageBox(strTmp);
		}
	} else if(gRegNameInfo[nIdx].inputType == CAP_FORMAT) {
		if (nIcSignature == 0xE240){
			if(nVal > 7) {
				AfxMessageBox("[ERROR] : THIS REGISTER's MAX VALUE IS 7.");
				return;
			}
			strTmp.Format("INTERNAL CAP = %.1f pF", (int)(28.6 + 7.8 * nVal));
			AfxMessageBox(strTmp);
			return;
		}

		if (nIcSignature == 0xE700){
			if (nVal > 7) {
				AfxMessageBox("[ERROR] : THIS REGISTER's MAX VALUE IS 7.");
				return;
			}
			strTmp.Format("INTERNAL CAP = %.1f pF", (int)(28.6 + 7.8 * nVal));
			AfxMessageBox(strTmp);
			return;
		}

		if (nIcSignature == 0xE750 || nIcSignature == 0xE548
			|| nIcSignature == 0xE545 || nIcSignature == 0xE532 || nIcSignature == 0xE628 || nIcSignature == 0xE622){
			if (nVal > 7) {
				AfxMessageBox("[ERROR] : THIS REGISTER's MAX VALUE IS 7.");
				return;
			}
			strTmp.Format("INTERNAL CAP = %.1f pF", (int)(28.6 + 7.8 * nVal));
			AfxMessageBox(strTmp);
			return;
		}

		if(nVal > 63) {
			AfxMessageBox("[ERROR] : THIS REGISTER's MAX VALUE IS 63.");
			return;
		}

		if (nIcSignature == 0xE200)
			strTmp.Format("INTERNAL CAP = %.1f pF", (int)(1.1f + (1.1f * nVal)));
		else
			strTmp.Format("INTERNAL CAP = %.1f pF", (int)(1.5f + (1.5f * nVal)));

		AfxMessageBox(strTmp);
	}

}

int GetIntCap(u16 nIcSignature, int nVal)
{
	if (nIcSignature == 0xE240){
		if(nVal > 7) {
			return 28.6;
		}
		return (int)(28.6 + 7.8 * nVal);
	}

	if (nIcSignature == 0xE700){
		if (nVal > 7) {
			return 28.6;
		}
		return (int)(28.6 + 7.8 * nVal);
	}

	if (nIcSignature == 0xE750 || nIcSignature == 0xE548
		|| nIcSignature == 0xE545 || nIcSignature == 0xE538
		|| nIcSignature == 0xE532 || nIcSignature == 0xE628 || nIcSignature == 0xE622){
		if (nVal > 7) {
			return 28.6;
		}
		return (int)(28.6 + 7.8 * nVal);
	}

	if (nIcSignature == 0xE200)
		return (int)(1.1f + (1.1f * nVal));

	return (int)(1.5f + (1.5f * nVal));
}

int GetValFromFrequency(u16 nIcSignature, s16 * pIcRegData, int nFreq)
{
	CString strTmp;
	float Cpu_Osc = 1.0f;

	if (nFreq == 0)	return 0;

	if (nIcSignature == 0xE200) {
		if (pIcRegData[REG_OSC_FREQ] == 0) {
			Cpu_Osc = 14*1000;
		} else { // if(pIcRegData[REG_OSC_FREQ] == 1) {
			Cpu_Osc = 18*1000;
		}
	}
	else if (nIcSignature == 0xF400){
		if (pIcRegData[REG_OSC_FREQ] == 0) {
			Cpu_Osc = 16.6*1000;
		}
		else if (pIcRegData[REG_OSC_FREQ] == 21) {
			Cpu_Osc = 33.3*1000;
		}
		else if (pIcRegData[REG_OSC_FREQ] == 4) {
			Cpu_Osc = 24*1000;
		}
		else { // if(pIcRegData[REG_OSC_FREQ] == 11) {
			Cpu_Osc = 40*1000;
		}
	}
	else if (nIcSignature == 0xE240){
		Cpu_Osc = (24 - (155 - nFreq)*0.124)*1000;
	}
	else if (nIcSignature == 0xE700){
		Cpu_Osc = FREQ_Table_7554[pIcRegData[REG_OSC_FREQ]];
//		Cpu_Osc = (50 - (255 - nFreq)*0.124) * 1000;
	}
	else if (nIcSignature == 0xE750 || nIcSignature == 0xE548
		|| nIcSignature == 0xE545 || nIcSignature == 0xE538
		|| nIcSignature == 0xE532){
		Cpu_Osc = FREQ_Table_75XX[pIcRegData[REG_OSC_FREQ]];
//		Cpu_Osc = (50 - (255 - nFreq)*0.124) * 1000;
	}
	else if (nIcSignature == 0xE628){
		Cpu_Osc = FREQ_Table_E628[pIcRegData[REG_OSC_FREQ]];
		//		Cpu_Osc = (50 - (255 - nFreq)*0.124) * 1000;
	}
	else if (nIcSignature == 0xE622){
		Cpu_Osc = FREQ_Table_E622[pIcRegData[REG_OSC_FREQ]];
		//		Cpu_Osc = (50 - (255 - nFreq)*0.124) * 1000;
	}


	return (int)((Cpu_Osc)/nFreq) - 1;
}

//void (*pCallbackFunc)();	// pointer for callback function
//void SetCallbackFunc(LPVOID lpCallBackFunc)
//{
//	pCallbackFunc = (void (__cdecl*)(void))lpCallBackFunc;	// set the function pointer
//}
#endif

CString	GetCurrentAppDirectory()
{
	HMODULE hModule;
	hModule = ::GetModuleHandle(NULL); // handle of current module
     ASSERT(hModule != 0);

    CString strExeFileName;
     VERIFY(::GetModuleFileName(hModule, strExeFileName.GetBuffer(_MAX_PATH),
      _MAX_PATH));
     strExeFileName.ReleaseBuffer();

    char Drive[_MAX_DRIVE];
     char Path[_MAX_PATH];
     char Filename[_MAX_FNAME];
     char Ext[_MAX_EXT];
     _splitpath(strExeFileName, Drive, Path, Filename, Ext);

    return CString(Drive)+CString(Path); // has trailing backslash
}

BYTE hex2byte(char hex)
{
	if (hex=='0')
		return 0;
	else if (hex=='1')
		return 1;
	else if (hex=='2')
		return 2;
	else if (hex=='3')
		return 3;
	else if (hex=='4')
		return 4;
	else if (hex=='5')
		return 5;
	else if (hex=='6')
		return 6;
	else if (hex=='7')
		return 7;
	else if (hex=='8')
		return 8;
	else if (hex=='9')
		return 9;
	else if (hex=='a' || hex=='A')
		return 10;
	else if (hex=='b' || hex=='B')
		return 11;
	else if (hex=='c' || hex=='C')
		return 12;
	else if (hex=='d' || hex=='D')
		return 13;
	else if (hex=='e' || hex=='E')
		return 14;
	else if (hex=='f' || hex=='F')
		return 15;
	else
		return -1;
}

#if 0
#if 0
BOOL CmdRun(LPSTR lpszCmd, LPSTR *lpszBuffer)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	BOOL bCreated = FALSE;
	SECURITY_ATTRIBUTES sa;

	*lpszBuffer = NULL;
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));
	memset(&si, 0, sizeof(STARTUPINFO));
	memset(&sa, 0, sizeof(SECURITY_ATTRIBUTES));

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	char szCmdOut[512];
	sprintf(szCmdOut, "%s\\cmd%ld_out.txt", GetCurrentAppDirectory(), GetCurrentThreadId());
	HANDLE hFile = CreateFile(szCmdOut, GENERIC_WRITE, 0, &sa, CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE ) {
		*lpszBuffer = new char[255];
		sprintf(*lpszBuffer, "[ERROR] CANNOT CREATE RESULT FILE");
		return FALSE;
	}

	//CloseHandle(hFile);

	//AfxMessageBox(szCmdOut);
	MySleep(100);

	char szCmd[512];
	sprintf(szCmd, "%s\\cmd%ld.cmd", GetCurrentAppDirectory(), GetCurrentThreadId());
	HANDLE hCmdFile = CreateFile(szCmd, GENERIC_WRITE, 0, &sa, CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL, NULL);
	if( hCmdFile == INVALID_HANDLE_VALUE ) {
		*lpszBuffer = new char[255];
		sprintf(*lpszBuffer, "[ERROR] CANNOT CREATE COMMAND FILE");
		CloseHandle(hFile);
		return FALSE;
	}
	MySleep(100);
	DWORD dwSize = 0;
	WriteFile(hCmdFile, lpszCmd,strlen(lpszCmd), &dwSize,NULL);
	CloseHandle(hCmdFile);

	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdOutput = hFile;
	si.hStdInput = hFile;
	si.hStdError = hFile;

	bCreated = CreateProcess(NULL, szCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	if( bCreated==FALSE ) {
		*lpszBuffer = new char[255];
		sprintf(*lpszBuffer, "[ERROR] CANNOT RUN COMMAND");
		CloseHandle(hFile);
		return FALSE;
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hFile);

	HANDLE hReadCmdFile = CreateFile(szCmdOut, GENERIC_READ, 0, &sa,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	dwSize = GetFileSize(hReadCmdFile,NULL);

	if(dwSize > 4096)	dwSize = 4096;

	*lpszBuffer = new char[dwSize + 1];
	memset(*lpszBuffer, 0, dwSize);
	DWORD dwRead = 0;
	ReadFile(hReadCmdFile,*lpszBuffer,dwSize,&dwRead,NULL);
	if(dwRead < dwSize)
		*(*lpszBuffer + dwRead) = 0;
	else
		*(*lpszBuffer + dwSize) = 0;

	CloseHandle(hReadCmdFile);

	DeleteFile(szCmd);
	DeleteFile(szCmdOut);

	return TRUE;
}
#else
BOOL CmdRun(LPSTR lpszCmd, LPSTR *lpszBuffer)
{
	CString csExecute;
	csExecute.Format("%s", lpszCmd);

	SECURITY_ATTRIBUTES secattr;
	ZeroMemory(&secattr, sizeof(secattr));
	secattr.nLength = sizeof(secattr);
	secattr.bInheritHandle = TRUE;

	HANDLE rPipe, wPipe;

	//Create pipes to write and read data
	CreatePipe(&rPipe, &wPipe, &secattr, 0);
	//
	STARTUPINFO sInfo;
	ZeroMemory(&sInfo, sizeof(sInfo));
	PROCESS_INFORMATION pInfo;
	ZeroMemory(&pInfo, sizeof(pInfo));
	sInfo.cb = sizeof(sInfo);
	sInfo.dwFlags = STARTF_USESTDHANDLES;
	sInfo.hStdInput = NULL;
	sInfo.hStdOutput = wPipe;
	sInfo.hStdError = wPipe;
	char command[1024];
	strcpy(command,
		csExecute.GetBuffer(csExecute.GetLength()));

	//Create the process here.
	CreateProcess(0, command, 0, 0, TRUE,
		NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, 0, 0, &sInfo, &pInfo);
	CloseHandle(wPipe);

	//now read the output pipe here.
	char buf[100];
	DWORD reDword;
	CString m_csOutput, csTemp;

	BOOL res;
	do
	{
		res = ::ReadFile(rPipe, buf, 100, &reDword, 0);
		csTemp = buf;
		m_csOutput += csTemp.Left(reDword);
	} while (res);

	*lpszBuffer = NULL;
	*lpszBuffer = (LPSTR)malloc(m_csOutput.GetLength() + 1);
	sprintf(*lpszBuffer, "%s", m_csOutput.GetBuffer());
	return TRUE;
}
#endif
#endif

BOOL IsDigit(char d)
{
	if ((d >= '0' && d <= '9') || d == '-')
		return TRUE;
	return FALSE;
}

UINT64 GetTimeInMillis(void)
{
	LARGE_INTEGER freq, now;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&now);

	// In order to prevent overflow, do the calculation in 2 parts
	return ((now.QuadPart * 100000 / freq.QuadPart) * 10000) / 1000000;
}

#if(0)
int GetTotalGroupCount()
{
	int Cnt = 0;
	while (1) {
		if (gRegGroupInfo[Cnt].id == ENDGROUP )
			return Cnt;
		Cnt++;
	}
}

int GetTotalRegisterCount()
{
	int Cnt = 0;
	while (1) {
		if (gRegNameInfo[Cnt].nGroupID == ENDGROUP)
			return Cnt;
		Cnt++;
	}
}
#endif

void Util_SetIcSignature(u16 icSignature)
{
	gCapInfo.icSignature = icSignature;

	#if(0)
	if (icSignature == 0xF400 || icSignature == 0xE200)
	{
		gRegGroupInfo = (stRegGroupInfo *)g3rdRegGroupInfo;
		gRegNameInfo = (tsRegNameInfo *)g3rdRegNameInfo;
	}
	else if (icSignature == 0xE240)
	{
		gRegGroupInfo = (stRegGroupInfo *)g4thRegGroupInfo;
		gRegNameInfo = (tsRegNameInfo *)g4thRegNameInfo;
	}
	else
	{
		gRegGroupInfo = (stRegGroupInfo *)g5thRegGroupInfo;
		gRegNameInfo = (tsRegNameInfo *)g5thRegNameInfo;
	}

	gCapInfo.totalGroupCount = GetTotalGroupCount();
	gCapInfo.totalRegisterCount = GetTotalRegisterCount();
	#endif
}

// \\?\hid#vid_14e5&pid_003a&mi_00#8&26629fa&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}

int GetTsmVersion(char * cDevicePath)
{
	int nVersion;
	char cVersion[3];
	char * endptr;
	char * cFind = StrStrI(cDevicePath, "REV_");

	if (cFind == NULL)
		return -1;

	cVersion[0] = cFind[4];
	cVersion[1] = cFind[5];
	cVersion[2] = 0;

	nVersion = strtol(cVersion, &endptr, 10);

	return nVersion;
}

int GetTsmId(char * cDevicePath)
{
	int nID;
	char cID[3];
	char * endptr;
	char * cFind = StrStrI(cDevicePath, "REV_");

	if (cFind == NULL)
		return -1;

	cID[0] = cFind[6];
	cID[1] = cFind[7];
	cID[2] = 0;

	nID = strtol(cID, &endptr, 10);

	return nID;
}

int GetDevSerial(char * cDevicePath)
{
	int nDevSerial;
	char * endptr;
	char * cFind = StrStrI(cDevicePath, "&mi_00#");
	if (cFind == NULL)		return 0;

	cFind = StrStrI(cFind + 7, "&");
	if (cFind == NULL)		return 0;

	nDevSerial = strtol(cFind + 1, &endptr, 16);

	return nDevSerial;
}

int GetFileSize(CString strFileName)
{
	FILE * fi = NULL;
	int nFileSize = 0;

	if (PathFileExists(strFileName) == FALSE)
		return 0;

	fi = fopen(strFileName, "r");
	if (fi == NULL)
		return 0;

	fseek(fi, 0, SEEK_END);
	nFileSize = ftell(fi);
	fclose(fi);

	return nFileSize;
}

//==============================================================================================
// Add space in string

static char g_stringWithSpace[MAX_DEVICE_COUNT][1024];

char * hvFillBlankSpaceInString(int nDevIndex, const CString & pstrSrc, int nMaxSpace, char cFillChar)
{
	if (nDevIndex < 0 || nDevIndex >= MAX_DEVICE_COUNT)
		return NULL;

	char * pDst = g_stringWithSpace[nDevIndex];

	CString sData = pstrSrc;

	sprintf(pDst, "%s", sData.GetBuffer());

	int nLength = strlen(pDst);
	if (nLength < nMaxSpace)
	{
		for (int i=0; i<(nMaxSpace-nLength); i++)
		{
			pDst[nLength + i] = cFillChar;
		}
	}

	pDst[nMaxSpace] = 0x00;

	return pDst;
}

char * hvFillBlankSpaceInString_0(int nDevIndex, const CString & pstrSrc)
{
	char * pFilledString = hvFillBlankSpaceInString(nDevIndex, pstrSrc, m_nLogLimitNumOfChar_0, '_');

	pFilledString[m_nLogLimitNumOfChar_0] = '\t';		// add TAB
	pFilledString[m_nLogLimitNumOfChar_0 + 1] = 0x00;

	return pFilledString;
}

char * hvItoaWithSpace(int nDevIndex, long lValue, int nMaxSpace)
{
	if (nDevIndex < 0 || nDevIndex >= MAX_DEVICE_COUNT)
			return NULL;

	char * pDst = g_stringWithSpace[nDevIndex];

	sprintf(pDst, "%ld", lValue);

	int nLength = strlen(pDst);
	if (nLength < nMaxSpace)
	{
		for (int i=0; i<(nMaxSpace-nLength); i++)
		{
			pDst[nLength + i] = ' ';
		}
	}

	pDst[nMaxSpace] = 0x00;

	return pDst;
}

void SetButtonConfig(CShadeButtonST * pButton)
{
	pButton->SetShade(CShadeButtonST::SHS_HARDBUMP);
	pButton->SetColor(CButtonST::BTNST_COLOR_FG_IN, ZT_COLOR_HIGHLIHGT);
	pButton->SetBtnCursor(IDC_CURSOR_HAND);
}

void ChangeCheckboxBgColor(CDC* pDC, BOOL bChecked)
{
	if (bChecked)
		pDC->SetBkColor(ZT_COLOR_SELECTED);
	else
		pDC->SetBkMode(TRANSPARENT);
}

unsigned short Str_to_Hex(unsigned char *string,unsigned char len)
{
	unsigned char temp_index = len;
	unsigned short i,temp=0;
	unsigned char shift_table[5] ={0,0,4,8,12};

	for(i=0;i<len;i++)
	{
		if(string[i]>='0' && string[i]<='9' )
		{
			temp |= (string[i]-0x30) << shift_table[temp_index];
			temp_index--;
		}
		else if(string[i]>='A' && string[i]<='F')
		{
			temp |= (string[i]-0x37) << shift_table[temp_index];
			temp_index--;
		}
		else if(string[i]>='a' && string[i]<='f')
		{
			temp |= (string[i]-0x57) << shift_table[temp_index];
			temp_index--;
		}
		/*
		else if(string[i]=='p' || string[i]=='P')
		{
			temp='p';
			break;
		}
		*/
	}

	return(temp);
}

void XlineFilter(s16 nThd, s16 ratio, s16 *BUFF, s16 nRow, s16 nCol)
{
	int i, j, x, y;
	u32 nSum;
	u32 nDivider;
	u32 nAvg;
	int self_ratio = 0;

	if(ratio <= 0)
		return;

	nDivider = nRow * 100;
	for(x = 0; x < nCol; x++)
	{
		nSum = 0;
		i = j = x * nRow;
		for(y = 0; y < nRow; y++, i++)
		{
			if(BUFF[i] > nThd) {nSum += (u32)nThd;}
			else {nSum += (u32)BUFF[i];}
		}

		nAvg = nSum*ratio/nDivider;

		for(y = 0; y < nRow; y++, j++)
		{
			BUFF[j] -= (s16)nAvg;
		}
	}
}

//==============================================================================================
// for file debugging

#if defined (HV_USE_FILE_DEBUG)
static TCHAR	g_HvTraceBuffer[1024];

#include ".\Trace\HTrace.h"

static CHTrace * g_pHvTracer = NULL;

void HV_TRACE_INITIALIZE(CString strSaveDirectory)
{
	g_pHvTracer = new CHTrace;

	if (g_pHvTracer == NULL)
		return;

	g_pHvTracer->Create(TRACERST_FLAG_SAVE, NULL);
	g_pHvTracer->SetTraceDirectory(strSaveDirectory);

	CTime curTime = CTime::GetCurrentTime();
	HV_TRACE("\n\n============================================\r\n");
	HV_TRACE("%s\r\n", curTime.Format("%Y-%m-%d %H:%M:%S"));
}

void HV_TRACE_FINALIZE()
{
	if (g_pHvTracer)
	{
		delete g_pHvTracer;
		g_pHvTracer = NULL;
	}
}

void HV_TRACE(TCHAR* fmt, ...)
{
	va_list arg;

	va_start(arg, fmt);
	vsprintf(g_HvTraceBuffer, fmt, arg);
	va_end(arg);

	if (g_pHvTracer)
	{
		g_pHvTracer->Trace(g_HvTraceBuffer);
	}
}
#endif


