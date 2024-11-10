#include "mod_SD.h"
#include "App.h"
#include <cstring>

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

        .baud_rate = 1 * 100 * 1000 //I suppose I need to put pull-up resistors on the SPI lines to go faster
        // .baud_rate = 12500 * 1000
        // .baud_rate = 25 * 1000 * 1000 // Actual frequency: 20833333.
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
