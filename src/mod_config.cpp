#include "mod_config.h"
#include <stdexcept>
#include <stdint.h>
#include "hardware/flash.h"
#include "pico/multicore.h"
#include "hwConfig.h"
#include <cstring>
#include "App.h"
#include "mod_Stdio.h"

#define FLASH_JEDEC_ID_CMD 0x9f
#define FLASH_JEDEC_ID_TOTAL_BYTES 8

uint32_t flash_size;

static_assert(sizeof(void *) == 4, "This code is for 32-bit pointers only");
#pragma message("mod_config code is very specific for the rp2040")

static void __attribute__((constructor)) getMemSize(void) {
    uint8_t txbuf[FLASH_JEDEC_ID_TOTAL_BYTES] = {0};
    uint8_t rxbuf[FLASH_JEDEC_ID_TOTAL_BYTES] = {0};
    txbuf[0] = FLASH_JEDEC_ID_CMD;
    flash_do_cmd(txbuf, rxbuf, FLASH_JEDEC_ID_TOTAL_BYTES);
    flash_size = 1 << rxbuf[3];
}

//define the struct as packed
#pragma pack(push, 1)
struct block
{
public:
    uint16_t block_size;
    uint8_t used : 1, type : 7;
    uint8_t name_size;

    void set(uint16_t data_size, uint8_t name_size, uint8_t type)
    {
        this->name_size = name_size;
        this->type = type;
        this->used = 1;
        this->block_size = sizeof(block) + data_size + padded_name_size();
    }

    const block *next() const {
        return (const block *)((uint8_t *)this + padded_size());
    }

    const char *name() const {
        return (const char *)((uint8_t *)this + sizeof(block));
    }

    uint8_t padded_name_size() const {
        return (name_size + 3) & ~3;
    }

    const void *data() const {
        return (const void *)((uint8_t *)this + sizeof(block) + padded_name_size());
    }

    uint16_t data_size() const {
        return block_size - sizeof(block) - padded_name_size();
    }
    
    uint16_t padded_size() const {
        return (block_size + 3) & ~3;
    }

    uint8_t waste() const {
        return padded_size() - name_size - sizeof(block) - data_size();
    }
};
#pragma pack(pop)

extern uint8_t __flash_binary_start;
extern uint8_t __flash_binary_end;

#define FLASH_BASE_ADDR_XIP    0x10000000  // Base address of the flash memory for XIP
#define FLASH_BASE_ADDR_CACHED 0x60000000  // Base address of the flash memory, cached access
#define FLASH_BASE_ADDR_UNCACHED 0x80000000  // Base address of the flash memory, cache disabled access
#define FLASH_BASE_ADDR FLASH_BASE_ADDR_CACHED

void mod_config::format(int blks)
{
    // Stop the other core and disable interrupts
    multicore_lockout_start_blocking();
    uint32_t irq_status = save_and_disable_interrupts();

    // Erase the configuration memory
    uint32_t sz = flash_size - FLASH_RESERVED;
    if (blks > 0)
    {
        sz = MIN(blks * 4096, sz);
    }
    flash_range_erase(FLASH_RESERVED, sz);

    // Re-enable interrupts and restart the other core
    restore_interrupts(irq_status);
    multicore_lockout_end_blocking();
}

void mod_config::Init() {
    cfg_start = (const block *)(FLASH_BASE_ADDR + FLASH_RESERVED);
    cfg_end = (const block *)(FLASH_BASE_ADDR + flash_size);

    if (FLASH_RESERVED > flash_size)
    {
        LOG_ERROR("Error: The reserved space is larger than the flash memory size, no configuration available.\n");
    }
    else if (cfg_start < (void *)&__flash_binary_end)
    {
        LOG_ERROR("Error: The code size exceeded the reserved space, the configuration buffer is now corrupted.\n");
    }
    else
    {
        LOG_INFO("Flash memory size: %d\n", flash_size);
        LOG_INFO("Reserved space for code: %d\n", FLASH_RESERVED);
        //walk the configuration blocks to gather statistics: how many blocks, how many bytes used, how many bytes free, how many bytes ready to be recycled, how many bytes wasted
        int used = 0, recycle = 0, wasted = 0, count_used = 0, count = 0;
        const block *b = cfg_start;
        while (b < cfg_end)
        {
            if (b->block_size == 0) //cfg is corrupted badly, disable
            {
                LOG_ERROR("Error: The configuration buffer is corrupted, disabling the configuration module.\n");
                valid = false;
                return;
            }
            if (b->block_size == 0xFFFF) //end of the configuration data
            {
                break;
            }
            if (b->used)
            {
                count_used++;
                used += b->padded_size();
                wasted += b->waste();
            }
            else
            {
                recycle += b->padded_size();
            }
            count++;
            b = b->next();
        }
        cfg_free = b;
        int free = (uint32_t)cfg_end - (uint32_t)b;
        LOG_INFO("%d blocks in use (%d bytes)\n%d blocks ready to be recycled (%d bytes)\n", count_used, used, count-count_used, recycle);
        LOG_INFO("%d bytes wasted\n%d bytes free\n", wasted, free);
    }
}

