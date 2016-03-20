#include "stdafx.h"
#include "Utils.h"


VOID __stdcall DisplayUsage()
{
	printf("Description:\n");
	printf("\tCommand-line tool that controls a service.\n\n");
	printf("Usage:\n");
	printf("\tsvccontrol [command] [service_name]\n\n");
	printf("\t[command]\n");
	printf("\t  start\n");
	printf("\t  dacl\n");
	printf("\t  stop\n");
}