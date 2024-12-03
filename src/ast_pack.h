#pragma once
#include <vector>
#include <cstdint>

class field_info
{
public:
    int count = 1;
    char type = 0; //0 = invalid, 1 = bitfield, 2 = struct, other chars are the type
    union
    {
        std::vector<uint8_t> *bitfield = nullptr;
        std::vector<field_info *> *fields;
    };
    int array_size = 0; //0 = not an array, >0 = array size

    ~field_info()
    {
        if (type == 1)
        {
            delete bitfield;
        }
        else if (type == 2)
        {
            delete fields;
        }
    }
};

field_info *parse_pack_format_string(const char *fmt);
void print_field(field_info *f, const char *indent = "");
