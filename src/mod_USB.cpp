#include "mod_USB.h"
#include "stdio.h"
#include "App.h"

// TODO: get the slot from DSP when the distance between SOFs is > 1.5ms, otherwise, just increment the slot

extern volatile uint32_t e15_last_sof;

void mod_USB::Init()
{
  tusb_init();
  tud_sof_cb_enable(true);

  mMApp.display.println("USB initialized");
  printf("USB initialized\n");
}

void mod_USB::Tick()
{
  tud_task(); // TinyUSB device task

  static uint32_t next_sync = 0; // target time for next sync, 20 us after the expected SOF
  uint32_t now = time_us_32();

  if (next_sync == 0)
  {
    // First sync
    SOF_sync_us = e15_last_sof % 1000 - 1000;
    next_sync = now - (now % 1000) + SOF_sync_us + 1000 + 20;
  }
  else if (now < next_sync)
  {
    // Not time to sync yet
    return;
  }
  else
  {
    int32_t new_sync = e15_last_sof % 1000 - 1000;
    auto sync_delta = abs((int)new_sync - SOF_sync_us);
    if (sync_delta > 400 && sync_delta < 600)
    {
      // Sync
      // auto slot_old = mMApp.dsp.getSlotRelative(0, mMApp.usb.SOF_sync_us);
      // auto slot_new = mMApp.dsp.getSlotRelative(0, new_sync);
      // printf("sync delta: %d, slot old %d new %d\n", sync_delta, slot_old, slot_new);

      SOF_sync_us = new_sync;
      next_sync = now - (now % 1000) + SOF_sync_us + 1000 + 20;
      count_sync_errors++;
    }
    else
    {
      // Just increment the slot
      next_sync += 1000;
    }
  }
}

void mod_USB::Test()
{
  // INTERVALCHECK(1000)

  // printf("SOF Sync errors %d\n", count_sync_errors);
}

inline uint32_t mod_USB::getLastSOF_us() const
{
  return e15_last_sof;
}

/* A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
 * Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]         HID | MSC | CDC          [LSB]
 */
#define _PID_MAP(itf, n) ((CFG_TUD_##itf) << (n))
#define USB_PID (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
                 _PID_MAP(MIDI, 3) | _PID_MAP(AUDIO, 4) | _PID_MAP(VENDOR, 5))

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device =
    {
        .bLength = sizeof(tusb_desc_device_t),
        .bDescriptorType = TUSB_DESC_DEVICE,
        .bcdUSB = 0x0200,
        .bDeviceClass = TUSB_CLASS_MISC,
        .bDeviceSubClass = MISC_SUBCLASS_COMMON,
        .bDeviceProtocol = MISC_PROTOCOL_IAD,
        .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

        .idVendor = 0xCafe,
        .idProduct = USB_PID,
        .bcdDevice = 0x0100,

        .iManufacturer = 0x01,
        .iProduct = 0x02,
        .iSerialNumber = 0x03,

        .bNumConfigurations = 0x01};

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+
#include "autogen_usb_descriptors.h" // Autogenerated from python script

// // Invoked when device is mounted
void tud_mount_cb(void)
{
  printf("USB Mounted\n");
}

// // Invoked when device is unmounted
void tud_umount_cb(void)
{
  printf("USB Unmounted\n");
}

// // Invoked when usb bus is suspended
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void)remote_wakeup_en;
  printf("USB Suspended\n");
}

// // Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  printf("USB Resumed\n");
}

// Device descriptor callback
uint8_t const *tud_descriptor_device_cb(void)
{
  return (uint8_t const *)&desc_device;
}

// Configuration descriptor callback
uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
  (void)index; // for multiple configurations
  return desc_fs_configuration;
}

static uint16_t _desc_str[32 + 1];

#include "pico/unique_id.h"

size_t board_usb_get_serial(uint16_t *serial_str, size_t max_len)
{
  pico_unique_board_id_t id;
  pico_get_unique_board_id(&id);

  size_t len = 0;
  for (size_t i = 0; i < PICO_UNIQUE_BOARD_ID_SIZE_BYTES; i++)
  {
    if (len + 2 > max_len)
      break;
    serial_str[len++] = "0123456789ABCDEF"[id.id[i] >> 4];
    serial_str[len++] = "0123456789ABCDEF"[id.id[i] & 0x0F];
  }
  return len;
}

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
  (void)langid;
  size_t chr_count;

  switch (index)
  {
  case 0: // Language ID
    memcpy(&_desc_str[1], string_desc_arr[0], 2);
    chr_count = 1;
    break;

  case 3: // Board Serial
    chr_count = board_usb_get_serial(_desc_str + 1, 32);
    break;

  default:
    // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

    if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])))
      return NULL;

    const char *str = string_desc_arr[index];

    // Cap at max char
    chr_count = strlen(str);
    size_t const max_count = sizeof(_desc_str) / sizeof(_desc_str[0]) - 1; // -1 for string type
    if (chr_count > max_count)
      chr_count = max_count;

    // Convert ASCII string into UTF-16
    for (size_t i = 0; i < chr_count; i++)
    {
      _desc_str[1 + i] = str[i];
    }
    break;
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));

  return _desc_str;
}
