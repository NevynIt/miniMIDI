#include "mod_SD.h"
#include "App.h"
#include <cstring>
#include <cstdarg>
#include <sys/stat.h> 
#include "sd_card.h"
#include "mod_Stdio.h"

/*
    switch to https://github.com/carlk3/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico

    Multi owner approach
    - owner 1 - stdio
        - acquire on fopen - recursive
        - release on fclose
    - owner 2 - usb
        - acquire on mount from usb side
        - release on eject from usb side

    reading and writing is with cached buffers
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




static FATFS fs; // File system object

#include <assert.h>
#include <string.h>
//
#include "my_debug.h"
//
#include "hw_config.h" //from SD library
//
// #include "ff.h" /* Obtains integer types */
//
#include "diskio.h" /* Declarations of disk functions */

#include "hwConfig.h" // from miniMIDI

/*
This example assumes the following hardware configuration:

|       | SPI0  | GPIO  | Pin   | SPI       | MicroSD   | Description            |
| ----- | ----  | ----- | ---   | --------  | --------- | ---------------------- |
| MISO  | RX    | 16    | 21    | DO        | DO        | Master In, Slave Out   |
| MOSI  | TX    | 19    | 25    | DI        | DI        | Master Out, Slave In   |
| SCK   | SCK   | 18    | 24    | SCLK      | CLK       | SPI clock              |
| CS0   | CSn   | 17    | 22    | SS or CS  | CS        | Slave (or Chip) Select |
| DET   |       | 22    | 29    |           | CD        | Card Detect            |
| GND   |       |       | 18,23 |           | GND       | Ground                 |
| 3v3   |       |       | 36    |           | 3v3       | 3.3 volt power         |

*/

// Hardware Configuration of SPI "objects"
// Note: multiple SD cards can be driven by one SPI if they use different slave
// selects.
static spi_t spis[] = { // One for each SPI.
    {
        .hw_inst = SPI_SD,       // SPI component
        .miso_gpio = GPIO_SD_RX, // GPIO number (not Pico pin number)
        .mosi_gpio = GPIO_SD_TX,
        .sck_gpio = GPIO_SD_CK,

        // .baud_rate = 1 * 100 * 1000 //I had to disable the semaphores in the sd driver to go faster
        .baud_rate = 25 * 1000 * 1000
    }};

// Hardware Configuration of the SD Card "objects"
static sd_card_t sd_cards[] = { // One for each SD card
    {
        .pcName = "0:",        // Name used to mount device
        .spi = &spis[0],       // Pointer to the SPI driving this card
        .ss_gpio = GPIO_SD_CS, // The SPI slave select GPIO for this SD card
        .use_card_detect = false,
        .card_detect_gpio = 0,  // Card detect
        .card_detected_true = 1 // What the GPIO read returns when a card is
                                // present.
    }};

/* ********************************************************************** */
size_t sd_get_num() { return count_of(sd_cards); }

sd_card_t *sd_get_by_num(size_t num)
{
    assert(num <= sd_get_num());
    if (num <= sd_get_num())
    {
        return &sd_cards[num];
    }
    else
    {
        return NULL;
    }
}

size_t spi_get_num() { return count_of(spis); }

spi_t *spi_get_by_num(size_t num)
{
    assert(num <= spi_get_num());
    if (num <= spi_get_num())
    {
        return &spis[num];
    }
    else
    {
        return NULL;
    }
}

void mod_SD::Init()
{
    sd_init_driver();
    Mount();
    sectors = sd_sectors(&sd_cards[0]);

    if (mounted)
    {
        mMApp.display.println("SD Card mounted");
        printf("SD Card mounted\n");
    }
    else
    {
        mMApp.display.println("No SD Card");
        printf("No SD Card\n");
    }
}

void mod_SD::Tick()
{
    INTERVALCHECK(10000)
    if (locked || mounted) //do not disturb currently running SD access
        return;
    // check for the presence of the SD card every 10 seconds
    sectors = sd_sectors(&sd_cards[0]);
}

void mod_SD::Test()
{
    static bool test_done = false;
    if (test_done)
        return;

    test_done = true;

    if (!mounted)
        return;
}

bool mod_SD::Mount()
{
    FRESULT res = f_mount(&fs, "", 1);
    mounted = (res == FR_OK);
    return mounted;
}

bool mod_SD::Unmount()
{
    FRESULT res = f_mount(nullptr, "", 1);
    mounted = false;
    return (res == FR_OK);
}

bool mod_SD::WriteFile(const std::string &path, const void *data, unsigned int size)
{
    FIL file;
    FRESULT res = f_open(&file, path.c_str(), FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK)
        return false;

    UINT written;
    res = f_write(&file, data, size, &written);
    f_close(&file);

    return (res == FR_OK && written == size);
}

bool mod_SD::ReadFile(const std::string &path, std::string &data)
{
    FIL file;
    FRESULT res = f_open(&file, path.c_str(), FA_READ);
    if (res != FR_OK)
        return false;

    char buffer[256];
    UINT read;
    data.clear();

    while ((res = f_read(&file, buffer, sizeof(buffer), &read)) == FR_OK && read > 0)
    {
        data.append(buffer, read);
    }

    f_close(&file);
    return (res == FR_OK);
}

