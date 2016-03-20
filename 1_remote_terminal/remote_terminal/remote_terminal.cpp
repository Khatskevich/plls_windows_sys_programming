// remote_terminal.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Service.h"
#include "Utils.h"



int run_child_with_redirected_io(int argc, TCHAR *argv[]);

int __cdecl server(void);
int __cdecl client(char *);


void _tmain2(int argc, TCHAR *argv[]);
//void __cdecl _tmain3(int argc, TCHAR *argv[]);


int _tmain(int argc, TCHAR *argv[])
{

	TCHAR szCommand[10];
	TCHAR szSvcName[80];
	if (argc >= 2)
		StringCchCopy(szCommand, 10, argv[1]);
	if (argc == 3)
		StringCchCopy(szSvcName, 80, argv[2]);
	
	if (argc == 2 && lstrcmpi(szCommand, TEXT("server")) == 0) {
		server();
	}
	else if (argc == 3 && lstrcmpi(szCommand, TEXT("client")) == 0) {
		char c_szText[100];
		//wcstombs(c_szText, argv[2], wcslen(argv[2]) + 1);
		client((char *)argv[2]);
	}
	else if (argc == 2 && lstrcmpi(szCommand, TEXT("install")) == 0)
	{
		SvcInstall();
	}
	else if (argc == 3 && lstrcmpi(szCommand, TEXT("start")) == 0)
		DoStartSvc(szSvcName);
	else if (argc == 3 && lstrcmpi(szCommand, TEXT("dacl")) == 0)
		DoUpdateSvcDacl(szSvcName);
	else if (argc == 3 && lstrcmpi(szCommand, TEXT("stop")) == 0)
		DoStopSvc(szSvcName);
	else if (argc == 3 && lstrcmpi(szCommand, TEXT("delete")) == 0)
		DoDeleteSvc(szSvcName);
	else
	{
		_tprintf(TEXT("Unknown command (%s)\n\n"), szCommand);
		DisplayUsage();
	}

	// TO_DO: Add any additional services for the process to this table.
	SERVICE_TABLE_ENTRY DispatchTable[] =
	{
		{ SVCNAME, (LPSERVICE_MAIN_FUNCTION)SvcMain },
		{ NULL, NULL }
	};

	// This call returns when the service has stopped. 
	// The process should simply terminate when the call returns.

	if (!StartServiceCtrlDispatcher(DispatchTable))
	{
		SvcReportEvent(TEXT("StartServiceCtrlDispatcher"));
	}
	getchar();
	return 0;
}


