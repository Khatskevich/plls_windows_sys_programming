// ida_disassembling.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//extern "C" int __fastcall min(int a, int b);

int main(int argc, TCHAR *argv[])
{

	IMAGE_DOS_HEADER* image_header;
	HANDLE hMapFile;      // handle for the file's memory-mapped region
	HANDLE hFile;         // the file handle
	BOOL bFlag;           // a result holder
	DWORD dBytesWritten;  // number of bytes written
	DWORD dwFileSize;     // temporary storage for file sizes
	DWORD dwFileMapSize;  // size of the file mapping
	DWORD dwMapViewSize;  // the size of the view
	DWORD dwFileMapStart; // where to start the file map view
	DWORD dwSysGran;      // system allocation granularity
	SYSTEM_INFO SysInfo;  // system information; used to get granularity
	LPVOID lpMapAddress;  // pointer to the base address of the
						  // memory-mapped region
	char * pData;         // pointer to the data
	int i;                // loop counter
	char* iData;            // on success contains the first int of data
	int iViewDelta = 0;       // the offset into the view where the data
						  //shows up

						  // Create the test file. Open it "Create Always" to overwrite any
						  // existing file. The data is re-created below
	hFile = CreateFile(TEXT("C:/plls_windows_sys_programming/2_ida_disassembling/x64/Debug/ida_disassembling.exe"),      // Open One.txt
		GENERIC_READ,           // Open for reading
		0,                      // Do not share
		NULL,                   // No security
		OPEN_EXISTING,          // Existing file only
		FILE_ATTRIBUTE_NORMAL,  // Normal file
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		_tprintf(TEXT("hFile is NULL: last error: %d\n"), GetLastError());
		return 4;
	}


	// Verify that the correct file size was written.
	dwFileSize = GetFileSize(hFile, NULL);
	_tprintf(TEXT("hFile size: %10d\n"), dwFileSize);

	// Create a file mapping object for the file
	// Note that it is a good idea to ensure the file size is not zero
	hMapFile = CreateFileMapping(hFile,          // current file handle
		NULL,           // default security
		PAGE_READONLY, // read/write permission
		0,              // size of mapping object, high
		dwFileSize,  // size of mapping object, low
		NULL);          // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("hMapFile is NULL: last error: %d\n"), GetLastError());
		return (2);
	}

	// Map the view and test the results.

	lpMapAddress = MapViewOfFile(hMapFile,            // handle to
													  // mapping object
		FILE_MAP_READ, // read/write
		0,                   // high-order 32
							 // bits of file
							 // offset
		0,      // low-order 32
							 // bits of file
							 // offset
		dwFileSize);      // number of bytes
							 // to map
	if (lpMapAddress == NULL)
	{
		_tprintf(TEXT("lpMapAddress is NULL: last error: %d\n"), GetLastError());
		return 3;
	}

	// Calculate the pointer to the data.
	pData = (char *)lpMapAddress + iViewDelta;

	// Extract the data, an int. Cast the pointer pData from a "pointer
	// to char" to a "pointer to int" to get the whole thing
	iData = (char *)pData;
	image_header = (IMAGE_DOS_HEADER*)pData;

	_tprintf(TEXT("1 = %c , 2 = %c e_lfanew = %x\n"),
		iData[0],
		iData[1],
		image_header->e_lfanew);
	_tprintf(TEXT("1 = %c , 2 = %c, 3 = %c , 4 = %c\n"),
		iData[image_header->e_lfanew + 0],
		iData[image_header->e_lfanew + 1],
		iData[image_header->e_lfanew + 2],
		iData[image_header->e_lfanew + 3]);
	IMAGE_FILE_HEADER* image_file_header = (IMAGE_FILE_HEADER*)(pData + image_header->e_lfanew + 4);
	_tprintf(TEXT("machine = %x , NumberOfSections = %d, NumberOfSymbols = %d , SizeOfOptionalHeader = %d\n"),
		image_file_header->Machine,
		image_file_header->NumberOfSections, 
		image_file_header->NumberOfSymbols, 
		image_file_header->SizeOfOptionalHeader );
	// Close the file mapping object and the open file
	IMAGE_SECTION_HEADER* sect_head = (IMAGE_SECTION_HEADER*)( (char*)(image_file_header + 1)+image_file_header->SizeOfOptionalHeader);
	for (i = 0; i < image_file_header->NumberOfSections; i++) {
		printf("%-8.8s: rawsize %u\n",
			sect_head[i].Name,
			sect_head[i].SizeOfRawData);
	}
	bFlag = UnmapViewOfFile(lpMapAddress);
	bFlag = CloseHandle(hMapFile); // close the file mapping object

	bFlag = CloseHandle(hFile);   // close the file itself

	//printf("%d/n", min(5, 10));
    return 0;
}
