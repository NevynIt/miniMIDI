#include "mod_SD.h"
#include "App.h"
#include <cstring>
#include <cstdarg>
#include <sys/stat.h> 
#include "mod_Stdio.h"
#include "hw_config.h"

/*
    switch to https://github.com/carlk3/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico -- in progress
    
    Multi owner approach
    - owner 1 - stdio
        - acquire on fopen - recursive
        - release on fclose
    - owner 2 - usb
        - acquire on mount from usb side
        - release on eject from usb side

    reading and writing is with cached buffers: -- this idea is OBE as I'm going to use the SD only from 1 code
    X caches, each Y sectors long - possible values X = 4, Y = 2

    each cache stores the following data:
     - last access counter (monotonic counter)
     - address (-1 if not valid)
     - dirty flag (0 contents fresh from the sd, 1 contents under update, 2 contents ready for upload)
     - data buffer
    
    keep static variables with
        - the address of the current transfer (-1 initially)
        - the cache where the transfer goes
        - the last access counter (0 initially)
        - the transfer queue
    
    cache_test(addr, use)
    {
        mutex
        {
            if one of the cache slots holds addr
                if dirty flag == 0 or 2 
                    if last access < counter
                        set last access to ++counter
                    if use is writing
                        set dirty flag to 1
                    return the data buffer
        }
        return nullptr
    }

    start_transfer
    {
        if dma is not active
            mutex
            {
                if any of the caches is dirty
                    set the address to -1 so it cannot be read or written while it is being transfered
                    set the dirty flag to 0
                    set the dma to write that cache back to the sd card
                else if something is in the queue
                    find the cache slot with the smallest last access
                    set the address to -1
                    set the dirty flag to 0
                    set last access to ++counter
                    set the current transfer address
                    set the dma to transfer from the address in queue to that buffer
            }
    }

    dma_interrupt_handler (once the transfer is complete)
    {
        set the cache address to the transfer address
        set the transfer address to -1
        release exactly one semaphore permit
    }

    read(int addr, int count, char *dst, bool blocking)
    {
        if the card is not ready or the request is wrong (addr + count > size)
            return -1
        initial_count = count

        while count > 0
            while addr is in cache - call cache_test(addr, for_reading)
                calculate how much data can be copied from that cache
                copy data from cache to dst
                increment addr
                decrement count 
            if count > 0
                queue addr for sd transfer
                if blocking
                    if we are on the core that owns SD access (core 0 for miniMIDI)
                        start_transfer
                    wait at the semaphore
                    while the address is not in the cache - call cache_test(addr, for_reading)
                        release one semaphore permit
                        sleep for 100ms
                        wait at the semaphore
        return initial_count - count
    }

    sd_tick -- at every tick
    {
        start_transfer
    }
    
    write(int addr, int count, char *src, bool blocking)
    {
        if the card is not ready or the request is wrong (addr + count > size)
            return -1
        initial_count = count
        
        while count > 0
            while addr is in cache - call cache_test(addr, for_writing)
                calculate how much data can be copied to that cache
                copy data from src to cache
                increment addr
                decrement count
                set dirty flag to 2
            if count > 0
                queue addr for sd transfer
                if blocking
                    if we are on the core that owns SD access (core 0 for miniMIDI)
                        start_transfer
                    wait at the semaphore
                    while the address is not in the cache - call cache_test(addr, for_reading)
                        release one semaphore permit
                        sleep for 100ms
                        wait at the semaphore
        return initial_count - count
    }
*/

void mod_SD::Init()
{
    auto sd = sd_get_by_num(0);
    sd->spi_if_p->spi->sck_gpio = mMApp.hwConfig->gpio_sd_ck;
    sd->spi_if_p->spi->mosi_gpio = mMApp.hwConfig->gpio_sd_tx;
    sd->spi_if_p->spi->miso_gpio = mMApp.hwConfig->gpio_sd_rx;
    sd->spi_if_p->ss_gpio = mMApp.hwConfig->gpio_sd_cs;
    sd->spi_if_p->spi->baud_rate = mMApp.hwConfig->spi_baud_rate;
    sd->spi_if_p->spi->hw_inst = get_spi_instance(mMApp.hwConfig->gpio_sd_rx, mMApp.hwConfig->gpio_sd_ck, mMApp.hwConfig->gpio_sd_tx);
    sd_init_driver();

    if (Mount())
    {
        mMApp.display.println("SD Card mounted");
        printf("SD Card mounted\n");
        Unmount();
    }
    else
    {
        mMApp.display.println("No SD Card");
        printf("No SD Card\n");
    }

}

void mod_SD::Tick()
{
    // INTERVALCHECK(10000)
    // if (locked || mounted) //do not disturb currently running SD access
    //     return;
}

void mod_SD::Test()
{
    // static bool test_done = false;
    // if (test_done)
    //     return;

    // test_done = true;

    // if (!mounted)
    //     return;
}

static FATFS fs;

