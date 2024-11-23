#include "mod_SD.h"
#include "App.h"
#include <cstring>
#include <cstdarg>
#include <sys/stat.h> 
#include "ff.h"

void mod_SD::Init()
{
    Mount();

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
    // Perform any periodic SD card tasks if needed
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
    if (!mounted)
        return false;

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
    if (!mounted)
        return false;

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

#include <assert.h>
#include <string.h>
//
#include "my_debug.h"
//
#include "hw_config.h" //from SD library
//
#include "ff.h" /* Obtains integer types */
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

        // .baud_rate = 1 * 100 * 1000 //I suppose I need to put pull-up resistors on the SPI lines to go faster
        // .baud_rate = 12500 * 1000
        .baud_rate = 25 * 1000 * 1000 // Actual frequency: 20833333.
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

extern "C" {

// Simple file descriptor management
static FIL open_files[10]; // Support up to 10 open files

int _open(const char *pathname, int flags, ...) {
    for (int i = 0; i < 10; ++i) {
        if (open_files[i].obj.fs == nullptr) { // Changed from open_files[i].fs_type
            va_list args;
            va_start(args, flags);
            FRESULT res = f_open(&open_files[i], pathname, flags);
            va_end(args);
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
    if (fd < 0 || fd >= 10 || open_files[fd].obj.fs == nullptr) { // Changed
        return -1;
    }
    UINT bytesRead;
    FRESULT res = f_read(&open_files[fd], buffer, count, &bytesRead);
    return (res == FR_OK) ? bytesRead : -1;
}

ssize_t _write(int fd, const void *buffer, size_t count) {
    fd -= 3;
    if (fd < 0 || fd >= 10 || open_files[fd].obj.fs == nullptr) { // Changed
        return -1;
    }
    UINT bytesWritten;
    FRESULT res = f_write(&open_files[fd], buffer, count, &bytesWritten);
    return (res == FR_OK) ? bytesWritten : -1;
}

int _close(int fd) {
    fd -= 3;
    if (fd < 0 || fd >= 10 || open_files[fd].obj.fs == nullptr) { // Changed
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