void mod_config::Tick() {
    // Perform any periodic configuration tasks if needed
}

void mod_config::defrag()
{
    LOG_WARNING("Defragging the configuration memory, all pointers, live and stored will be invalid now!!\n");
    uint8_t blk[4096];
    //initialise the block to 0xFFFF
    memset(blk, 0xFF, 4096);

    LOG_PANIC("Defrag not implemented yet\n");

    // new idea for defrag:
    // defrag does 3 passes:
    // - pass 1, scan through the cfg memory and copy all the used blocks, as they are, to a file on the sd
    //   keep track of the position of each
    // - pass 2, scan through this file, and for each pointer, find the block and the offset within the block
    //   if the block is active, replace the pointer with a pointer to the updated position of the same block
    //   if the block is not active, copy the inactive block at the end of the file (to be processed as well)
    //      and replace the pointer with a pointer to the new position of the inactive block
    // - pass 3, find the first flash block where it differs from the temporary file
    //   clear from there until the end, then start copying the contents from the file

    // const block *b = cfg_start;
    // uint32_t offset = 0;
    // uint32_t mem_offset = (uint32_t)cfg_start - FLASH_BASE_ADDR;
    // while (1)
    // {
    //     if (b->block_size == 0xFFFF) //end of the configuration data
    //     {
    //         uint32_t page_end = ((uint32_t)cfg_free + 4095) & ~4095;
    //         if (offset == 0)
    //         {
    //             printf("No blocks to keep\n");
    //             //clear all the configuration memory up to the block that contains cfg_free
    //             //find the end of the page just before cfg_free
    //             //clear the memory
    //             multicore_lockout_start_blocking();
    //             uint32_t irq_status = save_and_disable_interrupts();
    //             flash_range_erase(FLASH_RESERVED, page_end - (uint32_t)cfg_start);
    //             restore_interrupts(irq_status);
    //             multicore_lockout_end_blocking();
    //             cfg_free = cfg_start;
    //             return;
    //         }
    //         //clear the memory between mem_offset and the end of the page
    //         multicore_lockout_start_blocking();
    //         uint32_t irq_status = save_and_disable_interrupts();
    //         flash_range_erase(mem_offset, page_end - FLASH_BASE_ADDR - mem_offset);
    //         if (offset > 0)
    //         {
    //             //program the last block
    //             flash_range_program(mem_offset, blk, 4096);
    //         }
    //         restore_interrupts(irq_status);
    //         multicore_lockout_end_blocking();
    //         return;
    //     }
    //     if (b->used)
    //     {
    //         for (int i=0; i<b->padded_size(); i++)
    //         {
    //             blk[offset++] = ((uint8_t *)b)[i];
    //             if (offset == 4096)
    //             {
    //                 //erase the flash
    //                 multicore_lockout_start_blocking();
    //                 uint32_t irq_status = save_and_disable_interrupts();
    //                 flash_range_erase(mem_offset, 4096);
    //                 //write the block to the flash
    //                 flash_range_program(mem_offset, blk, 4096);
    //                 restore_interrupts(irq_status);
    //                 multicore_lockout_end_blocking();
    //                 //move to the next page
    //                 mem_offset += 4096;
    //                 offset = 0;
    //                 //initialise the block to 0xFFFF
    //                 memset(blk, 0xFF, 4096);
    //             }
    //         }
    //     }
    //     b = b->next();
    // }
}