bool mod_SD::AppendFile(const std::string &path, const void *data, unsigned int size)
{
    FIL file;
    FRESULT res = f_open(&file, path.c_str(), FA_WRITE | FA_OPEN_APPEND);
    if (res != FR_OK)
        return false;

    UINT written;
    res = f_write(&file, data, size, &written);
    f_close(&file);

    return (res == FR_OK && written == size);
}

FF_FILE *mod_SD::fopen(const char *pcFile, const char *pcMode) { return ff_fopen(pcFile, pcMode); }
int mod_SD::fclose(FF_FILE *pxStream) { return ff_fclose(pxStream); }
int mod_SD::stat(const char *pcFileName, FF_Stat_t *pxStatBuffer) { return ff_stat(pcFileName, pxStatBuffer); }
size_t mod_SD::fwrite(const void *pvBuffer, size_t xSize, size_t xItems, FF_FILE *pxStream) { return ff_fwrite(pvBuffer, xSize, xItems, pxStream); }
size_t mod_SD::fread(void *pvBuffer, size_t xSize, size_t xItems, FF_FILE *pxStream) { return ff_fread(pvBuffer, xSize, xItems, pxStream); }
int mod_SD::chdir(const char *pcDirectoryName) { return ff_chdir(pcDirectoryName); }
char *mod_SD::getcwd(char *pcBuffer, size_t xBufferLength) { return ff_getcwd(pcBuffer, xBufferLength); }
int mod_SD::mkdir(const char *pcPath) { return ff_mkdir(pcPath); }
int mod_SD::fputc(int iChar, FF_FILE *pxStream) { return ff_fputc(iChar, pxStream); }
int mod_SD::fgetc(FF_FILE *pxStream) { return ff_fgetc(pxStream); }
int mod_SD::rmdir(const char *pcDirectory) { return ff_rmdir(pcDirectory); }
int mod_SD::remove(const char *pcPath) { return ff_remove(pcPath); }
long mod_SD::ftell(FF_FILE *pxStream) { return ff_ftell(pxStream); }
int mod_SD::fseek(FF_FILE *pxStream, int iOffset, int iWhence) { return ff_fseek(pxStream, iOffset, iWhence); }
int mod_SD::findfirst(const char *pcDirectory, FF_FindData_t *pxFindData) { return ff_findfirst(pcDirectory, pxFindData); }
int mod_SD::findnext(FF_FindData_t *pxFindData) { return ff_findnext(pxFindData); }
FF_FILE *mod_SD::truncate(const char *pcFileName, long lTruncateSize) { return ff_truncate(pcFileName, lTruncateSize); }
int mod_SD::seteof(FF_FILE *pxStream) { return ff_seteof(pxStream); }
int mod_SD::rename(const char *pcOldName, const char *pcNewName, int bDeleteIfExists) { return ff_rename(pcOldName, pcNewName, bDeleteIfExists); }
char *mod_SD::fgets(char *pcBuffer, size_t xCount, FF_FILE *pxStream) { return ff_fgets(pcBuffer, xCount, pxStream); }

extern "C" {

// Simple file descriptor management
static FIL open_files[10]; // Support up to 10 open files

int _open(const char *pathname, int flags, int mode) {
    if (mMApp.sd.mounted == false) {
        return -1;
    }

    for (int i = 0; i < 10; ++i) {
        if (open_files[i].obj.fs == nullptr) { // Changed from open_files[i].fs_type
            // va_list args;
            // va_start(args, flags);
            FRESULT res = f_open(&open_files[i], pathname, flags | FA_READ | FA_WRITE);
            // va_end(args);
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

bool mod_SD::cardReady() {
    return sectors > 0;
}

uint32_t mod_SD::getSectorCount() {
    if (mounted)
        return 0;
    return sectors;
}

uint32_t mod_SD::getSectorSize() {
    if (mounted || !sectors)
        return 0;
    return 512; // Typically 512 bytes for SD cards
}

bool mod_SD::lock() {
    if (mounted || !sectors)
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

bool mod_SD::isWriteable() {
    if (!locked)
        return false;
    return !(sd_cards[0].m_Status & 0x04); //STA_PROTECT
}

uint8_t tmpbuffer[512 * 4];

int32_t mod_SD::read(uint32_t sector, uint16_t offset, uint8_t *buffer, uint32_t count) {
    if (!locked)
        return 0;
    int32_t addr = sector * 512 + offset;
    if (addr + count >= sectors * 512)
        return -1;
    int32_t first_sector = addr / 512;
    int32_t num_sectors = (addr + count) / 512 - first_sector + 1;
    sd_cards[0].read_blocks(&sd_cards[0], tmpbuffer, first_sector, num_sectors);
    
    if (addr + count > first_sector * 512 + 512*4)
        count = first_sector * 512 + 512*4 - addr;
    memcpy(buffer, tmpbuffer + offset, count);
    return count;
}

int32_t mod_SD::write(uint32_t sector, uint16_t offset, uint8_t *buffer, uint32_t count) {
    if (!locked)
        return 0;
    if (sector >= getSectorCount() || offset >= getSectorSize())
        return -1;
    sd_cards[0].read_blocks(&sd_cards[0], tmpbuffer, sector, 1);
    int tocopy = getSectorSize() - offset;
    if (tocopy > count)
        tocopy = count;
    memcpy(tmpbuffer + offset, buffer, tocopy);
    sd_cards[0].write_blocks(&sd_cards[0], tmpbuffer, sector, 1);
    return tocopy;
}
