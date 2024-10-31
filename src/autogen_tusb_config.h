
//--------------------------------------------------------------------
// START OF AUTOCONFIG_PY_SCRIPT - autogen_tusb_config.h
//--------------------------------------------------------------------
//------------- DEFINES -------------//
#define CFG_TUD_AUDIO 1
#define CFG_TUD_AUDIO_ENABLE_EP_IN 1
#define CFG_TUD_AUDIO_ENABLE_EP_OUT 1
#define CFG_TUD_AUDIO_EP_SZ_IN 196
#define CFG_TUD_AUDIO_EP_SZ_OUT 196
#define CFG_TUD_AUDIO_FUNC_1_CTRL_BUF_SZ 64
#define CFG_TUD_AUDIO_FUNC_1_DESC_LEN 238
#define CFG_TUD_AUDIO_FUNC_1_EP_IN_SW_BUF_SZ CFG_TUD_AUDIO_EP_SZ_IN
#define CFG_TUD_AUDIO_FUNC_1_EP_IN_SZ_MAX CFG_TUD_AUDIO_EP_SZ_IN
#define CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ CFG_TUD_AUDIO_EP_SZ_OUT
#define CFG_TUD_AUDIO_FUNC_1_EP_OUT_SZ_MAX CFG_TUD_AUDIO_EP_SZ_OUT
#define CFG_TUD_AUDIO_FUNC_1_MAX_CHAN 2
#define CFG_TUD_AUDIO_FUNC_1_N_AS_INT 2
#define CFG_TUD_AUDIO_FUNC_1_N_INPUTS 1
#define CFG_TUD_AUDIO_FUNC_1_N_OUTPUTS 1
#define CFG_TUD_AUDIO_FUNC_1_SAMPLE_RATE 48000
#define CFG_TUD_CDC 1
#define CFG_TUD_CDC_EP_BUFSIZE CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_CDC_RX_BUFSIZE CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_CDC_TX_BUFSIZE CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_ENDPOINT0_SIZE 64
#define CFG_TUD_MIDI 1
#define CFG_TUD_MIDI_RX_BUFSIZE CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_MIDI_TX_BUFSIZE CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_MSC 0
#define STRING_ARRAY_SIZE 13

/* INTERFACES */
enum {
    ITF_NUM_mM_Sound = 0,
    ITF_NUM_mM_Sound_LineIn = 1,
    ITF_NUM_mM_Sound_LineOut = 2,
    ITF_NUM_mM_Serial = 3,
    ITF_NUM_mM_Serial_data = 4,
    ITF_NUM_mM_MIDI = 5,
    ITF_NUM_mM_MIDI_streaming = 6,
};

/* ENDPOINTS */
enum {
    EP_NUM_reserved = 0,
    EP_NUM_mM_Sound_LineIn_in = 1,
    EP_NUM_mM_Sound_LineOut_out = 2,
    EP_NUM_mM_Serial_notif = 3,
    EP_NUM_mM_Serial_io = 4,
    EP_NUM_mM_MIDI_io = 5,
};

/* AUDIO ENTITIES */
enum {
    ENT_NUM_reserved = 0,
    ENT_NUM_mM_Sound_clock = 1,
    ENT_NUM_mM_Sound_LineIn_input = 2,
    ENT_NUM_mM_Sound_LineIn_output = 3,
    ENT_NUM_mM_Sound_LineIn_feature = 4,
    ENT_NUM_mM_Sound_LineOut_input = 5,
    ENT_NUM_mM_Sound_LineOut_output = 6,
    ENT_NUM_mM_Sound_LineOut_feature = 7,
};


//--------------------------------------------------------------------
// END OF AUTOCONFIG_PY_SCRIPT
//--------------------------------------------------------------------
