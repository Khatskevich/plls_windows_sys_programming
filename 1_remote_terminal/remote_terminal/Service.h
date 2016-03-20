#pragma once
#define SVCNAME TEXT("SvcName")

VOID SvcInstall(void);
VOID WINAPI SvcCtrlHandler(DWORD);
VOID WINAPI SvcMain(DWORD, LPTSTR *);

VOID ReportSvcStatus(DWORD, DWORD, DWORD);
VOID SvcInit(DWORD, LPTSTR *);
VOID SvcReportEvent(LPTSTR);
VOID __stdcall DoDeleteSvc(TCHAR* szSvcName);



VOID __stdcall DoStartSvc(TCHAR* szSvcName);
VOID __stdcall DoUpdateSvcDacl(TCHAR* szSvcName);
VOID __stdcall DoStopSvc(TCHAR* szSvcName);