bool mod_SD::incr_count()
{
    if (open_count == 0)
    {
        if (locked)
            return false;
        FRESULT res = f_mount(&fs, "", 1);
        if (res != FR_OK)
            return false;
    }
    open_count++;
    return true;
}

bool mod_SD::decr_count()
{
    if (open_count == 0)
    {
        LOG_ERROR("open_count underflow");
        return false;
    }
    open_count--;
    if (open_count == 0)
    {
        FRESULT res = f_mount(nullptr, "", 1);
        if (res != FR_OK)
            return false;
    }
    return true;
}

bool mod_SD::Mount()
{
    return incr_count();
}

bool mod_SD::Unmount()
{
    return decr_count();
}

bool mod_SD::WriteFile(const char *path, const void *data, unsigned int size)
{
    auto f = fopen(path, "w");
    if (f == nullptr)
        return false;

    size_t written = fwrite(data, 1, size, f);
    fclose(f);
    return written == size;
}

const std::string mod_SD::ReadFile(const char *path)
{
    auto f = fopen(path, "r");
    if (f == nullptr)
        return std::string();

    std::string data;
    char buffer[256];
    while (true)
    {
        size_t read = fread(buffer, 1, sizeof(buffer), f);
        if (read == 0)
            break;
        data.append(buffer, read);
    }
    fclose(f);
    return data;
}

bool mod_SD::AppendFile(const char *path, const void *data, unsigned int size)
{
    auto f = fopen(path, "a");
    if (f == nullptr)
        return false;

    size_t written = fwrite(data, 1, size, f);
    fclose(f);
    return written == size;
}

FF_FILE *mod_SD::fopen(const char *pcFile, const char *pcMode)
{
    if (!incr_count())
        return nullptr;

    auto f = ff_fopen(pcFile, pcMode);

    if (f == nullptr)
        decr_count();
    return f;
}

FF_FILE *mod_SD::truncate(const char *pcFileName, long lTruncateSize) {
    if (!incr_count())
        return nullptr;
    
    auto f = ff_truncate(pcFileName, lTruncateSize);

    if (f == nullptr)
        decr_count();
    return f;
}

int mod_SD::fclose(FF_FILE *pxStream)
{
    if (pxStream == nullptr)
        return -1;

    int res = ff_fclose(pxStream);
    if (res != RES_OK)
        LOG_ERROR("fclose failed");

    decr_count();
    return res;
}

//these require an open file, so they are protected by open_count
size_t mod_SD::fwrite(const void *pvBuffer, size_t xSize, size_t xItems, FF_FILE *pxStream) { return ff_fwrite(pvBuffer, xSize, xItems, pxStream); }
size_t mod_SD::fread(void *pvBuffer, size_t xSize, size_t xItems, FF_FILE *pxStream) { return ff_fread(pvBuffer, xSize, xItems, pxStream); }
int mod_SD::fputc(int iChar, FF_FILE *pxStream) { return ff_fputc(iChar, pxStream); }
int mod_SD::fgetc(FF_FILE *pxStream) { return ff_fgetc(pxStream); }
long mod_SD::ftell(FF_FILE *pxStream) { return ff_ftell(pxStream); }
int mod_SD::fseek(FF_FILE *pxStream, int iOffset, int iWhence) { return ff_fseek(pxStream, iOffset, iWhence); }
int mod_SD::seteof(FF_FILE *pxStream) { return ff_seteof(pxStream); }
char *mod_SD::fgets(char *pcBuffer, size_t xCount, FF_FILE *pxStream) { return ff_fgets(pcBuffer, xCount, pxStream); }

//these need a mounted filesystem
int mod_SD::stat(const char *pcFileName, FF_Stat_t *pxStatBuffer)
{
    if (!Mounted())
        return -1;

    return ff_stat(pcFileName, pxStatBuffer);
}

int mod_SD::chdir(const char *pcDirectoryName) {
    if (!Mounted())
        return -1;
    return ff_chdir(pcDirectoryName);
}

char *mod_SD::getcwd(char *pcBuffer, size_t xBufferLength) {
    if (!Mounted())
        return nullptr;
    return ff_getcwd(pcBuffer, xBufferLength);
}

int mod_SD::mkdir(const char *pcPath) {
    if (!Mounted())
        return -1;
    return ff_mkdir(pcPath);
}

int mod_SD::rmdir(const char *pcDirectory) {
    if (!Mounted())
        return -1;
    return ff_rmdir(pcDirectory);
}

int mod_SD::remove(const char *pcPath) {
    if (!Mounted())
        return -1;
    return ff_remove(pcPath);
}

int mod_SD::findfirst(const char *pcDirectory, FF_FindData_t *pxFindData) {
    if (!Mounted())
        return -1;
    return ff_findfirst(pcDirectory, pxFindData);
}

int mod_SD::findnext(FF_FindData_t *pxFindData) {
    if (!Mounted())
        return -1;
    return ff_findnext(pxFindData);
}

