#include "sysy.h"

#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>

int getint() {
  int t;
  scanf("%d", &t);
  return t;
}

int getch() {
  char c;
  scanf("%c", &c);
  return (int)c;
}

int getarray(int a[]) {
  int n;
  scanf("%d", &n);
  for (int i = 0; i < n; i++)
    scanf("%d", &a[i]);
  return n;
}

void putint(int a) { printf("%d", a); }

void putch(int a) { printf("%c", a); }

void putarray(int n, int a[]) {
  printf("%d:", n);
  for (int i = 0; i < n; i++)
    printf(" %d", a[i]);
  printf("\n");
}

__attribute((destructor)) void after_main() {
  if (timing)
    fprintf(stderr, "%ld\n",
            1000000 * (End.tv_sec - Start.tv_sec) + End.tv_usec -
                Start.tv_usec);
}

void _sysy_starttime() {
  gettimeofday(&Start, NULL);
  timing = 1;
}

void _sysy_stoptime() { gettimeofday(&End, NULL); }
