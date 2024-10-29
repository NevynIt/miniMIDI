#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

// Common Configuration
#define CFG_TUSB_MCU                OPT_MCU_RP2040
#define CFG_TUSB_RHPORT0_MODE       (OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED)
// #define CFG_TUSB_OS                 OPT_OS_NONE
// #define CFG_TUSB_DEBUG              0

// Device Configuration
#define CFG_TUD_ENDPOINT0_SIZE      64

// Class Configuration
#define CFG_TUD_CDC                 1
#define CFG_TUD_MIDI                4
#define CFG_TUD_AUDIO               1

// CDC FIFO size of TX and RX
#define CFG_TUD_CDC_RX_BUFSIZE      64
#define CFG_TUD_CDC_TX_BUFSIZE      64

// MIDI FIFO size of TX and RX
#define CFG_TUD_MIDI_RX_BUFSIZE     128
#define CFG_TUD_MIDI_TX_BUFSIZE     128

// Audio Configuration
#define CFG_TUD_AUDIO_FUNC_1_DESC_LEN  TUD_AUDIO_MIC_ONE_CH_DESC_LEN + TUD_AUDIO_SPEAKER_MONO_FB_DESC_LEN
#define CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX  1
#define CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX  1
#define CFG_TUD_AUDIO_FUNC_1_EP_IN_SZ_MAX   48
#define CFG_TUD_AUDIO_FUNC_1_EP_OUT_SZ_MAX  48

// Define the number of Standard AS Interface Descriptors
#define CFG_TUD_AUDIO_FUNC_1_N_AS_INT 2

// Define the audio class control request buffer size
#define CFG_TUD_AUDIO_FUNC_1_CTRL_BUF_SZ 64

#ifdef __cplusplus
 }
#endif

#endif /* _TUSB_CONFIG_H_ */