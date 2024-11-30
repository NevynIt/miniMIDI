
//--------------------------------------------------------------------
// START OF AUTOCONFIG_PY_SCRIPT - autogen_usb_descriptors.h
//--------------------------------------------------------------------
#include "cinttypes"
#define NULL __null


//--------------------------------------------------------------------+
// String array
//--------------------------------------------------------------------+
static char const * const string_desc_arr[] =
{
    (const char[]){0x09, 0x04},
    "miniMIDI",
    "miniMIDI MIX",
    NULL,
    "mM_Sound",
    "LineIn",
    "LineOut",
    "mM_Serial",
    "mM_MIDI",
    "mM_MIDI 1",
    "mM_MIDI 2",
    "mM_LEDS",
    "mM_SYNTH",
    "mM_SD",
};


//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+
static const uint8_t desc_fs_configuration[] = {
/* #### Configuration Descriptor */\
/*    9, TUSB_DESC_CONFIGURATION, U16_TO_U8S_LE(_total_len), _itfcount, config_num, _stridx, TU_BIT(7) | _attribute, (_power_ma)/2 */\
0x9, 0x2, 0xc, 0x2, 0x8, 0x1, 0x0, 0x80, 0x32, 
/* #### AUDIO Descriptor: mM_Sound */\
/* {
    "type": "Audio",
    "rate": 48000,
    "features": [
        "Volume",
        "Mute"
    ],
    "audio_func": 10,
    "interfaces": {
        "LineIn": {
            "type": "input",
            "channels": 2,
            "audio_term_type": 513,
            "audio_channels_config": 3,
            "bitResolution": 16,
            "bytesPerSample": 2
        },
        "LineOut": {
            "type": "output",
            "channels": 2,
            "audio_term_type": 770,
            "audio_channels_config": 3,
            "bitResolution": 16,
            "bytesPerSample": 2
        }
    }
} */\
/* ## AUDIO HEADER - IAD */\
/*    8, TUSB_DESC_INTERFACE_ASSOCIATION, itf_audio, n_itf, TUSB_CLASS_AUDIO, AUDIO_FUNCTION_SUBCLASS_UNDEFINED, AUDIO_FUNC_PROTOCOL_CODE_V2, stridx */\
0x8, 0xb, 0x0, 0x3, 0x1, 0x0, 0x20, 0x4, 
/* ## AUDIO CONTROL INTERFACE */\
/*    9, TUSB_DESC_INTERFACE, itf_audio, 0x00, 0x00, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_CONTROL, AUDIO_INT_PROTOCOL_CODE_V2, str_ac */\
0x9, 0x4, 0x0, 0x0, 0x0, 0x1, 0x1, 0x20, 0x0, 
/* ## Class-Specific AC Interface Header Descriptor */\
/*    9, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_HEADER, U16_TO_U8S_LE(0x0200), func['audio_func'], 'audio_ac_hdr_len', AUDIO_CS_AS_INTERFACE_CTRL_LATENCY_POS */\
0x9, 0x24, 0x1, 0x0, 0x2, 0xa, 0x6f, 0x0, 0x0, 
/* ## Clock Source Descriptor */\
/*    8, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_CLOCK_SOURCE, ent_clk, AUDIO_CLOCK_SOURCE, AUDIO_CLOCK_SOURCE_CTRL, assocTerm, str_clk */\
0x8, 0x24, 0xa, 0x1, 0x1, 0x1, 0x0, 0x0, 
/* ### Interface controls LineIn */\
/* ## Input Terminal Descriptor */\
/*    17, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_INPUT_TERMINAL, ent_in, U16_TO_U8S_LE(term_type_in), ent_out, ent_clk, tdata['channels'], U32_TO_U8S_LE(tdata['audio_channels_config']), 0x00, U16_TO_U8S_LE(ctrl), str_tname */\
0x11, 0x24, 0x2, 0x2, 0x1, 0x2, 0x3, 0x1, 0x2, 0x3, 0x0, 0x0, 0x0, 0x0, 0x4, 0x0, 0x5, 
/* ## Feature Unit Descriptor */\
/*    (6+(tdata['channels']+1)*4), TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_FEATURE_UNIT, ent_feature, ent_in, ctrlch*tdata['channels'], str_tname */\
0x12, 0x24, 0x6, 0x4, 0x2, 0xf, 0x0, 0x0, 0x0, 0xf, 0x0, 0x0, 0x0, 0xf, 0x0, 0x0, 0x0, 0x5, 
/* ## Output Terminal Descriptor */\
/*    12, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_OUTPUT_TERMINAL, ent_out, U16_TO_U8S_LE(term_type_out), _assocTerm, srcid, ent_clk, U16_TO_U8S_LE(ctrl), str_tname */\
0xc, 0x24, 0x3, 0x3, 0x1, 0x1, 0x2, 0x4, 0x1, 0x0, 0x0, 0x5, 
/* ### Interface controls LineOut */\
/* ## Input Terminal Descriptor */\
/*    17, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_INPUT_TERMINAL, ent_in, U16_TO_U8S_LE(term_type_in), ent_out, ent_clk, tdata['channels'], U32_TO_U8S_LE(tdata['audio_channels_config']), 0x00, U16_TO_U8S_LE(ctrl), str_tname */\
0x11, 0x24, 0x2, 0x5, 0x1, 0x1, 0x6, 0x1, 0x2, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 
/* ## Feature Unit Descriptor */\
/*    (6+(tdata['channels']+1)*4), TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_FEATURE_UNIT, ent_feature, ent_in, ctrlch*tdata['channels'], str_tname */\
0x12, 0x24, 0x6, 0x7, 0x5, 0xf, 0x0, 0x0, 0x0, 0xf, 0x0, 0x0, 0x0, 0xf, 0x0, 0x0, 0x0, 0x6, 
/* ## Output Terminal Descriptor */\
/*    12, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_OUTPUT_TERMINAL, ent_out, U16_TO_U8S_LE(term_type_out), _assocTerm, srcid, ent_clk, U16_TO_U8S_LE(ctrl), str_tname */\
0xc, 0x24, 0x3, 0x6, 0x2, 0x3, 0x5, 0x7, 0x1, 0x4, 0x0, 0x6, 
/* ### Interface LineIn */\
/* {
    "type": "input",
    "channels": 2,
    "audio_term_type": 513,
    "audio_channels_config": 3,
    "bitResolution": 16,
    "bytesPerSample": 2,
    "ent_base": 2,
    "strnum": 5
} */\
/* ## Alternate 0 - default alternate setting with 0 bandwidth */\
/*    9, TUSB_DESC_INTERFACE, itf, altn, neps, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_STREAMING, AUDIO_INT_PROTOCOL_CODE_V2, str_tname */\
0x9, 0x4, 0x1, 0x0, 0x0, 0x1, 0x2, 0x20, 0x5, 
/* ## Alternate 1 - alternate interface for data streaming */\
/*    9, TUSB_DESC_INTERFACE, itf, altn, neps, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_STREAMING, AUDIO_INT_PROTOCOL_CODE_V2, str_tname */\
0x9, 0x4, 0x1, 0x1, 0x1, 0x1, 0x2, 0x20, 0x5, 
/* ## Class-Specific AS Interface Descriptor */\
/*    16, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AS_INTERFACE_AS_GENERAL, ent, AUDIO_CTRL_NONE, AUDIO_FORMAT_TYPE_I, U32_TO_U8S_LE(AUDIO_DATA_FORMAT_TYPE_I_PCM), tdata['channels'], U32_TO_U8S_LE(tdata['audio_channels_config']), str_tname */\
0x10, 0x24, 0x1, 0x3, 0x0, 0x1, 0x1, 0x0, 0x0, 0x0, 0x2, 0x3, 0x0, 0x0, 0x0, 0x5, 
/* ## Type I Format Type Descriptor */\
/*    6, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AS_INTERFACE_FORMAT_TYPE, AUDIO_FORMAT_TYPE_I, tdata['bytesPerSample'], tdata['bitResolution'] */\
0x6, 0x24, 0x2, 0x1, 0x2, 0x10, 
/* ## Standard AS Isochronous Audio Data Endpoint Descriptor */\
/*    7, TUSB_DESC_ENDPOINT, ep, TUSB_XFER_ISOCHRONOUS | TUSB_ISO_EP_ATT_ADAPTIVE | TUSB_ISO_EP_ATT_DATA, U16_TO_U8S_LE(epsize), 0x01 */\
0x7, 0x5, 0x81, 0x9, 0xc4, 0x0, 0x1, 
/* ## Class-Specific AS Isochronous Audio Data Endpoint Descriptor */\
/*    8, TUSB_DESC_CS_ENDPOINT, AUDIO_CS_EP_SUBTYPE_GENERAL, AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK, AUDIO_CTRL_NONE, AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_MILLISEC, U16_TO_U8S_LE(0x0001) */\
0x8, 0x25, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 
/* ### Interface LineOut */\
/* {
    "type": "output",
    "channels": 2,
    "audio_term_type": 770,
    "audio_channels_config": 3,
    "bitResolution": 16,
    "bytesPerSample": 2,
    "ent_base": 5,
    "strnum": 6
} */\
/* ## Alternate 0 - default alternate setting with 0 bandwidth */\
/*    9, TUSB_DESC_INTERFACE, itf, altn, neps, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_STREAMING, AUDIO_INT_PROTOCOL_CODE_V2, str_tname */\
0x9, 0x4, 0x2, 0x0, 0x0, 0x1, 0x2, 0x20, 0x6, 
/* ## Alternate 1 - alternate interface for data streaming */\
/*    9, TUSB_DESC_INTERFACE, itf, altn, neps, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_STREAMING, AUDIO_INT_PROTOCOL_CODE_V2, str_tname */\
0x9, 0x4, 0x2, 0x1, 0x1, 0x1, 0x2, 0x20, 0x6, 
/* ## Class-Specific AS Interface Descriptor */\
/*    16, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AS_INTERFACE_AS_GENERAL, ent, AUDIO_CTRL_NONE, AUDIO_FORMAT_TYPE_I, U32_TO_U8S_LE(AUDIO_DATA_FORMAT_TYPE_I_PCM), tdata['channels'], U32_TO_U8S_LE(tdata['audio_channels_config']), str_tname */\
0x10, 0x24, 0x1, 0x5, 0x0, 0x1, 0x1, 0x0, 0x0, 0x0, 0x2, 0x3, 0x0, 0x0, 0x0, 0x6, 
/* ## Type I Format Type Descriptor */\
/*    6, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AS_INTERFACE_FORMAT_TYPE, AUDIO_FORMAT_TYPE_I, tdata['bytesPerSample'], tdata['bitResolution'] */\
0x6, 0x24, 0x2, 0x1, 0x2, 0x10, 
/* ## Standard AS Isochronous Audio Data Endpoint Descriptor */\
/*    7, TUSB_DESC_ENDPOINT, ep, TUSB_XFER_ISOCHRONOUS | TUSB_ISO_EP_ATT_ADAPTIVE | TUSB_ISO_EP_ATT_DATA, U16_TO_U8S_LE(epsize), 0x01 */\
0x7, 0x5, 0x2, 0x9, 0xc4, 0x0, 0x1, 
/* ## Class-Specific AS Isochronous Audio Data Endpoint Descriptor */\
/*    8, TUSB_DESC_CS_ENDPOINT, AUDIO_CS_EP_SUBTYPE_GENERAL, AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK, AUDIO_CTRL_NONE, AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_MILLISEC, U16_TO_U8S_LE(0x0001) */\
0x8, 0x25, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 
/* #### CDC Descriptor: mM_Serial */\
/* {
    "type": "CDC"
} */\
/* ## Interface Association */\
/*    8, TUSB_DESC_INTERFACE_ASSOCIATION, itf_cdc, 2, TUSB_CLASS_CDC, CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL, CDC_COMM_PROTOCOL_NONE, stridx */\
0x8, 0xb, 0x3, 0x2, 0x2, 0x2, 0x0, 0x7, 
/* ## CDC Control Interface */\
/*    9, TUSB_DESC_INTERFACE, itf_cdc, 0, 1, TUSB_CLASS_CDC, CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL, CDC_COMM_PROTOCOL_NONE, stridx */\
0x9, 0x4, 0x3, 0x0, 0x1, 0x2, 0x2, 0x0, 0x7, 
/* ## CDC Header */\
/*    5, TUSB_CS_DESC_INTERFACE, CDC_FUNC_DESC_HEADER, U16_TO_U8S_LE(0x0120) */\
0x5, 0x24, 0x0, 0x20, 0x1, 
/* ## CDC Call */\
/*    5, TUSB_CS_DESC_INTERFACE, CDC_FUNC_DESC_CALL_MANAGEMENT, 0x00, itf_data */\
0x5, 0x24, 0x1, 0x0, 0x4, 
/* ## CDC ACM: support line request */\
/*    4, TUSB_CS_DESC_INTERFACE, CDC_FUNC_DESC_ABSTRACT_CONTROL_MANAGEMENT, 0x02 */\
0x4, 0x24, 0x2, 0x2, 
/* ## CDC Union */\
/*    5, TUSB_CS_DESC_INTERFACE, CDC_FUNC_DESC_UNION, itf_cdc, itf_data */\
0x5, 0x24, 0x6, 0x3, 0x4, 
/* ## Endpoint Notification */\
/*    7, TUSB_DESC_ENDPOINT, ep_notif, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(ep_notif_size), 0x10 */\
0x7, 0x5, 0x83, 0x3, 0x8, 0x0, 0x10, 
/* ## CDC Data Interface */\
/*    9, TUSB_CS_DESC_INTERFACE, itf_data, 0, 2, TUSB_CLASS_CDC_DATA, 0x00, 0x00, 0x00 */\
0x9, 0x4, 0x4, 0x0, 0x2, 0xa, 0x0, 0x0, 0x0, 
/* ## Endpoint Out */\
/*    7, TUSB_DESC_ENDPOINT, epout, TUSB_XFER_BULK, U16_TO_U8S_LE(epsize), 0x00 */\
0x7, 0x5, 0x4, 0x2, 0x40, 0x0, 0x0, 
/* ## Endpoint In */\
/*    7, TUSB_DESC_ENDPOINT, epin, TUSB_XFER_BULK, U16_TO_U8S_LE(epsize), 0x00 */\
0x7, 0x5, 0x84, 0x2, 0x40, 0x0, 0x0, 
/* #### MIDI Descriptor: mM_MIDI */\
/* {
    "type": "MIDI",
    "cables": 4,
    "names": [
        "mM_MIDI 1",
        "mM_MIDI 2",
        "mM_LEDS",
        "mM_SYNTH"
    ]
} */\
/* ### INITIAL HEADER */\
/* ## Audio Control (AC) Interface */\
/*    9, TUSB_DESC_INTERFACE, itf_midi, 0, 0, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_CONTROL, AUDIO_FUNC_PROTOCOL_CODE_UNDEF, stridx */\
0x9, 0x4, 0x5, 0x0, 0x0, 0x1, 0x1, 0x0, 0x8, 
/* ## AC Header */\
/*    9, TUSB_DESC_CS_INTERFACE, 0x01, U16_TO_U8S_LE(0x0100), U16_TO_U8S_LE(0x0009), 0x01, itf_streaming */\
0x9, 0x24, 0x1, 0x0, 0x1, 0x9, 0x0, 0x1, 0x6, 
/* ## MIDI Streaming (MS) Interface */\
/*    9, TUSB_DESC_INTERFACE, itf_streaming, 0, 2, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_MIDI_STREAMING, AUDIO_FUNC_PROTOCOL_CODE_UNDEF, 0 */\
0x9, 0x4, 0x6, 0x0, 0x2, 0x1, 0x3, 0x0, 0x0, 
/* ## MS Header */\
/*    7, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_HEADER, U16_TO_U8S_LE(0x0100), U16_TO_U8S_LE(7 + ncables * TUD_MIDI_DESC_JACK_LEN + 2 * TUD_MIDI_DESC_EP_LEN(ncables)) */\
0x7, 0x24, 0x1, 0x0, 0x1, 0xa1, 0x0, 
/* ### MIDI CABLE 0 */\
/* ## MS In Jack (Embedded) */\
/*    6, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_IN_JACK, MIDI_JACK_EMBEDDED, TUD_MIDI_JACKID_IN_EMB(i+1), cablename_str[i] */\
0x6, 0x24, 0x2, 0x1, 0x1, 0x9, 
/* ## MS In Jack (External) */\
/*    6, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_IN_JACK, 0x02, TUD_MIDI_JACKID_IN_EXT(i+1), cablename_str[i] */\
0x6, 0x24, 0x2, 0x2, 0x2, 0x9, 
/* ## MS Out Jack (Embedded), connected to In Jack External */\
/*    9, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_OUT_JACK, MIDI_JACK_EMBEDDED, TUD_MIDI_JACKID_OUT_EMB(i+1), 1, TUD_MIDI_JACKID_IN_EXT(i+1), 1, cablename_str[i] */\
0x9, 0x24, 0x3, 0x1, 0x3, 0x1, 0x2, 0x1, 0x9, 
/* ## MS Out Jack (External), connected to In Jack Embedded */\
/*    9, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_OUT_JACK, 0x02, TUD_MIDI_JACKID_OUT_EXT(i+1), 1, TUD_MIDI_JACKID_IN_EMB(i+1), 1, cablename_str[i] */\
0x9, 0x24, 0x3, 0x2, 0x4, 0x1, 0x1, 0x1, 0x9, 
/* ### MIDI CABLE 1 */\
/* ## MS In Jack (Embedded) */\
/*    6, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_IN_JACK, MIDI_JACK_EMBEDDED, TUD_MIDI_JACKID_IN_EMB(i+1), cablename_str[i] */\
0x6, 0x24, 0x2, 0x1, 0x5, 0xa, 
/* ## MS In Jack (External) */\
/*    6, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_IN_JACK, 0x02, TUD_MIDI_JACKID_IN_EXT(i+1), cablename_str[i] */\
0x6, 0x24, 0x2, 0x2, 0x6, 0xa, 
/* ## MS Out Jack (Embedded), connected to In Jack External */\
/*    9, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_OUT_JACK, MIDI_JACK_EMBEDDED, TUD_MIDI_JACKID_OUT_EMB(i+1), 1, TUD_MIDI_JACKID_IN_EXT(i+1), 1, cablename_str[i] */\
0x9, 0x24, 0x3, 0x1, 0x7, 0x1, 0x6, 0x1, 0xa, 
/* ## MS Out Jack (External), connected to In Jack Embedded */\
/*    9, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_OUT_JACK, 0x02, TUD_MIDI_JACKID_OUT_EXT(i+1), 1, TUD_MIDI_JACKID_IN_EMB(i+1), 1, cablename_str[i] */\
0x9, 0x24, 0x3, 0x2, 0x8, 0x1, 0x5, 0x1, 0xa, 
/* ### MIDI CABLE 2 */\
/* ## MS In Jack (Embedded) */\
/*    6, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_IN_JACK, MIDI_JACK_EMBEDDED, TUD_MIDI_JACKID_IN_EMB(i+1), cablename_str[i] */\
0x6, 0x24, 0x2, 0x1, 0x9, 0xb, 
/* ## MS In Jack (External) */\
/*    6, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_IN_JACK, 0x02, TUD_MIDI_JACKID_IN_EXT(i+1), cablename_str[i] */\
0x6, 0x24, 0x2, 0x2, 0xa, 0xb, 
/* ## MS Out Jack (Embedded), connected to In Jack External */\
/*    9, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_OUT_JACK, MIDI_JACK_EMBEDDED, TUD_MIDI_JACKID_OUT_EMB(i+1), 1, TUD_MIDI_JACKID_IN_EXT(i+1), 1, cablename_str[i] */\
0x9, 0x24, 0x3, 0x1, 0xb, 0x1, 0xa, 0x1, 0xb, 
/* ## MS Out Jack (External), connected to In Jack Embedded */\
/*    9, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_OUT_JACK, 0x02, TUD_MIDI_JACKID_OUT_EXT(i+1), 1, TUD_MIDI_JACKID_IN_EMB(i+1), 1, cablename_str[i] */\
0x9, 0x24, 0x3, 0x2, 0xc, 0x1, 0x9, 0x1, 0xb, 
/* ### MIDI CABLE 3 */\
/* ## MS In Jack (Embedded) */\
/*    6, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_IN_JACK, MIDI_JACK_EMBEDDED, TUD_MIDI_JACKID_IN_EMB(i+1), cablename_str[i] */\
0x6, 0x24, 0x2, 0x1, 0xd, 0xc, 
/* ## MS In Jack (External) */\
/*    6, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_IN_JACK, 0x02, TUD_MIDI_JACKID_IN_EXT(i+1), cablename_str[i] */\
0x6, 0x24, 0x2, 0x2, 0xe, 0xc, 
/* ## MS Out Jack (Embedded), connected to In Jack External */\
/*    9, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_OUT_JACK, MIDI_JACK_EMBEDDED, TUD_MIDI_JACKID_OUT_EMB(i+1), 1, TUD_MIDI_JACKID_IN_EXT(i+1), 1, cablename_str[i] */\
0x9, 0x24, 0x3, 0x1, 0xf, 0x1, 0xe, 0x1, 0xc, 
/* ## MS Out Jack (External), connected to In Jack Embedded */\
/*    9, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_OUT_JACK, 0x02, TUD_MIDI_JACKID_OUT_EXT(i+1), 1, TUD_MIDI_JACKID_IN_EMB(i+1), 1, cablename_str[i] */\
0x9, 0x24, 0x3, 0x2, 0x10, 0x1, 0xd, 0x1, 0xc, 
/* ### MIDIOUT ENDPOINT */\
/*    9, TUSB_DESC_ENDPOINT, epout, TUSB_XFER_BULK, U16_TO_U8S_LE(epsize), 0, 0, 0 */\
0x9, 0x5, 0x5, 0x2, 0x40, 0x0, 0x0, 0x0, 0x0, 
/* ## MS Endpoint (connected to embedded jack) */\
/*    (4 + ncables), TUSB_DESC_CS_ENDPOINT, MIDI_CS_ENDPOINT_GENERAL, ncables, [jackids] */\
0x8, 0x25, 0x1, 0x4, 0x1, 0x5, 0x9, 0xd, 
/* ### MIDIIN ENDPOINT */\
/*    9, TUSB_DESC_ENDPOINT, epin, TUSB_XFER_BULK, U16_TO_U8S_LE(epsize), 0, 0, 0 */\
0x9, 0x5, 0x85, 0x2, 0x40, 0x0, 0x0, 0x0, 0x0, 
/* ## MS Endpoint (connected to embedded jack) */\
/*    (4 + ncables), TUSB_DESC_CS_ENDPOINT, MIDI_CS_ENDPOINT_GENERAL, ncables, [jackids] */\
0x8, 0x25, 0x1, 0x4, 0x3, 0x7, 0xb, 0xf, 
/* ### MSC Descriptor: mM_SD */\
/* {
    "type": "MSC"
} */\
/* ## Interface Descriptor */\
/*    9, TUSB_DESC_INTERFACE, itf_msc, 0, 2, TUSB_CLASS_MSC, MSC_SUBCLASS_SCSI, MSC_PROTOCOL_BOT, stridx */\
0x9, 0x4, 0x7, 0x0, 0x2, 0x8, 0x6, 0x50, 0xd, 
/* ## Endpoint Out Descriptor */\
/*    7, TUSB_DESC_ENDPOINT, epout, TUSB_XFER_BULK, U16_TO_U8S_LE(epsize), 0 */\
0x7, 0x5, 0x6, 0x2, 0x40, 0x0, 0x0, 
/* ## Endpoint In Descriptor */\
/*    7, TUSB_DESC_ENDPOINT, epin, TUSB_XFER_BULK, U16_TO_U8S_LE(epsize), 0 */\
0x7, 0x5, 0x86, 0x2, 0x40, 0x0, 0x0, 
};

//--------------------------------------------------------------------
// END OF AUTOCONFIG_PY_SCRIPT
//--------------------------------------------------------------------
