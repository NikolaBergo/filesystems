#ifndef UNTITLED_MY_UTF8_H
#define UNTITLED_MY_UTF8_H

#include <vector>

std::pair<std::vector<uint8_t>, int> to_utf8(const std::vector<uint32_t>& x);

std::pair<std::vector<uint32_t>, int> from_utf8(const std::vector<uint8_t> &x);

#endif //UNTITLED_MY_UTF8_H
