#include <iostream>
#include <vector>
#include <stdio.h>
#include "my_utf8.h"
#include <stdexcept>
#include <wchar.h>
#include <bitset>

using namespace std;

int main() {
    wchar_t str[50] = L"Какой-то русский текст";
    vector<uint32_t> unicode_stream(50);
    vector<uint8_t> utf8_stream;
    for (int i = 0; i < 50; i++)
        unicode_stream[i] = uint32_t(str[i]);

    utf8_stream = to_utf8(unicode_stream);
    vector<uint32_t> check;
    check = from_utf8(utf8_stream);
    if (check == unicode_stream)
        cout << "YES!\n";

    return 0;
}