// remote_terminal.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int run_child_with_redirected_io(int argc, TCHAR *argv[]);


int main(int argc, TCHAR *argv[])
{
	run_child_with_redirected_io(argc, argv);
    return 0;
}

