#include "MemoryAllocationLog.h"

st_ALLOC_INFO g_stAllocInfo[100];
int g_iTotalAllocCount = 0;
int g_iTotalAllocSize = 0;

//save parameter to st_AllocInfo.
bool SaveAllocInfo(void *pPtr, int iSize, char *szFile, int iLine)
{
	int iCnt;
	for (iCnt = 0; iCnt < 100; iCnt++)
	{
		if (g_stAllocInfo[iCnt].pPtr == NULL)
		{
			g_stAllocInfo[iCnt].pPtr = pPtr;
			g_stAllocInfo[iCnt].iSize = iSize;
			strcpy_s(g_stAllocInfo[iCnt].szFile, sizeof(g_stAllocInfo[iCnt].szFile), szFile);
			g_stAllocInfo[iCnt].iLine = iLine;
			return true;
		}
	}
	return false;
}

int FindAllocInfo(void *pPtr)
{
	int iCnt;
	
	if (pPtr == NULL) //NULLpointer Exception.
	{
		return -1;
	}

	for (iCnt = 0; iCnt < 100; iCnt++)
	{
		if (g_stAllocInfo[iCnt].pPtr == pPtr) //반복문을 순회하면서 g_stAllocInfo의 배열에서 해당 pPtr을 찾아서 배열번호 반환.
		{
			return iCnt;
		}
		return -1; //못찾으면 -1 반환.
	}
}

/*
//메모리할당을 요청한 파일, 준 번호를 기억하고싶다.
template <typename T>
T *MemoryAlloc(int iCount, char *szFile, int iLine)
{
	T *pNew = new T[iCount];

	SaveAllocInfo(pNew, sizeof(T) * iCount, szFile, iLine);

	g_iTotalAllocCount += 1;
	g_iTotalAllocSize += (sizeof(t) * iCount);

	return pNew;
}

template <typename T>
bool MemoryRelease(T *pPtr)
{
	int iAlocIndex = FindAllocInfo(pPtr); //g_stAllocInfo에서 pPtr과 같은 번호의 g_stAllocInfo[iCnt].pPtr을 찾아서 해당 iCnt반환.

	if (iAlocIndex == -1) //if not found. 
	{
		return false;
	}

	memset(&g_stAllocInfo[iAlocIndex], 0, sizeof(st_ALLOC_INFO));
	delete[] pPtr; //memory destroy.
	return true;
}*/

bool PrintAlloc(void)
{
	printf("\nTotal Alloc Size : %d\n", g_iTotalAllocSize);
	printf("Total Alloc Count : %d\n", g_iTotalAllocCount);
	

	int iCnt;
	for (iCnt = 0; iCnt < 100; iCnt++)
	{
		if (g_stAllocInfo[iCnt].pPtr != NULL)
		{
			printf("Not Release Memory : [0x%x] %d Bytes \n", g_stAllocInfo[iCnt].pPtr, g_stAllocInfo[iCnt].iSize);
			printf("FIle : %s : %d\n\n", g_stAllocInfo[iCnt].szFile, g_stAllocInfo[iCnt].iLine);
		}
	}

	return true;
}