int mod_SD::rename(const char *pcOldName, const char *pcNewName, int bDeleteIfExists) {
    if (!Mounted())
        return -1;
    return ff_rename(pcOldName, pcNewName, bDeleteIfExists);
}

extern "C" {

#include <fcntl.h>
int fcntl_to_ff(int flags, int mode)
{
    int ff_mode = FA_READ;

    if (flags & O_RDONLY) {
        ff_mode |= FA_READ;
    }
    if (flags & O_WRONLY) {
        ff_mode |= FA_WRITE;
    }
    if (flags & O_RDWR) {
        ff_mode |= FA_READ | FA_WRITE;
    }
    if (flags & O_CREAT) {
        ff_mode |= FA_CREATE_ALWAYS;
    }
    if (flags & O_EXCL) {
        ff_mode |= FA_CREATE_NEW;
    }
    if (flags & O_TRUNC) {
        ff_mode |= FA_CREATE_ALWAYS;
    }
    if (flags & O_APPEND) {
        ff_mode |= FA_OPEN_APPEND;
    }

    return ff_mode;
}

// Simple file descriptor management
static FIL open_files[10] = {0}; // Support up to 10 open files

int _open(const char *pathname, int flags, int mode) {
    for (int i = 0; i < 10; ++i) {
        if (open_files[i].obj.fs == nullptr) { 
            int ff_mode = 0;
            ff_mode = fcntl_to_ff(flags, mode);
            FRESULT res = f_open(&open_files[i], pathname, ff_mode);
            if (res == FR_OK) {
                return i + 3;
            }
            return -1;
        }
    }
    return -1; // No available file descriptors
}

ssize_t _read(int fd, void *buffer, size_t count) {
    fd -= 3;
    if (fd < 0 || fd >= 10 || open_files[fd].obj.fs == nullptr) {
        return -1;
    }
    UINT bytesRead;
    FRESULT res = f_read(&open_files[fd], buffer, count, &bytesRead);
    return (res == FR_OK) ? bytesRead : -1;
}

ssize_t _write(int fd, const void *buffer, size_t count) {
    if (fd == 1 || fd == 2) {
        // Redirect stdout and stderr to UART
        mMApp.uart.out(buffer, count);
        return count;
    }

    fd -= 3;
    if (fd < 0 || fd >= 10 || open_files[fd].obj.fs == nullptr) {
        return -1;
    }
    UINT bytesWritten;
    FRESULT res = f_write(&open_files[fd], buffer, count, &bytesWritten);
    return (res == FR_OK) ? bytesWritten : -1;
}

int _close(int fd) {
    fd -= 3;
    if (fd < 0 || fd >= 10 || open_files[fd].obj.fs == nullptr) {
        return -1;
    }
    FRESULT res = f_close(&open_files[fd]);
    if (res == FR_OK) {
        // Clear the file object
        memset(&open_files[fd], 0, sizeof(FIL));
        return 0;
    }
    return -1;
}

int _lseek(int fd, off_t offset, int whence) {
    fd -= 3;
    if (fd < 0 || fd >= 10 || open_files[fd].obj.fs == nullptr) { 
        return -1;
    }
    FSIZE_t new_pos;
    switch (whence) {
        case SEEK_SET:
            new_pos = offset;
            break;
        case SEEK_CUR:
            new_pos = open_files[fd].fptr + offset;
            break;
        case SEEK_END:
            new_pos = open_files[fd].obj.objsize + offset;
            break;
        default:
            return -1;
    }
    FRESULT res = f_lseek(&open_files[fd], new_pos);
    return (res == FR_OK) ? 0 : -1;
}

int _fstat(int fd, struct stat *st) {
    fd -= 3;
    if (fd < 0 || fd >= 10 || open_files[fd].obj.fs == nullptr) { 
        return -1;
    }

    st->st_size = open_files[fd].obj.objsize;
    
    // Set file type based on attributes
    if (open_files[fd].obj.attr & AM_DIR) {
        st->st_mode = S_IFDIR;
    } else {
        st->st_mode = S_IFREG;
    }

    // Set permissions based on read-only attribute
    if (open_files[fd].obj.attr & AM_RDO) {
        st->st_mode |= S_IRUSR | S_IRGRP | S_IROTH;
    } else {
        st->st_mode |= S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    }

    // Set additional stat fields with default or available values
    st->st_nlink = 1;           // Number of hard links
    st->st_uid = 0;              // User ID of owner (not available)
    st->st_gid = 0;              // Group ID of owner (not available)
    st->st_atime = 0;            // Time of last access (not available)
    st->st_mtime = 0;            // Time of last modification (not available)
    st->st_ctime = 0;            // Time of last status change (not available)

    return 0;
}

} // extern "C"

bool mod_SD::lock() {
    if (Mounted())
        return false;
    locked = true;
    return true;
}

bool mod_SD::unlock() {
    if (!locked)
        return false;
    locked = false;
    return true;
}