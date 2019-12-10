#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdexcept>
#include <wchar.h>
#include <bitset>

#include "my_utf8.h"

using namespace std;

int main() {
    wchar_t str[23] = L"Какой-то русский текст";
    int err = 0;
    vector<uint32_t> unicode_stream(22);

    for (int i = 0; i < 22; i++) {
        unicode_stream[i] = uint32_t(str[i]);
    }

    pair<vector<uint8_t>, int> utf8_stream = to_utf8(unicode_stream);
    pair<vector<uint32_t>, int> check = from_utf8(utf8_stream.first);
    cout << check.second << std::endl;

    if (check.first == unicode_stream)
        cout << "YES!\n";

    return 0;
}