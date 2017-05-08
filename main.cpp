#include "MemoryAllocationLog.h"
using namespace std;

int main(void)
{
	//Memory Initialize.
	memset(g_stAllocInfo, 0, sizeof(st_ALLOC_INFO) * 100);

	//int *p = MemoryAlloc<int>(1, __FILE__, __LINE__);
	//#define _MemoryAlloc(T, Size)	MemoryAlloc<T>(Size, __FILE__, __LINE__)

	int *p4 = _MemoryAlloc(int, 1);
	int *p400 = _MemoryAlloc(int, 100);

	char *pZ1 = _MemoryAlloc(char, 50);
	char *pZ2 = _MemoryAlloc(char, 150);
	char *pZ3 = _MemoryAlloc(char, 60);
	char *pZ4 = _MemoryAlloc(char, 70);

	MemoryRelease(p4);
	MemoryRelease(pZ1);
	MemoryRelease(pZ3);
	MemoryRelease(p4);

	PrintAlloc();
	return 0;
};