const block *mod_config::find(uint8_t type, const char *name) const
{
    if (!valid)
    {
        return nullptr;
    }

    //walk the configuration blocks to find the block with the given name and type
    const block *b = cfg_start;
    while (b < cfg_free)
    {
        if (b->block_size == 0xFFFF) //end of the configuration data
        {
            break;
        }
        if (b->used && b->type == type && strcmp(b->name(), name) == 0)
        {
            return b;
        }
        b = b->next();
    }
    return nullptr;
}

void recycle(const block *b)
{
    uint8_t page[256];
    block new_block = {.block_size = b->block_size, .used = 0, .type = b->type, .name_size = b->name_size};
    //find the start of the page of b
    uint32_t page_start = (uint32_t)b & ~255;
    //copy the current page to the buffer
    memcpy(page, (void *)page_start, 256);
    uint32_t offset = (uint32_t)b - page_start;
    //copy the new block to the buffer (there is always space, as blocks are always aligned to 4 bytes, and they are 4 bytes long)
    memcpy(page + offset, &new_block, sizeof(block));
    //write the page to the flash
    multicore_lockout_start_blocking();
    uint32_t irq_status = save_and_disable_interrupts();
    flash_range_program(page_start - FLASH_BASE_ADDR, page, 256);
    restore_interrupts(irq_status);
    multicore_lockout_end_blocking();
}

typedef union
{
    void *ptr;
    uint32_t word;
    struct __packed 
    {
        uint8_t type: 4, user_id: 4;
        uint8_t skip;
        uint16_t user_data;
    };
} block_word;

enum ptr_action
{
    ptr_action_copy,
    ptr_action_update,
    ptr_action_warn,
    ptr_action_error,
    ptr_action_skiptag,
};

ptr_action check_pointer(const void *ptr, const void *src, uint16_t size, uint8_t id)
{
    // cfg_start = (const block *)(FLASH_BASE_ADDR + FLASH_RESERVED);
    // cfg_end = (const block *)(FLASH_BASE_ADDR + flash_size);

    const block_word *w = (const block_word *)&ptr;
    switch (w->type)
    {
    case 0x0: //null and boot rom
    case 0x4: //peripherals
    case 0x5: //peripherals
    case 0xE: //ARM PPB
        return ptr_action_copy;
    case 0x1: //flash XIP
        if (w->word >= (FLASH_BASE_ADDR_XIP + FLASH_RESERVED) && w->word < (FLASH_BASE_ADDR_XIP + flash_size))
            return ptr_action_update;
        else
            return ptr_action_error;
    case 0x2: //ram
        if (w->ptr >= src && w->ptr < src + size)
            return ptr_action_update;
        else
            return ptr_action_error;
    case 0x6: //flash XIP cached
        if (w->word >= (FLASH_BASE_ADDR_CACHED + FLASH_RESERVED) && w->word < (FLASH_BASE_ADDR_CACHED + flash_size))
            return ptr_action_copy;
        else
            return ptr_action_error;
    case 0xF: //skip tag
        return ptr_action_skiptag;
    default: //invalid
        return ptr_action_warn;
    }
}

void update_pointer(void **ptr, const void *src, const void *dst, uint16_t size, uint8_t id)
{
    block_word *w = (block_word *)(*ptr);
    switch (w->type)
    {
    case 0x1: //flash XIP
        w->word += FLASH_BASE_ADDR_CACHED - FLASH_BASE_ADDR_XIP;
        break;
    case 0x2: //ram
        w->ptr = (void *)((uint32_t)w->ptr - (uint32_t)src + (uint32_t)dst);
        break;
    }
}

