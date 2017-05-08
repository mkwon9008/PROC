#pragma once
#ifndef __MEMORY_ALLOCATION_LOG_H__
#define __MEMORY_ALLOCATION_LOG_H__

#include <iostream>
#define _MemoryAlloc(T, Size)	MemoryAlloc<T>(Size, __FILE__, __LINE__)
struct st_ALLOC_INFO
{
	void *pPtr;
	int iSize;
	char szFile[256];
	int iLine;
};

extern st_ALLOC_INFO g_stAllocInfo[];
extern int g_iTotalAllocCount;
extern int g_iTotalAllocSize;

bool SaveAllocInfo(void *pPtr, int iSize, char *szFile, int iLine);
int FindAllocInfo(void *pPtr);
bool PrintAlloc(void);

template <typename T>
T *MemoryAlloc(int iCount, char *szFile, int iLine)
{
	T *pNew = new T[iCount];

	SaveAllocInfo(pNew, sizeof(T) * iCount, szFile, iLine);

	g_iTotalAllocCount += 1;
	g_iTotalAllocSize += (sizeof(T) * iCount);
	printf("%d개를 %d 만큼 할당!\n", iCount, iCount * sizeof(T));
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
	printf("FREE! %dBytes\n", sizeof(st_ALLOC_INFO));
	delete[] pPtr; //memory destroy.
	return true;
}

#endif