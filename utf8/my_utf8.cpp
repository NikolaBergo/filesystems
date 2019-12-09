#include <iostream>

#include "my_utf8.h"

/*
 * UTF-8 code and decode implementation
 */

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

static int check_bytes_code_num(uint32_t val) {
    if (val <= unicode_to_one_byte)
        return 1;

    if (val <= unicode_to_two_bytes)
        return 2;

    if (val <= unicode_to_three_bytes)
        return 3;

    if (val <= unicode_to_four_bytes)
        return 4;

    return -1;
}

static int check_bytes_decode_num(uint8_t val) {
    if ((val >> 7u) == 0)
        return 1;
    if ((val & two_byte_code_lead) == two_byte_code_lead)
        return 2;
    if ((val & three_byte_code_lead) == three_byte_code_lead)
        return 3;
    if ((val & four_byte_code_lead) == four_byte_code_lead)
        return 4;

    return -1;
}

static void code_to_utf8(std::vector<uint8_t>& bytes, size_t size, uint32_t unicode_num) {
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

static int decode_from_utf8(std::vector<uint8_t>& bytes) {
    size_t size = bytes.size();
    uint32_t unicode_num = 0;

    if (size == 1) {
        if ((bytes[0] & one_byte_code_lead) != one_byte_code_lead)
            return -1;
        unicode_num = uint32_t(bytes[0]);
        return unicode_num;
    }
    else if (size == 2) {
        if ((bytes[0] & two_byte_code_lead) != two_byte_code_lead)
            return -1;
        unicode_num = uint32_t(bytes[0] & two_byte_code_useful) << rest_byte_useful_size*(size-1);
    }
    else if (size == 3) {
        if ((bytes[0] & three_byte_code_lead) != three_byte_code_lead)
            return -1;
        unicode_num = uint32_t(bytes[0] & three_byte_code_useful) << rest_byte_useful_size*(size-1);
    }
    else if (size == 4) {
        if ((bytes[0] & four_byte_code_lead) != four_byte_code_lead)
            return -1;
        unicode_num = uint32_t(bytes[0] & four_byte_code_useful) << rest_byte_code_useful*(size-1);
    }

    for (int i = 1; i < size; i++) {
        if ((bytes[i] & rest_byte_code) != rest_byte_code)
            return -1;

        unicode_num |= (uint32_t(bytes[i] & rest_byte_code_useful) << rest_byte_useful_size * (size - i - 1));
    }

    return unicode_num;
}

std::pair<std::vector<uint8_t>, int> to_utf8(const std::vector<uint32_t>& x) {
    std::vector<uint8_t> retval;
    std::vector<uint8_t> bytes(4);

    for (size_t i = 0; i < x.size(); i++) {
        int bytes_num = check_bytes_code_num(x[i]);

        if (bytes_num < 0) {
            std::cout << "invalid unicode value input[" << i << "] = " << x[i] << std::endl;
            return {retval, -1};
        }
        else {
            code_to_utf8(bytes, bytes_num, x[i]);

            for (size_t j = 0; j < bytes_num; j++) {
                retval.push_back(bytes[j]);
            }
        }
    }

    return {retval, 0};
}

std::pair<std::vector<uint32_t>, int> from_utf8(const std::vector<uint8_t> &x) {
    std::vector<uint32_t> retval;
    int err = 0;
    size_t i = 0;

    while (i < x.size()) {
        int bytes_num = check_bytes_decode_num(x[i]);

        if (bytes_num < 0) {
            std::cout << "invalid utf8 leading byte: input[" << i << "] = " << x[i] << std::endl;
            return {retval, -1};
        }
        else {
            std::vector<uint8_t> bytes(x.begin()+i, x.begin()+i+bytes_num);
            int unicode_num = decode_from_utf8(bytes);

            if (unicode_num < 0) {
                std::cout << "invalid utf8 sequence starting at: input[" << i << "] = " << x[i] << std::endl;
                return {retval, -1};
            }

            retval.push_back(uint32_t(unicode_num));
            i += bytes_num;
        }
    }

    return {retval, 0};
}

