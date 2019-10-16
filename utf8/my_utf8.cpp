#include <iostream>
#include <vector>
#include "my_utf8.h"

const uint8_t  to_utf8_error = 0xFF;
const uint32_t from_utf8_error = 0xFFFFFFFF;


enum byte_masks {
    // leading byte masks depending on UTF-8 bytes quantity
    one_byte_code_lead     = 0b00000000u,
    one_byte_code_useful   = 0b01111111u,

    two_byte_code_lead     = 0b11000000u,
    two_byte_code_useful   = 0b00011111u,

    three_byte_code_lead   = 0b11100000u,
    three_byte_code_useful = 0b00001111u,

    four_byte_code_lead    = 0b11110000u,
    four_byte_code_useful  = 0b00000111u,

    // non-leading byte masks
            rest_byte_code         = 0b10000000u,
    rest_byte_code_useful  = 0b00111111u,
    rest_byte_useful_size  = 6u,
};

// boundary values of Unicode that can be converted to UTF-8
enum byte_num_boundaries {
    unicode_to_one_byte    = 0x00007F,
    unicode_to_two_bytes   = 0x0007FF,
    unicode_to_three_bytes = 0x00FFFF,
    unicode_to_four_bytes  = 0x10FFFF,
};

int check_bytes_code_num(uint32_t val) {
    if (val <= unicode_to_one_byte)
        return 1;

    if (val <= unicode_to_two_bytes)
        return 2;

    if (val <= unicode_to_three_bytes)
        return 3;

    if (val <= unicode_to_four_bytes)
        return 4;

    return -to_utf8_error;
}

int check_bytes_decode_num(uint8_t val) {
    if ((val >> 7u) == 0)
        return 1;
    if ((val & two_byte_code_lead) == two_byte_code_lead)
        return 2;
    if ((val & three_byte_code_lead) == three_byte_code_lead)
        return 3;
    if ((val & four_byte_code_lead) == four_byte_code_lead)
        return 4;

    return -from_utf8_error;
}

void code_to_utf8(std::vector<uint8_t>& bytes, uint32_t unicode_num) {
    size_t size = bytes.size();

    if (size == 1) {
        bytes[0] = one_byte_code_lead |
                   uint8_t(unicode_num & one_byte_code_useful);
        return;
    }
    else if (size == 2) {
        bytes[0] = two_byte_code_lead |
                   uint8_t((unicode_num >> rest_byte_useful_size*(size-1)) & two_byte_code_useful);
    }
    else if (size == 3) {
        bytes[0] = three_byte_code_lead |
                   uint8_t((unicode_num >> rest_byte_useful_size*(size-1)) & three_byte_code_useful);
    }
    else if (size == 4) {
        bytes[0] = four_byte_code_lead |
                   uint8_t((unicode_num >> rest_byte_useful_size*(size-1)) & four_byte_code_useful);
    }

    for (int i = 1; i < size; i++)
        bytes[i] = rest_byte_code |
                   uint8_t((unicode_num >> rest_byte_useful_size*(size-i-1)) & rest_byte_code_useful);
}

void decode_from_utf8(std::vector<uint8_t>& bytes, uint32_t& unicode_num) {
    size_t size = bytes.size();

    if (size == 1) {
        unicode_num = uint32_t(bytes[0]);
        return;
    }
    else if (size == 2) {
        unicode_num = uint32_t(bytes[0] & two_byte_code_useful) << rest_byte_useful_size*(size-1);
    }
    else if (size == 3) {
        unicode_num = uint32_t(bytes[0] & three_byte_code_useful) << rest_byte_useful_size*(size-1);
    }
    else if (size == 4) {
        unicode_num = uint32_t(bytes[0] & four_byte_code_useful) << rest_byte_code_useful*(size-1);
    }

    for (int i = 1; i < size; i++)
        unicode_num |= (uint32_t(bytes[i] & rest_byte_code_useful) << rest_byte_useful_size*(size-i-1));
}

std::vector<uint8_t> to_utf8(const std::vector<uint32_t>& x) {
    std::vector<uint8_t> retval;

    for (auto item : x) {
        int bytes_num = check_bytes_code_num(item);

        if (bytes_num < 0) {
            retval.push_back(to_utf8_error);
        }
        else {
            std::vector<uint8_t> bytes(bytes_num);
            code_to_utf8(bytes, item);

            for (auto byte : bytes)
                retval.push_back(byte);
        }
    }

    return retval;
}

std::vector<uint32_t> from_utf8(const std::vector<uint8_t> &x) {
    std::vector<uint32_t> retval;
    size_t i = 0;

    while (i < x.size()) {
        int bytes_num = check_bytes_decode_num(x[i]);

        if (bytes_num < 0) {
            retval.push_back(from_utf8_error);
            i++;
        }
        else {
            std::vector<uint8_t> bytes(x.begin()+i, x.begin()+i+bytes_num);
            uint32_t unicode_num = 0;
            decode_from_utf8(bytes, unicode_num);
            retval.push_back(unicode_num);
            i += bytes_num;
        }
    }

    return retval;
}

