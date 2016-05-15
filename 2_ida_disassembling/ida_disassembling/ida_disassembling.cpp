// ida_disassembling.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Mapper.h"

//extern "C" int __fastcall min(int a, int b);

#define MAXHINE_64_CONST 0x8664

IMAGE_DOS_HEADER* getImageDosHeader(char* file) {
	return (IMAGE_DOS_HEADER*) file;
}

IMAGE_FILE_HEADER* getImageFileHeader(char* file) {
	return  (IMAGE_FILE_HEADER*)(file + getImageDosHeader(file)->e_lfanew + 4);
}

IMAGE_SECTION_HEADER* getImageSectionHeaders(char* file) {
	IMAGE_FILE_HEADER * image_file_header = getImageFileHeader(file);
	return (IMAGE_SECTION_HEADER*)((char*)(image_file_header + 1) + image_file_header->SizeOfOptionalHeader);
}
IMAGE_OPTIONAL_HEADER64* getOptionalHeader64(char* file) {
	IMAGE_FILE_HEADER * image_file_header = getImageFileHeader(file);
	if (image_file_header->Machine != MAXHINE_64_CONST) {
		return NULL;
	}
	return (IMAGE_OPTIONAL_HEADER64 *)&image_file_header[1];
}

IMAGE_DATA_DIRECTORY* getImageDataDirectories64(char* file) {
	IMAGE_OPTIONAL_HEADER64 * optional_header = getOptionalHeader64(file);
	if (optional_header == NULL) {
		return NULL;
	}
	return (IMAGE_DATA_DIRECTORY *)
		((char*)&optional_header->NumberOfRvaAndSizes + sizeof(optional_header->NumberOfRvaAndSizes));
}




int main(int argc, TCHAR *argv[])
{

	IMAGE_DOS_HEADER* image_header;
	char * pData;         // pointer to the data
	int i;                // loop counter
	char* iData;            // on success contains the first int of data

	Mapper map;

	if (!map.MapFile(TEXT("C:/plls_windows_sys_programming/2_ida_disassembling/x64/Debug/ida_disassembling.exe"))) {
		_tprintf(TEXT("Error mapping file"));
		return 0;
	}
	pData = map.GetMapAddress();

	iData = (char *)pData;
	image_header = getImageDosHeader(pData); 

	_tprintf(TEXT("1 = %c , 2 = %c e_lfanew = %x\n"),
		iData[0],
		iData[1],
		image_header->e_lfanew);
	_tprintf(TEXT("1 = %c , 2 = %c, 3 = %c , 4 = %c\n"),
		iData[image_header->e_lfanew + 0],
		iData[image_header->e_lfanew + 1],
		iData[image_header->e_lfanew + 2],
		iData[image_header->e_lfanew + 3]);
	IMAGE_FILE_HEADER* image_file_header = getImageFileHeader(pData);
	_tprintf(TEXT("machine = %x , NumberOfSections = %d, NumberOfSymbols = %d , SizeOfOptionalHeader = %d\n"),
		image_file_header->Machine,
		image_file_header->NumberOfSections, 
		image_file_header->NumberOfSymbols, 
		image_file_header->SizeOfOptionalHeader );
	// Close the file mapping object and the open file
	IMAGE_SECTION_HEADER* sect_head = getImageSectionHeaders(pData);
	for (i = 0; i < image_file_header->NumberOfSections; i++) {
		printf("%-8.8s: rawsize %u\n",
			sect_head[i].Name,
			sect_head[i].SizeOfRawData);
	}
	IMAGE_OPTIONAL_HEADER64 * optional_header = getOptionalHeader64(pData);
	if ( optional_header != NULL) {
		_tprintf(TEXT("magic = %x \n"),
			optional_header->Magic);
		int data_directories_num = optional_header->NumberOfRvaAndSizes;
		IMAGE_DATA_DIRECTORY * data_directories = getImageDataDirectories64(pData);
		_tprintf(TEXT("num of data directories = %d\n"), data_directories_num);
		_tprintf(TEXT("offset = %d\n"), ((char*)data_directories - (char*)optional_header));
		for (i = 0; i < data_directories_num; i++) {
			_tprintf(TEXT("%d virt addr = %x size = %x \n"), i, data_directories[i].VirtualAddress, data_directories[i].Size);
		}
	}
	map.UnmapFile();
err:
	return 0;
}
