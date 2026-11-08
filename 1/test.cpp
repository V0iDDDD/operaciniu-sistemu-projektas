#include <iostream>
#include <stdint.h>

using namespace std;

string word_to_ascii(uint32_t w)
{
    string result;
    for (int i = 0; i < 4; i++)
        result += (char)((w >> 8*(3-i)) % 256);
    return result;
}

int main()
{
    int32_t a = -5;
    cout << a << endl;
    uint32_t b = a;
    cout << b << endl;
    // uint32_t* b = (uint32_t*)(&a);
    // cout << *b << endl;

    return 0;
}
