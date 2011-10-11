#include <iostream>
#include "src/amods.h"

using namespace std;

int main(int argc, char **argv) {
  amods::AMoDS amods;
  int param = 0;
  int run = amods.Start((void*)param);
  int end = amods.WaitUntilEnd();
  cout << "out: " << end << "\n";
  return 0;
}
