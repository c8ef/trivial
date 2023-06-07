int main() {
  int n = 0;
  int a = 0;
  int b = 1;
  while (n < 10) {
    a = b;
    b = a;
    n = n + 1;
  }
  return a + b;
}