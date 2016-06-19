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

int isMachine64(char* file) {
	IMAGE_FILE_HEADER* image_file_header = getImageFileHeader(file);
	return image_file_header->Machine == IMAGE_FILE_MACHINE_AMD64 ? 1 : 0;
}

IMAGE_OPTIONAL_HEADER64* getOptionalHeader64(char* file) {
	IMAGE_FILE_HEADER * image_file_header = getImageFileHeader(file);
	if (!isMachine64(file) || image_file_header->SizeOfOptionalHeader == 0) {
		return NULL;
	}
	return (IMAGE_OPTIONAL_HEADER64 *)&image_file_header[1];
}

IMAGE_OPTIONAL_HEADER32* getOptionalHeader32(char* file) {
	IMAGE_FILE_HEADER * image_file_header = getImageFileHeader(file);
	if (isMachine64(file) || image_file_header->SizeOfOptionalHeader==0) {
		return NULL;
	}
	return (IMAGE_OPTIONAL_HEADER32 *)&image_file_header[1];
}

IMAGE_DATA_DIRECTORY* getImageDataDirectories(char* file) {
	IMAGE_FILE_HEADER * image_file_header = getImageFileHeader(file);
	if (image_file_header->SizeOfOptionalHeader == 0)
		return NULL;
	if (isMachine64(file)) {
		IMAGE_OPTIONAL_HEADER64 * optional_header = getOptionalHeader64(file);
		return (IMAGE_DATA_DIRECTORY *)
			((char*)&optional_header->NumberOfRvaAndSizes + sizeof(optional_header->NumberOfRvaAndSizes));
	}
	else {
		IMAGE_OPTIONAL_HEADER32 * optional_header = getOptionalHeader32(file);
		return (IMAGE_DATA_DIRECTORY *)
			((char*)&optional_header->NumberOfRvaAndSizes + sizeof(optional_header->NumberOfRvaAndSizes));
	}
}

IMAGE_SECTION_HEADER* getSectionByName(char* file, char* name) {
	IMAGE_FILE_HEADER* image_file_header = getImageFileHeader(file);
	IMAGE_SECTION_HEADER* sections = getImageSectionHeaders(file);
	for (int i = 0; i < image_file_header->NumberOfSections; i++) {
		for (int j = 0; j <= strlen(name)-1; j++) {
			if (j == strlen(name)-1) {
				return sections + i;
			}
			if (sections[i].Name[j] != name[j]) {
				break;
			}
		}
	}
	return NULL;
}


char* getAddrFromRVA(char* file, int rvaddr)
{
	IMAGE_SECTION_HEADER *phead;
	IMAGE_FILE_HEADER* image_file_header = getImageFileHeader(file);
	int offset;
	for (int i = 0; i < image_file_header->NumberOfSections; i++) {
		phead = (IMAGE_SECTION_HEADER*)(getImageSectionHeaders(file) + i);
		offset = rvaddr - phead->VirtualAddress;
		if (offset >= 0 && offset < phead->SizeOfRawData) {
			return file + phead->PointerToRawData + offset;
		}
	}
	return NULL;
}

#define UNW_FLAG_EHANDLER  0x01
#define UNW_FLAG_UHANDLER  0x02
#define UNW_FLAG_CHAININFO 0x04

typedef union _UNWIND_CODE {
	struct {
		BYTE CodeOffset;
		BYTE UnwindOp : 4;
		BYTE OpInfo : 4;
	};
	USHORT FrameOffset;
} UNWIND_CODE, *PUNWIND_CODE;

typedef struct _UNWIND_INFO {
	BYTE Version : 3;
	BYTE Flags : 5;
	BYTE SizeOfProlog;
	BYTE CountOfCodes;
	BYTE FrameRegister : 4;
	BYTE FrameOffset : 4;
	UNWIND_CODE UnwindCode[1];
	/*  UNWIND_CODE MoreUnwindCode[((CountOfCodes + 1) & ~1) - 1];
	*   union {
	*       OPTIONAL ULONG ExceptionHandler;
	*       OPTIONAL ULONG FunctionEntry;
	*   };
	*   OPTIONAL ULONG ExceptionData[]; */
} UNWIND_INFO, *PUNWIND_INFO;


