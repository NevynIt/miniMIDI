#include "App.h"
#include "mod_USB_MSC.h"
#include "mod_SD.h"
#include "tusb.h"

//see example from https://github.com/udo-munk/RP2040-GEEK-80/blob/main/stdio_msc_usb/msc_usb.c

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

int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) {
    LOG_VERBOSE("tud_msc_read10_cb called %d, %d, %d\n", lba, offset, bufsize);
    if (!mMApp.sd.lock()) {
        return -1; // Indicate error if SD card cannot be locked
    }

    int32_t result = mMApp.sd.read(lba, offset, (uint8_t*)buffer, bufsize);
    mMApp.sd.unlock();
    return result;
}

int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) {
    LOG_VERBOSE("tud_msc_write10_cb called\n");
    if (!mMApp.sd.lock()) {
        return -1; // Indicate error if SD card cannot be locked
    }

    int32_t result = mMApp.sd.write(lba, offset, buffer, bufsize);
    mMApp.sd.unlock();
    return result;
}

bool tud_msc_test_unit_ready_cb(uint8_t lun) {
    LOG_VERBOSE("tud_msc_test_unit_ready_cb called\n");
    if (!mMApp.sd.cardReady() || mMApp.sd.mounted) {
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3A, 0x00); // Medium Not Present
        return false;
    }
    return true;
}

void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count, uint16_t* block_size) {
    LOG_VERBOSE("tud_msc_capacity_cb called\n");
    block_size = 0;
    *block_count = mMApp.sd.getSectorCount();
    if (block_count)
    *block_size = mMApp.sd.getSectorSize();
}

uint8_t tud_msc_get_maxlun_cb(void) {
    LOG_VERBOSE("tud_msc_get_maxlun_cb called\n");
    return 1; // Only one Logical Unit Number (LUN)
}

// Invoked when received SCSI_CMD_INQUIRY
// Application fill vendor id, product id and revision with string up to 8, 16, 4 characters respectively
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4]) {
    LOG_VERBOSE("tud_msc_inquiry_cb called %d\n", lun);
    (void) lun; 

    const char vid[] = "miniMIDI";
    const char pid[] = "SD Card";
    const char rev[] = "1.0";

    memcpy(vendor_id  , vid, strlen(vid));
    memcpy(product_id , pid, strlen(pid));
    memcpy(product_rev, rev, strlen(rev));
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject) {
    LOG_VERBOSE("tud_msc_start_stop_cb called\n");
    if (load_eject) {
        if (start) {
            return mMApp.sd.lock();
        } else {
            return mMApp.sd.unlock();
        }
    }
    return true;
}

bool tud_msc_prevent_allow_medium_removal_cb(uint8_t lun, uint8_t prohibit_removal, uint8_t control) {
    LOG_VERBOSE("tud_msc_prevent_allow_medium_removal_cb called\n");
    // Allow or prevent medium removal
    return true;
}

// int32_t tud_msc_request_sense_cb(uint8_t lun, void* buffer, uint16_t bufsize) {
//     printf("tud_msc_request_sense_cb called\n");
//     uint8_t sense_data[18] = {0};

//     // Fill sense data with appropriate values
//     sense_data[0] = 0x70; // Response code
//     sense_data[2] = SCSI_SENSE_NONE; // Sense key
//     sense_data[12] = 0X00; // Additional Sense Code (ASC)
//     sense_data[13] = 0x00; // Additional Sense Code Qualifier (ASCQ)

//     memcpy(buffer, sense_data, bufsize < sizeof(sense_data) ? bufsize : sizeof(sense_data));
//     return sizeof(sense_data);
// }

bool tud_msc_is_writable_cb(uint8_t lun) {
    LOG_VERBOSE("tud_msc_is_writable_cb called\n");
    return mMApp.sd.isWriteable();
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks (MUST not be handled here)
int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize) {
    printf("tud_msc_scsi_cb called\n");
    void const* response = NULL;
    int32_t resplen = 0;

    // most scsi handled is input
    bool in_xfer = true;

    switch (scsi_cmd[0]) {
        default:
            // Set Sense = Invalid Command Operation
            tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

            // negative means error -> tinyusb could stall and/or response with failed status
            return -1;
    }

    // return resplen must not larger than bufsize
    if (resplen > bufsize) resplen = bufsize;

    if (response && (resplen > 0)) {
        if (in_xfer) {
            memcpy(buffer, response, (size_t) resplen);
        } else {
            // SCSI output
        }
    }

    return resplen;
}
