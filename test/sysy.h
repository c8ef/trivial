#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>

int getint();
int getch();
int getarray(int a[]);

void putint(int a);
void putch(int a);
void putarray(int n, int a[]);

static struct timeval Start, End;
static int timing = 0;

__attribute((destructor)) void after_main();

void _sysy_starttime();
void _sysy_stoptime();