int main(int argc, _TCHAR *argv[])
{

	IMAGE_DOS_HEADER* image_header;
	char * pData;         // pointer to the data
	int i;                // loop counter
	char* iData;            // on success contains the first int of data

	Mapper map;
	if (argc == 1) {
		_tprintf(TEXT("Wrong parameters number"));
		return 0;
	}
	if (!map.MapFile(argv[1])) {
		_tprintf(TEXT("Error mapping file"));
		return 0;
	}
	pData = map.GetMapAddress();

	iData = (char *)pData;
	image_header = getImageDosHeader(pData); 

	// IMAGE HEADER
	_tprintf(TEXT("1 = %c , 2 = %c e_lfanew = %x\n"),
		iData[0],
		iData[1],
		image_header->e_lfanew);
	_tprintf(TEXT("1 = %c , 2 = %c, 3 = %c , 4 = %c\n"),
		iData[image_header->e_lfanew + 0],
		iData[image_header->e_lfanew + 1],
		iData[image_header->e_lfanew + 2],
		iData[image_header->e_lfanew + 3]);
	// IMAGE_FILE_HEADER
	IMAGE_FILE_HEADER* image_file_header = getImageFileHeader(pData);
	_tprintf(TEXT("machine = %x , NumberOfSections = %d, NumberOfSymbols = %d , SizeOfOptionalHeader = %d\n"),
		image_file_header->Machine,
		image_file_header->NumberOfSections, 
		image_file_header->NumberOfSymbols, 
		image_file_header->SizeOfOptionalHeader );
	// SECTIONS
	IMAGE_SECTION_HEADER* sect_head = getImageSectionHeaders(pData);
	for (i = 0; i < image_file_header->NumberOfSections; i++) {
		printf("%-8.8s: rawsize %u\n",
			sect_head[i].Name,
			sect_head[i].SizeOfRawData);
	}
	// OPTIONAL HEADER
	IMAGE_OPTIONAL_HEADER64 * optional_header64 = getOptionalHeader64(pData);
	IMAGE_OPTIONAL_HEADER32 * optional_header32 = getOptionalHeader32(pData);
	int data_directories_num = optional_header64->NumberOfRvaAndSizes;
	if (optional_header64 != NULL) {
		_tprintf(TEXT("magic = %x \n"),
			optional_header64->Magic);
		data_directories_num = optional_header64->NumberOfRvaAndSizes;
	}
	if (optional_header32 != NULL) {
		_tprintf(TEXT("magic = %x \n"),
			optional_header32->Magic);
		data_directories_num = optional_header32->NumberOfRvaAndSizes;
	}
	{
		IMAGE_DATA_DIRECTORY * data_directories = getImageDataDirectories(pData);
		_tprintf(TEXT("num of data directories = %d\n"), data_directories_num);
		for (i = 0; i < data_directories_num; i++) {
			_tprintf(TEXT("%d virt addr = %x size = %x \n"), i, data_directories[i].VirtualAddress, data_directories[i].Size);
		}

		// IMPORTS TABLE
		if (data_directories[1].Size > 0) {
			union THUNK_DATAXX {
				ULONGLONG u64;
				UINT u32;
			} *pthunk;
			IMAGE_IMPORT_BY_NAME *pname;
			ULONG64 sz = image_file_header->Machine == IMAGE_FILE_MACHINE_AMD64 ? 8 : 4;
			IMAGE_IMPORT_DESCRIPTOR* pentry = (IMAGE_IMPORT_DESCRIPTOR*)getAddrFromRVA(pData, data_directories[1].VirtualAddress);
			int i = 0;

			for (int j = 0; pentry[j].Characteristics != 0; j++) {
				printf("dll name: %s\n", getAddrFromRVA(pData, pentry[j].Name));

				i = 0;
				while (1) {
					pthunk = (THUNK_DATAXX*)getAddrFromRVA(pData, pentry[j].OriginalFirstThunk + i * sz);
					if (pthunk->u32 == 0)
						break;
					if ((pthunk->u64 & (1ULL << (sz * 8 - 1)))) //Check for ordinal bit
						_tprintf(_T("  function: %d\n"), pthunk->u32 & 0xffff);
					else {
						pname = (IMAGE_IMPORT_BY_NAME*)getAddrFromRVA(pData, pthunk->u32);
						printf("  function: %s\n", pname->Name);
					}
					i++;
				}
			}
		}
		// EXPORTS TABLE
		if (data_directories[0].Size > 0){
			IMAGE_EXPORT_DIRECTORY* exp_dir = (IMAGE_EXPORT_DIRECTORY*)getAddrFromRVA(pData, data_directories[0].VirtualAddress);
			int i = 0;
			DWORD *nameTable, *funcTable;
			WORD *ordinalTable;

			printf("dll name: %s, functions: %d, names: %d\n", getAddrFromRVA(pData, exp_dir->Name),
				exp_dir->NumberOfFunctions, exp_dir->NumberOfNames);

			nameTable = (DWORD*)getAddrFromRVA(pData, exp_dir->AddressOfNames);
			funcTable = (DWORD*)getAddrFromRVA(pData, exp_dir->AddressOfFunctions);
			ordinalTable = (WORD*)getAddrFromRVA(pData, exp_dir->AddressOfNameOrdinals);
			for (i = 0; i < exp_dir->NumberOfNames; i++) {
				printf("  %s: ordinal %d, rva 0x%x\n", getAddrFromRVA(pData, nameTable[i]),
					ordinalTable[i] + exp_dir->Base, funcTable[ordinalTable[i]]);
			}
		}
		// EXCEPTIONS TABLE
		if (data_directories[IMAGE_DIRECTORY_ENTRY_EXCEPTION].Size > 0) {
			RUNTIME_FUNCTION *pdir = (RUNTIME_FUNCTION*)getAddrFromRVA(pData, data_directories[IMAGE_DIRECTORY_ENTRY_EXCEPTION].VirtualAddress);
			int i = 0;
			for (i = 0; i < data_directories[IMAGE_DIRECTORY_ENTRY_EXCEPTION].Size / sizeof(RUNTIME_FUNCTION); i++) {
				UNWIND_INFO *uinfo = (UNWIND_INFO*)getAddrFromRVA(pData, (pdir+i)->UnwindInfoAddress);
				if (uinfo != NULL && uinfo->Flags == UNW_FLAG_EHANDLER) {
					ULONG handler = *(ULONG*)&uinfo->UnwindCode[((uinfo->CountOfCodes + 1) & ~1)];
					printf("func (0x%x-0x%x): 0x%x\n", pdir->BeginAddress, pdir->EndAddress, handler);
				}
			}
		}
	}
	map.UnmapFile();
err:
	return 0;
}
