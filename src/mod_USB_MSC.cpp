#include "App.h"
#include "mod_USB_MSC.h"
#include "mod_SD.h"
#include "tusb.h"
#include "diskio.h"

//based on example from https://github.com/udo-munk/RP2040-GEEK-80/blob/main/stdio_msc_usb/msc_usb.c

void mod_USB_MSC::Init()
{

}

void mod_USB_MSC::Tick()
{
}

void mod_USB_MSC::Test()
{
    // Perform any USB MSC tests here
}

/**
 * @brief Reads data from the disk.
 *
 * This callback is invoked when the host sends a READ (10) request to the device.
 *
 * @param[in] lun The logical unit number that the request is for.
 * @param[in] lba The logical block address to read from.
 * @param[in] offset The offset in bytes from the beginning of the block to read from.
 * @param[out] buffer The buffer to store the read data in.
 * @param[in] bufsize The size of the buffer in bytes.
 *
 * @return The number of bytes read from the disk.
 */
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) {
    // LOG_VERBOSE("tud_msc_read10_cb called %d, %d, %d\n", lba, offset, bufsize);
    assert(!(offset % 512));
    assert(!(bufsize % 512));

    if (mMApp.sd.Mounted())
    {
        LOG_WARNING("SD card mounted\n");
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3A, 0x00); // Medium Not Present
        return -1;
    }
    if (!tud_msc_test_unit_ready_cb(lun)) return -1;

    // Read data from the disk.
    DRESULT dr = disk_read(lun, (BYTE*)buffer, lba + offset/512, bufsize / 512);
    if (RES_OK != dr) return -1;

    return (int32_t)bufsize;
}

/**
 * @brief Write data to the disk.
 *
 * @param[in] lun The logical unit number that the request is for.
 * @param[in] lba The logical block address to write to.
 * @param[in] offset The offset in bytes from the beginning of the
 *                    block to write to.
 * @param[in] buffer The buffer containing the data to be written.
 * @param[in] bufsize The size of the buffer in bytes.
 *
 * @return The number of bytes written.
 */
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) {
    // LOG_VERBOSE("tud_msc_write10_cb called\n");
    assert(!(offset % 512));
    assert(!(bufsize % 512));

    if (mMApp.sd.Mounted())
    {
        LOG_WARNING("SD card mounted\n");
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3A, 0x00); // Medium Not Present
        return -1;
    }
    if (!tud_msc_test_unit_ready_cb(lun)) return -1;

    // Write data to the disk.
    DRESULT dr = disk_write(lun, (BYTE*)buffer, lba + offset/512, bufsize / 512);
    if (RES_OK != dr) return -1;

    return bufsize;
}

/**
 * @brief Checks if the unit is ready.
 *
 * @param[in] lun The logical unit number that the request is for.
 *
 * @retval true The unit is ready.
 * @retval false The unit is not ready.
 */
bool tud_msc_test_unit_ready_cb(uint8_t lun) {
    if (mMApp.sd.Mounted()) {
        // LOG_WARNING("SD card is mounted\n");
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3A, 0x00); // Medium Not Present
        return false;
    }

    DSTATUS ds = disk_initialize(lun);
    if (ds & STA_NOINIT || ds & STA_NODISK) {
        // LOG_WARNING("SD card not ready\n");
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3A, 0x00); // Medium Not Present
        return false;
    }

    return true;
}

/**
 * @brief Gets the capacity of the disk.
 *
 * @param[in] lun The logical unit number that the request is for.
 * @param[out] block_count_p The number of blocks in the disk.
 * @param[out] block_size_p The size of each block in bytes.
 *
 * @return true if the request was successful, false otherwise.
 */
void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count_p, uint16_t* block_size_p) {
    // LOG_VERBOSE("tud_msc_capacity_cb called\n");
    if (!tud_msc_test_unit_ready_cb(lun)) {
        *block_count_p = 0;
        *block_size_p = 0;
    } else {
        DRESULT dr = disk_ioctl(lun, GET_SECTOR_COUNT, block_count_p);
        if (RES_OK != dr) *block_count_p = 0;
        *block_size_p = 512;
    }
}

uint8_t tud_msc_get_maxlun_cb(void) {
    // LOG_VERBOSE("tud_msc_get_maxlun_cb called\n");
    return 1; // Only one Logical Unit Number (LUN)
}

/**
 * @brief Handles the INQUIRY request.
 *
 * @param lun The LUN that the request is for.
 * @param vendor_id The vendor ID to return.
 * @param product_id The product ID to return.
 * @param product_rev The product revision to return.
 */
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4]) {
    // LOG_VERBOSE("tud_msc_inquiry_cb called %d\n", lun);
    (void) lun; 

    const char vid[] = "miniMIDI";
    const char pid[] = "SD Card";
    const char rev[] = "1.0";

    memcpy(vendor_id  , vid, strlen(vid));
    memcpy(product_id , pid, strlen(pid));
    memcpy(product_rev, rev, strlen(rev));
}

/**
 * @brief Callback for the Mass Storage Class's START STOP UNIT request.
 *
 * This callback is invoked when the host sends a START STOP UNIT request to the device.
 *
 * @param[in] lun The logical unit number that the request is for.
 * @param[in] power_condition The power condition of the request.
 * @param[in] start true if the request is to start the unit, false if it is to stop it.
 * @param[in] load_eject true if the request is to load or eject the media.
 *
 * @retval true if the request was successful, false otherwise.
 */
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject) {
    // LOG_VERBOSE("tud_msc_start_stop_cb called\n");
    (void)power_condition;
    if (load_eject) {
        if (start) {
            // load disk storage
        } else {
            // unload disk storage
            DRESULT dr = disk_ioctl(lun, CTRL_SYNC, 0);
            if (RES_OK != dr) return false;
        }
    }
    return true;
}

// bool tud_msc_prevent_allow_medium_removal_cb(uint8_t lun, uint8_t prohibit_removal, uint8_t control) {
//     LOG_VERBOSE("tud_msc_prevent_allow_medium_removal_cb called\n");
//     // Allow or prevent medium removal
//     return true;
// }

/**
 * @brief Returns true if the MSC is writable.
 *
 * @param[in] lun The logical unit number that the request is for.
 *
 * @return true if the MSC is writable, false otherwise.
 */
bool tud_msc_is_writable_cb(uint8_t lun) {
    // LOG_VERBOSE("tud_msc_is_writable_cb called\n");
    if (mMApp.sd.Mounted()) return false;

    DSTATUS ds = disk_status(lun);
    return !(STA_PROTECT & ds);
}

/**
 * @brief Callback for handling SCSI commands.
 *
 * @param[in] lun The logical unit number that the request is for.
 * @param[in] scsi_cmd The SCSI command to be handled.
 * @param[in] buffer The buffer containing the data to be written.
 * @param[in] bufsize The size of the buffer in bytes.
 *
 * @return The number of bytes written.
 */
int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize) {
    // LOG_VERBOSE("tud_msc_scsi_cb called\n");
    void const* response = NULL;
    int32_t resplen = 0;

    // most scsi handled is input
    bool in_xfer = true;

    switch (scsi_cmd[0]) {
        default:
            // Set Sense = Invalid Command Operation
            tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);
            // negative means error -> tinyusb could stall and/or response with failed status
            resplen = -1;
            break;
    }

    // return resplen must not larger than bufsize
    if (resplen > bufsize) resplen = bufsize;

    if (response && (resplen > 0)) {
        if (in_xfer) {
            memcpy(buffer, response, (size_t)resplen);
        } else {
            ;  // SCSI output
        }
    }
    return (int32_t)resplen;
}