const void *fix_pointers(const void *src, const void *dst, uint16_t size, uint8_t id, const char *key)
{
    switch (id)
    {
        case cfgNoPointers:
            return src;
        case cfgPointers:
        {
            //check if there is any pointer to fix or invalid pointers
            bool doupdate = false;
            for (int i = 0; i < size / 4; i++)
            {
                ptr_action action = check_pointer(((const void **)src)[i], src, size, id);
                switch (action)
                {
                case ptr_action_copy:
                    break;
                case ptr_action_update:
                    doupdate = true;
                    break;
                case ptr_action_warn:
                    LOG_WARNING("Warning: Invalid pointer in %s, word %d, %p\n", key, i, ((const void **)src)[i]);
                    break;
                case ptr_action_error:
                    LOG_ERROR("Error: Invalid pointer in %s, word %d, %p\n", key, i, ((const void **)src)[i]);
                    return nullptr;
                case ptr_action_skiptag:
                    LOG_WARNING("Warning: Invalid skiptag in %s, word %d\n", key, i);
                    break;
                }
            }
            if (doupdate)
            {
                void *new_data = malloc(size);
                if (new_data == nullptr)
                {
                    LOG_ERROR("Error: Not enough memory to fix the pointers in %s\n", key);
                    return nullptr;
                }
                memcpy(new_data, src, size);
                for (int i = 0; i < size / 4; i++)
                {
                    update_pointer(&((void **)new_data)[i], src, new_data, size, id);
                }
                return new_data;
            }
            else
            {
                return src;
            }
        }
        case cfgTagged:
        {
            bool do_update = false;
            //check if there is any pointer to fix or invalid pointers
            for (int i = 0; i < size / 4; i++)
            {
                ptr_action action = check_pointer(((const void **)src)[i], src, size, id);
                switch (action)
                {
                case ptr_action_copy:
                    break;
                case ptr_action_update:
                    do_update = true;
                    break;
                case ptr_action_warn:
                    LOG_WARNING("Warning: Invalid pointer in %s, word %d, %p\n", key, i, ((const void **)src)[i]);
                    break;
                case ptr_action_error:
                    LOG_ERROR("Error: Invalid pointer in %s, word %d, %p\n", key, i, ((const void **)src)[i]);
                    return nullptr;
                case ptr_action_skiptag:
                {
                    auto j = i; //store to log the word number if needed
                    i += ((const block_word *)src)[i].skip + 1;
                    if (i >= size / 4)
                    {
                        LOG_ERROR("Error: Overflow skiptag in %s, word %d\n", key, j);
                        return nullptr;
                    }
                }
                    break;
                }
            }
            if (do_update)
            {
                void *new_data = malloc(size);
                if (new_data == nullptr)
                {
                    LOG_ERROR("Error: Not enough memory to fix the pointers in %s\n", key);
                    return nullptr;
                }
                memcpy(new_data, src, size);
                for (int i = 0; i < size / 4; i++)
                {
                    const block_word *w = (const block_word *)&((const void **)new_data)[i];
                    if (w->type == 0xF)
                    {
                        i += w->skip;
                    }
                    else
                        update_pointer(&((void **)new_data)[i], src, new_data, size, id);
                }
                return new_data;
            }
            else
            {
                return src;
            }
        }
        default:
            LOG_ERROR("Error: Invalid id %d in %s\n", id, key);
            break;
    }
}

