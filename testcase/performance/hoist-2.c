// gcc hoist.c sylib.c -include sylib.h -Wall -Wno-unused-result -o binary-hoist
// && time ./binary-hoist < hoist.in
int loopCount = 0;
int func(int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8, int i9,
         int i10, int i11, int i12, int i13, int i14, int i15) {
  int result = 0;
  int i = 0;
  while (i < loopCount) {
    int sum = 0;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum + i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 +
          i13 + i14 + i15;
    sum = sum / 100;
    result = result + sum;
    result = result % 1500000001;
    i = i + 1;
  }
  return result;
}

int main() {
  loopCount = getint();
  starttime();
  int result = func(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
  stoptime();
  putint(result);
  putch(10);
  return 0;
}
