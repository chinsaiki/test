#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>

using namespace std;

//g++ demo1.cc -ltcmalloc

//g++ -O3 -ltcmalloc -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free demo1.cc


class BigNumber
{
public:
    BigNumber(int i)
    {
      cout << "BigNumber(" << i  << ")" << endl;
      digits = new char[100000];
    }

    ~BigNumber()
    {
      if (digits != NULL)
        delete[] digits;
    }

private:
    char* digits = NULL;

};

int main() {
  cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

  vector<BigNumber*> v;

  for(int i=0; i< 100; i++)
  {
    v.push_back(new BigNumber(i));
  }

  return 0;
}

