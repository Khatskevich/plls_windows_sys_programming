// shellcode.cpp : Defines the entry point for the console application.
//


#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <winnt.h>
#include <stddef.h>

int* min2(int, int);

char ar[] = { 1, 2, 3 };

struct sc_data_t {
	char libName[16];
	void* ploadLibrary;
	UINT32 a, b, c;
	UINT32 reserved;
};

int _tmain(int argc, _TCHAR* argv[])
{
	UINT8* func = min2;
	int offset = ((int*)(&func[1]))[0];
	func = func + offset+5;
	int i;
	for (i = 0; func[i] != 0xc3; i++) {
		_tprintf("0x%x, ", (unsigned int)func[i]);
	}
	_tprintf("0x%x ", (unsigned int)func[i]);
	//LoadLibraryA("keyiso.dll");
	//struct sc_data_t sc_data;
	//_tprintf("%d, %d\n", sizeof(struct sc_data_t), offsetof(struct sc_data_t, c));
	//LoadLibrary("a");
	return 0;
}