const void *mod_config::SetConfig(const char *key, uint8_t id, const void *src, uint16_t size) {
    if (!valid)
    {
        return nullptr;
    }

    const void *value = src;

    const block *b = find(id, key);
    if (b)
    {
        value = fix_pointers(src, b->data(), size, id, key);
        if (value == nullptr)
        {
            return nullptr;
        }
        //check if the data is the same
        if (memcmp(b->data(), value, size) == 0)
        {
            //nothing new is stored, returns the address of the data (not the block)
            if (value != src) free((void *)value);
            return b->data();
        }
        //free this copy, as the block will go somewhere else and dst is different
        if (value != src) free((void *)value);
        value = src;
    }
    //the block will go to the address at cfg_free->data()
    value = fix_pointers(value, cfg_free->data(), size, id, key);
    block new_block;
    auto key_size = strlen(key) + 1;
    new_block.set(size, key_size, id);

    //check if there is enough space for the new block
    if (cfg_end - cfg_free < new_block.padded_size())
    {
        //compact the flash memory
        defrag();
        if (cfg_end - cfg_free < sizeof(block) + key_size + size)
        {
            LOG_PANIC("No space left in the configuration memory");
            if (value != src) free((void *)value);
            return nullptr;
        }
    }

    //store the new data at the end
    uint8_t page[256];
    //find the start of the page of cfg_free
    uint32_t page_start = (uint32_t)cfg_free & ~255;
    //copy the current page to the buffer
    memcpy(page, (void *)page_start, 256);
    uint32_t offset = (uint32_t)cfg_free - page_start;
    //copy the new block to the buffer (there is always space, as blocks are always aligned to 4 bytes, and they are 4 bytes long)
    memcpy(page + offset, &new_block, sizeof(block));
    offset += sizeof(block);
    for (int i = 0; i < key_size; i++)
    {
        if (offset >= 256)
        {
            //write the page to the flash
            multicore_lockout_start_blocking();
            uint32_t irq_status = save_and_disable_interrupts();
            flash_range_program(page_start - FLASH_BASE_ADDR, page, 256);
            restore_interrupts(irq_status);
            multicore_lockout_end_blocking();
            //move to the next page
            page_start += 256;
            offset = 0;
        }
        page[offset++] = key[i];
    }
    offset += new_block.padded_name_size() - key_size;
    for (int i = 0; i < size; i++)
    {
        if (offset >= 256)
        {
            //write the page to the flash
            multicore_lockout_start_blocking();
            uint32_t irq_status = save_and_disable_interrupts();
            flash_range_program(page_start - FLASH_BASE_ADDR, page, 256);
            restore_interrupts(irq_status);
            multicore_lockout_end_blocking();
            //move to the next page
            page_start += 256;
            offset = 0;
        }
        page[offset++] = ((uint8_t *)value)[i];
    }
    if (offset > 0)
    {
        //write the page to the flash
        multicore_lockout_start_blocking();
        uint32_t irq_status = save_and_disable_interrupts();
        flash_range_program(page_start - FLASH_BASE_ADDR, page, 256);
        restore_interrupts(irq_status);
        multicore_lockout_end_blocking();
    }

    if (b)
    { // invalidate the old data
        recycle(b);
        for (b = find(id, key); b != nullptr && b != cfg_free; b = find(id, key))
            recycle(b);
        if (!b)
        {
            LOG_ERROR("Error: The block was not stored: %s\n", key);
            return nullptr;
        }
    }
    auto ptr = cfg_free->data();
    cfg_free = cfg_free->next();
    if (value != src) free((void *)value);
    return ptr;
}

const void *mod_config::GetConfig(const char *key, uint8_t id) const {
    if (!valid)
    {
        return nullptr;
    }

    const block *b = find(id, key);
    if (b)
    {
        return b->data();
    }
    return nullptr;
}

// Logic to save data in the flash memory:
// (the whole memory is initially filled with high bits, writing sets some low, and must be written in blocks of 256, which can be written again, as long that it's always towards low)
// starting from a given address, after space reserved for the code
// the memory is organized in blocks of variable size
// each block is composed by:
// - 16 bit of block size (S)
// - 1 bit of block recycle flag (1 = in use, 0 = memory ready to be recycled)
// - 7 bits of block class id 
// - 8 bit of block name size (N)
// - N bytes for the name (null terminated)
// - S-N-4 bytes of data
// - padding to the next 4 bytes boundary if needed
// ------
// when the memory is so full that the next variable cannot be stored, the whole flash is reorganized, compacting to the right and only keeping the data in use, and clearing everything else
// the commands for config are:
// - void * store(type, name, buffer, size)
// - - if the same name and the same data is there already, nothing is stored, returns the address of the data (not the block)
// - - if the data is there but it's different, first stores the new one and then invalidates the old
// - - if the data is not there, store the new data at the end
// - void * find(type, name) // 

struct testdata
{
    int a;
    int b;
    int c;
    char d[10];
};

template<>
uint8_t getClassID<testdata>() {
    return 1;
}

void mod_config::Test()
{
    static bool test_done = false;
    if (test_done) return;
    test_done = true;

    testdata td = {13, 23, 44, "test"};
    printf("size of testdata: %d\n", sizeof(testdata));
    auto tmp1 = SetConfig("test", td);
    auto tmp2 = GetConfig<testdata>("test");
    if (tmp1 != tmp2)
    {
        printf("Error: the data is not the same\n");
    }
    else
    {
        printf("Data stored and retrieved correctly\n");
    }
}