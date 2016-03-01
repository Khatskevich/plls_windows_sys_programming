// ida_disassembling.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

extern int __fastcall min(int a, int b);

int main()
{
	printf("%d/n", min(5, 10));
    return 0;
}
