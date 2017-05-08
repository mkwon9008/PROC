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
		if (g_stAllocInfo[iCnt].pPtr == pPtr) //find pPtr from g_stAllocInfo[ARR]
		{
			return iCnt;
		}
		return -1; //if not found then return -1
	}
}

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
