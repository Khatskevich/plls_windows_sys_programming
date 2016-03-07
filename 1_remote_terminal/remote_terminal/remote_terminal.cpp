// remote_terminal.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int run_child_with_redirected_io(int argc, TCHAR *argv[]);

int __cdecl server(void);
int __cdecl client(char *);

int main(int argc, char *argv[])
{
	if (argc == 2 && strcmp(argv[1], "server")==0 ) {
		server();
	}
	if (argc == 3 && strcmp(argv[1], "client")==0) {
		char c_szText[100];
		//wcstombs(c_szText, argv[2], wcslen(argv[2]) + 1);
		client(argv[2]);
	}
    return 0;
}

