#ifndef MOD_CONFIG_H
#define MOD_CONFIG_H

#include "Module.h"
#include <string>
#include <stdint.h>

//pointers in configuration can only be:
//  0x0... - nullptr or boot rom information
//  0x1... - flash data, as long as it's within the cfg area, pointers to other areas raise errors
//         - these will be replaced by 0x6 pointers when the block is written to flash
//  0x2... - ram data, as long as it's within the space of the block while it's in ram
//         - these will be replaced by 0x6 pointers when the block is written to flash
//         - any other 0x2 pointer will raise an error when the block is written to flash
//  0x4... - peripherals (pointing directly to things like pio1, uart0, etc...)
//  0x5... - other pointers to peripherals
//  0x6... - equivalent to 0x1... with no replacement needed
//  0xE... - pointers to the ARM PPB
//anything else means the word is not a pointer and will be left alone after a warning
//even in structures where a pointer would be expected

enum cfgClassID : uint8_t
{
    cfgNoPointers,      //there are no pointers in the block of data
    cfgPointers,        //the data is only pointers
    cfgAlternating,     //the data is an alternation of a pointer and a word of data (which is not checked for pointer rules)
    cfgTagged,          //the data is a tagged structure
                        //every word is checked for pointer rules, but:
                        //every time the first 4 bits of the word do not indicate a valid pointer:
                        //  if the first 4 bits are 0xF, the rest of the word is a "skip" tag
                        //  the second 8 bits are taken as the number of words to skip (up to 1KB)
                        //  the rest is user data, and is not checked for pointer rules
                        //  if the skip word would lead outside the block, an error is raised
                        //  if the first 4 bits are anything else, an error is raised
    cfgLast
};

template<typename T, uint16_t N, uint8_t v = 0>
struct __packed tagged_array
{
    public:
    static_assert((sizeof(T) * N) % 4 == 0, "tagged_array size must be a multiple of 4 bytes");
    static_assert((sizeof(T) * N) < 1024, "tagged_array size must be less than 1KB");
    uint8_t __tag = 0xF0 | v; //"skip block" tag
    uint8_t __skip = (sizeof(T) * N) / 4; //lenght of the array
    uint8_t x, y; //padding to 4 bytes, which can be used for user data
    T data[N]; //the actual array
};

struct __packed tagged_block_example
{   //be very careful with word alignment
    void *ptr1; //needs to be initialised to a valid pointer before writing to flash
    void *ptr2;
    // void *p = pio1; //this would work, but I did not include the header for pio1
    struct __packed {
        uint8_t s00_tag = 0xF0; //keep this as an invalid pointer tag
        uint8_t s00_skip = 10;
        uint8_t something;
        uint8_t another;
        uint32_t s_00_data[10*1];
    };
    void *ptr3;
    struct __packed {
        uint8_t s01_tag = 0xF4; //keep this as an invalid pointer tag
        uint8_t s01_skip = 10;
        int16_t whatever;
        uint16_t s_01_data[10*2];
    };
    uint8_t s02_tag = 0xF2; //keep this as an invalid pointer tag
    uint8_t s02_skip = 8;
    int16_t whatever_else;
    char s_02_data[8*4];
    tagged_array<int16_t, 10, 3> s03;
    void *ptrArray[10];
};

template<typename T>
uint8_t getClassID() {
    return cfgNoPointers; //default type, no pointers
}

struct block;

class mod_config : public Module {
public:
    void Init() override;
    void Tick() override;
    void Test() override;

    virtual const char **GetCommands() const override;
    virtual int DoCommand(int i, lua_State *L) override;

    const block *find(uint8_t type, const char *name) const;
    void defrag();
    void format(int blks = 0);

    template<typename T>
    T *SetConfig(const std::string& key, const T& value)
    {
        return (T *)SetConfig(key.c_str(), getClassID<T>(), &value, sizeof(T));
    }

    template<typename T>
    T *SetConfig(const char *key, const T& value)
    {
        return (T *)SetConfig(key, getClassID<T>(), &value, sizeof(T));
    }

    template<typename T>
    T *GetConfig(const std::string& key) const
    {
        return (T *)GetConfig(key.c_str(), getClassID<T>());
    }

    template<typename T>
    T *GetConfig(const char *key) const
    {
        return (T *)GetConfig(key, getClassID<T>());
    }

    const void *GetConfig(const char *key, uint8_t id) const;
    const void *SetConfig(const char *key, uint8_t id, const void *value, uint16_t size);

public:
    const block *cfg_start = nullptr;
    const block *cfg_end = nullptr;
    const block *cfg_free = nullptr;
    bool valid = true;
};

#endif // MOD_CONFIG_H