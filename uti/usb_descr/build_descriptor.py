from tusb_const import *
import json
import os

conf_descr_data = { #AUDIO MUST BE THE FIRST FUNCTION
    'max_power': 100, # mA
    # 'conf_num': 1, # not implemented
    'strings' : [
        0x0409, # 0: is supported language is English (0x0409)
        "miniMIDI", # 1: Manufacturer
        "miniMIDI MIX", # 2: Product
        None, # 3: Serials will use unique ID if possible
    ],
    'functions': {
        'mM_Sound' : {
            # single rate, bit resolution and features for all terminals
            'type': 'Audio',
            'rate': 48000, #only internal fixed clock supported for now, single rate
            'features': ['Volume', 'Mute'], # default for all terminals
            # 'audio_func': AUDIO_FUNC_MICROPHONE,
            'audio_func': AUDIO_FUNC_PRO_AUDIO,
            'interfaces': {
                # 'InputA' : {
                #     'type': 'input',
                #     'channels': 6,
                #     'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT | AUDIO_CHANNEL_CONFIG_FRONT_CENTER | AUDIO_CHANNEL_CONFIG_LOW_FRQ_EFFECTS | AUDIO_CHANNEL_CONFIG_BACK_LEFT | AUDIO_CHANNEL_CONFIG_BACK_RIGHT,
                #     'audio_term_type': AUDIO_TERM_TYPE_IN_GENERIC_MIC,
                #     'bitResolution': 16, #only 1 resolution, no alternates
                #     'bytesPerSample': 2,
                # },
                # 'OutputA' : {
                #     'type': 'output',
                #     'audio_term_type': AUDIO_TERM_TYPE_OUT_GENERIC_SPEAKER,
                #     'channels': 6,
                #     'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT | AUDIO_CHANNEL_CONFIG_FRONT_CENTER | AUDIO_CHANNEL_CONFIG_LOW_FRQ_EFFECTS | AUDIO_CHANNEL_CONFIG_BACK_LEFT | AUDIO_CHANNEL_CONFIG_BACK_RIGHT,
                #     'bitResolution': 16, #only 1 resolution, no alternates
                #     'bytesPerSample': 2,
                # },
                # 'InputA' : {
                #     'type': 'input',
                #     # 'channels': 4,
                #     # 'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT | AUDIO_CHANNEL_CONFIG_BACK_LEFT | AUDIO_CHANNEL_CONFIG_BACK_RIGHT,
                #     'channels': 2,
                #     'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT,
                #     'audio_term_type': AUDIO_TERM_TYPE_IN_GENERIC_MIC,
                #     'bitResolution': 16, #only 1 resolution, no alternates
                #     'bytesPerSample': 2,
                # },
                # 'OutputA' : {
                #     'type': 'output',
                #     'audio_term_type': AUDIO_TERM_TYPE_OUT_GENERIC_SPEAKER,
                #     # 'channels': 4,
                #     # 'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT | AUDIO_CHANNEL_CONFIG_BACK_LEFT | AUDIO_CHANNEL_CONFIG_BACK_RIGHT,
                #     'channels': 2,
                #     'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT,
                #     'bitResolution': 16, #only 1 resolution, no alternates
                #     'bytesPerSample': 2,
                # },
                # 'InputB' : {
                #     'type': 'input',
                #     # 'channels': 4,
                #     # 'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT | AUDIO_CHANNEL_CONFIG_BACK_LEFT | AUDIO_CHANNEL_CONFIG_BACK_RIGHT,
                #     'channels': 2,
                #     'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT,
                #     'audio_term_type': AUDIO_TERM_TYPE_IN_GENERIC_MIC,
                #     'bitResolution': 16, #only 1 resolution, no alternates
                #     'bytesPerSample': 2,
                # },
                # 'OutputB' : {
                #     'type': 'output',
                #     'audio_term_type': AUDIO_TERM_TYPE_OUT_GENERIC_SPEAKER,
                #     # 'channels': 4,
                #     # 'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT | AUDIO_CHANNEL_CONFIG_BACK_LEFT | AUDIO_CHANNEL_CONFIG_BACK_RIGHT,
                #     'channels': 2,
                #     'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT,
                #     'bitResolution': 16, #only 1 resolution, no alternates
                #     'bytesPerSample': 2,
                # },
                # 'InputC' : {
                #     'type': 'input',
                #     # 'channels': 4,
                #     # 'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT | AUDIO_CHANNEL_CONFIG_BACK_LEFT | AUDIO_CHANNEL_CONFIG_BACK_RIGHT,
                #     'channels': 2,
                #     'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT,
                #     'audio_term_type': AUDIO_TERM_TYPE_IN_UNDEFINED,
                #     'bitResolution': 16, #only 1 resolution, no alternates
                #     'bytesPerSample': 2,
                # },
                # 'OutputC' : {
                #     'type': 'output',
                #     'audio_term_type': AUDIO_TERM_TYPE_OUT_UNDEFINED,
                #     # 'channels': 4,
                #     # 'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT | AUDIO_CHANNEL_CONFIG_BACK_LEFT | AUDIO_CHANNEL_CONFIG_BACK_RIGHT,
                #     'channels': 2,
                #     'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT,
                #     'bitResolution': 16, #only 1 resolution, no alternates
                #     'bytesPerSample': 2,
                # },

                # 'Microphone' : {
                #     'type': 'input',
                #     'channels': 1,
                #     'audio_term_type': AUDIO_TERM_TYPE_IN_GENERIC_MIC,
                #     'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_CENTER,
                #     # 'channels': 4,
                #     # 'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT | AUDIO_CHANNEL_CONFIG_BACK_LEFT | AUDIO_CHANNEL_CONFIG_BACK_RIGHT,
                #     'bitResolution': 16, #only 1 resolution, no alternates
                #     'bytesPerSample': 2,
                # },
                'LineIn' : {
                    'type': 'input',
                    'channels': 2,
                    'audio_term_type': AUDIO_TERM_TYPE_IN_UNDEFINED,
                    'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT,
                    'bitResolution': 16, #only 1 resolution, no alternates
                    'bytesPerSample': 2,
                },
                # 'Synths' : {
                #     'type': 'input',
                #     'channels': 3,
                #     'audio_term_type': AUDIO_TERM_TYPE_IN_UNDEFINED,
                #     'audio_channels_config': AUDIO_CHANNEL_CONFIG_NON_PREDEFINED,
                #     'bitResolution': 16, #only 1 resolution, no alternates
                #     'bytesPerSample': 2,
                # },
                # 'Speakers' : {
                #     'type': 'output',
                #     'channels': 2,
                #     'audio_term_type': AUDIO_TERM_TYPE_OUT_GENERIC_SPEAKER,
                #     'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT,
                #     'bitResolution': 16, #only 1 resolution, no alternates
                #     'bytesPerSample': 2,
                # },
                'LineOut' : {
                    'type': 'output',
                    'channels': 2,
                    'audio_term_type': AUDIO_TERM_TYPE_OUT_UNDEFINED,
                    'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT,
                    'bitResolution': 16, #only 1 resolution, no alternates
                    'bytesPerSample': 2,
                },
                # 'LineOut' : {
                #     'type': 'output',
                #     'channels': 4,
                #     'audio_term_type': AUDIO_TERM_TYPE_OUT_UNDEFINED,
                #     'audio_channels_config': AUDIO_CHANNEL_CONFIG_FRONT_LEFT | AUDIO_CHANNEL_CONFIG_FRONT_RIGHT | AUDIO_CHANNEL_CONFIG_BACK_LEFT | AUDIO_CHANNEL_CONFIG_BACK_RIGHT,
                #     'bitResolution': 16, #only 1 resolution, no alternates
                #     'bytesPerSample': 2,
                # },
            },
        },
        # 'mM_CLI': {
        #     'type': 'CDC',
        # },
        'mM_Serial': {
            'type': 'CDC',
        },
        'mM_MIDI' : {
            'type': 'MIDI',
            'cables': 4,
            'names': [
                "mM_MIDI 1",
                "mM_MIDI 2",
                "mM_MIDI 3",
                "mM_MIDI 4",
            ],	
        },
        # 'mM_MIDI' : {
        #     'type': 'MIDI',
        #     'cables': 6,
        #     'names': [
        #         "mM_MIDI 1",
        #         "mM_MIDI 2",
        #         "mM_MIDI 3",
        #         "mM_MIDI 4",
        #         "mM_MIDI 5",
        #         "mM_MIDI 6",
        #     ],	
        # },
        # 'mM_Data': {
        #     'type': 'MSC',
        # },
    },
}

class dummy:
    def __init__(self):
        pass
    
    def __getitem__(self, key):
        return getattr(self, key)

    def __setitem__(self, key, value):
        setattr(self, key, value)

    def __contains__(self, key):
        return hasattr(self, key)

def replace_params(descr, params):
    # replace tuples and strtings in descr
    final_descr = []
    for x in descr:
        if isinstance(x, tuple):
            final_descr.extend(x)
        elif isinstance(x, str):
            if x in params:
                v = params[x]
                if isinstance(v, tuple) or isinstance(v, list):
                    final_descr.extend(v)
                else:
                    final_descr.append(v)
            else:
                final_descr.append(x)
        elif isinstance(x,type(None)):
            pass
        else:
            final_descr.append(x)
    return final_descr

def add_cmt(params, pos, cmt):
    params.cmts.append( (params.total_len + pos, cmt,) )

def build_conf_descr(data, params):
    pad = None

    # build configuration descriptor
    #define TUD_CONFIG_DESCRIPTOR(config_num, _itfcount, _stridx, _total_len, _attribute, _power_ma) 
    # 9, TUSB_DESC_CONFIGURATION, U16_TO_U8S_LE(_total_len), _itfcount, config_num, _stridx, TU_BIT(7) | _attribute, (_power_ma)/2
    conf_descr = [
        0x09,               # Size : 9 Bytes
        TUSB_DESC_CONFIGURATION,               # Configuration Descriptor (0x02)
        'total_len', '',        # Total length in bytes of data returned   Includes the combined length of all descriptors (configuration, interface, endpoint, and class- or vendor-specific) returned for this configuration.
        'itfnum',     # Number of Interfaces
        0x01,               # bConfigurationValue, Value to use as an argument to select this configuration
        0x00,               # iConfiguration, Index of String Descriptor describing this configuration
        TU_BIT(7),          # bmAttributes, 0b01100000 -> D6: Self-powered, D7: Reserved (set to one)
        data['max_power'] // 2,               # Maximum Power : 250 * 2mA = 0,5A
    ]
    add_cmt(params, 0, "#### Configuration Descriptor")
    add_cmt(params, 0, "   9, TUSB_DESC_CONFIGURATION, U16_TO_U8S_LE(_total_len), _itfcount, config_num, _stridx, TU_BIT(7) | _attribute, (_power_ma)/2")
    assert len(conf_descr) == 9
    return conf_descr

def build_cdc_descr(name, func, params):
    # build CDC descriptor
    itf_cdc = params.itfnum
    params.itf_use.append(name)
    itf_data = params.itfnum + 1
    params.itf_use.append(name + '_data')
    params.itfnum += 2
    params.defines['CFG_TUD_CDC'] += 1
    params.defines['CFG_TUD_CDC_RX_BUFSIZE'] = 'CFG_TUD_ENDPOINT0_SIZE'
    params.defines['CFG_TUD_CDC_TX_BUFSIZE'] = 'CFG_TUD_ENDPOINT0_SIZE'
    params.defines['CFG_TUD_CDC_EP_BUFSIZE'] = 'CFG_TUD_ENDPOINT0_SIZE'

    ep_notif = params.epnum | 0x80
    params.ep_use.append(name+'_notif')
    epout = params.epnum + 1
    params.ep_use.append(name+'_io')
    epin = epout | 0x80
    params.epnum += 2
    params.strings.append(name)
    stridx = params.strnum
    params.strnum += 1

    ep_notif_size = 8

    pad = None

    n = 0
    add_cmt(params, n, f"#### CDC Descriptor: {name}")
    # add a json representation of func
    add_cmt(params, n, json.dumps(func, indent=4))
    add_cmt(params, n, "## Interface Association")
    add_cmt(params, n, "   8, TUSB_DESC_INTERFACE_ASSOCIATION, itf_cdc, 2, TUSB_CLASS_CDC, CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL, CDC_COMM_PROTOCOL_NONE, stridx")
    n += 8
    add_cmt(params, n, "## CDC Control Interface")
    add_cmt(params, n, "   9, TUSB_DESC_INTERFACE, itf_cdc, 0, 1, TUSB_CLASS_CDC, CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL, CDC_COMM_PROTOCOL_NONE, stridx")
    n += 9
    add_cmt(params, n, "## CDC Header")
    add_cmt(params, n, "   5, TUSB_CS_DESC_INTERFACE, CDC_FUNC_DESC_HEADER, U16_TO_U8S_LE(0x0120)")
    n += 5
    add_cmt(params, n, "## CDC Call")
    add_cmt(params, n, "   5, TUSB_CS_DESC_INTERFACE, CDC_FUNC_DESC_CALL_MANAGEMENT, 0x00, itf_data")
    n += 5
    add_cmt(params, n, "## CDC ACM: support line request")
    add_cmt(params, n, "   4, TUSB_CS_DESC_INTERFACE, CDC_FUNC_DESC_ABSTRACT_CONTROL_MANAGEMENT, 0x02")
    n += 4
    add_cmt(params, n, "## CDC Union")
    add_cmt(params, n, "   5, TUSB_CS_DESC_INTERFACE, CDC_FUNC_DESC_UNION, itf_cdc, itf_data")
    n += 5
    add_cmt(params, n, "## Endpoint Notification")
    add_cmt(params, n, "   7, TUSB_DESC_ENDPOINT, ep_notif, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(ep_notif_size), 0x10")
    n += 7
    add_cmt(params, n, "## CDC Data Interface")
    add_cmt(params, n, "   9, TUSB_CS_DESC_INTERFACE, itf_data, 0, 2, TUSB_CLASS_CDC_DATA, 0x00, 0x00, 0x00")
    n += 9
    add_cmt(params, n, "## Endpoint Out")
    add_cmt(params, n, "   7, TUSB_DESC_ENDPOINT, epout, TUSB_XFER_BULK, U16_TO_U8S_LE(epsize), 0x00")
    n += 7
    add_cmt(params, n, "## Endpoint In")
    add_cmt(params, n, "   7, TUSB_DESC_ENDPOINT, epin, TUSB_XFER_BULK, U16_TO_U8S_LE(epsize), 0x00")
    n += 7

    # // CDC Descriptor Template
    # // Interface number, string index, EP notification address and size, EP data address (out, in) and size.
    #define TUD_CDC_DESCRIPTOR(_itfnum, _stridx, _ep_notif, _ep_notif_size, _epout, _epin, _epsize) \
    cdc_descr = [
        # Interface Association
        # 8, TUSB_DESC_INTERFACE_ASSOCIATION, _itfnum, 2, TUSB_CLASS_CDC, CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL, CDC_COMM_PROTOCOL_NONE, 0,\
        8, TUSB_DESC_INTERFACE_ASSOCIATION, itf_cdc, 2, TUSB_CLASS_CDC, CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL, CDC_COMM_PROTOCOL_NONE, stridx,
        # CDC Control Interface
        # 9, TUSB_DESC_INTERFACE, _itfnum, 0, 1, TUSB_CLASS_CDC, CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL, CDC_COMM_PROTOCOL_NONE, _stridx,\
        9, TUSB_DESC_INTERFACE, itf_cdc, 0, 1, TUSB_CLASS_CDC, CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL, CDC_COMM_PROTOCOL_NONE, stridx,
        # CDC Header
        # 5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_HEADER, U16_TO_U8S_LE(0x0120),\
        5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_HEADER, U16_TO_U8S_LE(0x0120),
        # CDC Call
        # 5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_CALL_MANAGEMENT, 0, (uint8_t)((_itfnum) + 1),\
        5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_CALL_MANAGEMENT, 0x00, itf_data,
        # CDC ACM: support line request
        # 4, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_ABSTRACT_CONTROL_MANAGEMENT, 2,\
        4, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_ABSTRACT_CONTROL_MANAGEMENT, 0x02,
        # CDC Union
        # 5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_UNION, _itfnum, (uint8_t)((_itfnum) + 1),\
        5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_UNION, itf_cdc, itf_data,
        # Endpoint Notification
        # 7, TUSB_DESC_ENDPOINT, _ep_notif, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(_ep_notif_size), 16,\
        7, TUSB_DESC_ENDPOINT, ep_notif, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(ep_notif_size), 0x10,
        # CDC Data Interface
        # 9, TUSB_DESC_INTERFACE, (uint8_t)((_itfnum)+1), 0, 2, TUSB_CLASS_CDC_DATA, 0, 0, 0,\
        9, TUSB_DESC_INTERFACE, itf_data, 0, 2, TUSB_CLASS_CDC_DATA, 0x00, 0x00, 0x00,
        # Endpoint Out
        # 7, TUSB_DESC_ENDPOINT, _epout, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0,\
        7, TUSB_DESC_ENDPOINT, epout, TUSB_XFER_BULK, 'epsize', '', 0x00,
        # Endpoint In
        # 7, TUSB_DESC_ENDPOINT, _epin, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0
        7, TUSB_DESC_ENDPOINT, epin, TUSB_XFER_BULK, 'epsize', '', 0x00,
    ]
    cdc_descr = replace_params(cdc_descr, {})
    # assert len(cdc_descr) == TUD_CDC_DESC_LEN
    return cdc_descr

def build_midi_descr(name, func, params):
    # build MIDI descriptor
    params.defines['CFG_TUD_MIDI'] += 1
    params.defines['CFG_TUD_MIDI_RX_BUFSIZE'] = 'CFG_TUD_ENDPOINT0_SIZE'
    params.defines['CFG_TUD_MIDI_TX_BUFSIZE'] = 'CFG_TUD_ENDPOINT0_SIZE'

    itf_midi = params.itfnum
    params.itf_use.append(name)
    itf_streaming = params.itfnum + 1
    params.itf_use.append(name + '_streaming')
    params.itfnum += 2

    epout = params.epnum
    params.ep_use.append(name+'_io')
    epin = epout | 0x80
    params.epnum += 1
    params.strings.append(name)
    stridx = params.strnum
    params.strnum += 1
    ncables = func['cables']
    cablename_str = []
    for n in func['names']:
        params.strings.append(n)
        cablename_str.append(params.strnum)
        params.strnum += 1
    for i in range(ncables - len(func['names'])):
        cablename_str.append(0)

    pad = None

    n = 0
    add_cmt(params, n, f"#### MIDI Descriptor: {name}")
    # add a json representation of func
    add_cmt(params, n, json.dumps(func, indent=4))
    add_cmt(params, n, "### INITIAL HEADER")
    add_cmt(params, n, "## Audio Control (AC) Interface")
    add_cmt(params, n, "   9, TUSB_DESC_INTERFACE, itf_midi, 0, 0, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_CONTROL, AUDIO_FUNC_PROTOCOL_CODE_UNDEF, stridx")
    n += 9
    add_cmt(params, n, "## AC Header")
    add_cmt(params, n, "   9, TUSB_DESC_CS_INTERFACE, 0x01, U16_TO_U8S_LE(0x0100), U16_TO_U8S_LE(0x0009), 0x01, itf_streaming")
    n += 9
    add_cmt(params, n, "## MIDI Streaming (MS) Interface")
    add_cmt(params, n, "   9, TUSB_DESC_INTERFACE, itf_streaming, 0, 2, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_MIDI_STREAMING, AUDIO_FUNC_PROTOCOL_CODE_UNDEF, 0")
    n += 9
    add_cmt(params, n, "## MS Header")
    add_cmt(params, n, "   7, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_HEADER, U16_TO_U8S_LE(0x0100), U16_TO_U8S_LE(7 + ncables * TUD_MIDI_DESC_JACK_LEN + 2 * TUD_MIDI_DESC_EP_LEN(ncables))")
    n += 7

    midi_descr = [
    # INITIAL HEADER
        # #define TUD_MIDI_DESC_HEAD(_itfnum,  _stridx, _numcables) \
        #   /* Audio Control (AC) Interface */\
        #   9, TUSB_DESC_INTERFACE, _itfnum, 0, 0, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_CONTROL, AUDIO_FUNC_PROTOCOL_CODE_UNDEF, _stridx,\
        9, TUSB_DESC_INTERFACE, itf_midi, 0, 0, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_CONTROL, AUDIO_FUNC_PROTOCOL_CODE_UNDEF, stridx,
        #   /* AC Header */\
        #   9, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_HEADER, U16_TO_U8S_LE(0x0100), U16_TO_U8S_LE(0x0009), 1, (uint8_t)((_itfnum) + 1),\
        9, TUSB_DESC_CS_INTERFACE, 0x01, U16_TO_U8S_LE(0x0100), U16_TO_U8S_LE(0x0009), 0x01, itf_streaming,

        #   /* MIDI Streaming (MS) Interface */\
        #   9, TUSB_DESC_INTERFACE, (uint8_t)((_itfnum) + 1), 0, 2, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_MIDI_STREAMING, AUDIO_FUNC_PROTOCOL_CODE_UNDEF, 0,\
        9, TUSB_DESC_INTERFACE, itf_streaming, 0, 2, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_MIDI_STREAMING, AUDIO_FUNC_PROTOCOL_CODE_UNDEF, 0,
        #   /* MS Header */\
        #   7, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_HEADER, U16_TO_U8S_LE(0x0100), U16_TO_U8S_LE(7 + (_numcables) * TUD_MIDI_DESC_JACK_LEN + 2 * TUD_MIDI_DESC_EP_LEN(_numcables))
        7, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_HEADER, U16_TO_U8S_LE(0x0100), U16_TO_U8S_LE(7 + ncables * TUD_MIDI_DESC_JACK_LEN + 2 * TUD_MIDI_DESC_EP_LEN(ncables)),
    ]

    for i in range(ncables):
        add_cmt(params, n, f"### MIDI CABLE {i}")

        add_cmt(params, n, "## MS In Jack (Embedded)")
        add_cmt(params, n, "   6, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_IN_JACK, MIDI_JACK_EMBEDDED, TUD_MIDI_JACKID_IN_EMB(i+1), cablename_str[i]")
        n += 6
        add_cmt(params, n, "## MS In Jack (External)")
        add_cmt(params, n, "   6, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_IN_JACK, 0x02, TUD_MIDI_JACKID_IN_EXT(i+1), cablename_str[i]")
        n += 6
        add_cmt(params, n, "## MS Out Jack (Embedded), connected to In Jack External")
        add_cmt(params, n, "   9, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_OUT_JACK, MIDI_JACK_EMBEDDED, TUD_MIDI_JACKID_OUT_EMB(i+1), 1, TUD_MIDI_JACKID_IN_EXT(i+1), 1, cablename_str[i]")
        n += 9
        add_cmt(params, n, "## MS Out Jack (External), connected to In Jack Embedded")
        add_cmt(params, n, "   9, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_OUT_JACK, 0x02, TUD_MIDI_JACKID_OUT_EXT(i+1), 1, TUD_MIDI_JACKID_IN_EMB(i+1), 1, cablename_str[i]")
        n += 9

        midi_descr.extend([
        # FOR EACH CABLE
            # #define TUD_MIDI_DESC_JACK_DESC(_cablenum, _stridx) \
            #   /* MS In Jack (Embedded) */\
            #   6, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_IN_JACK, MIDI_JACK_EMBEDDED, TUD_MIDI_JACKID_IN_EMB(_cablenum), _stridx,\
            6, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_IN_JACK, MIDI_JACK_EMBEDDED, TUD_MIDI_JACKID_IN_EMB(i+1), cablename_str[i],
            #   /* MS In Jack (External) */\
            #   6, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_IN_JACK, MIDI_JACK_EXTERNAL, TUD_MIDI_JACKID_IN_EXT(_cablenum), _stridx,\
            6, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_IN_JACK, 0x02, TUD_MIDI_JACKID_IN_EXT(i+1), cablename_str[i],
            #   /* MS Out Jack (Embedded), connected to In Jack External */\
            #   9, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_OUT_JACK, MIDI_JACK_EMBEDDED, TUD_MIDI_JACKID_OUT_EMB(_cablenum), 1, TUD_MIDI_JACKID_IN_EXT(_cablenum), 1, _stridx,\
            9, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_OUT_JACK, MIDI_JACK_EMBEDDED, TUD_MIDI_JACKID_OUT_EMB(i+1), 1, TUD_MIDI_JACKID_IN_EXT(i+1), 1, cablename_str[i],
            #   /* MS Out Jack (External), connected to In Jack Embedded */\
            #   9, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_OUT_JACK, MIDI_JACK_EXTERNAL, TUD_MIDI_JACKID_OUT_EXT(_cablenum), 1, TUD_MIDI_JACKID_IN_EMB(_cablenum), 1, _stridx
            9, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_OUT_JACK, 0x02, TUD_MIDI_JACKID_OUT_EXT(i+1), 1, TUD_MIDI_JACKID_IN_EMB(i+1), 1, cablename_str[i],
        ])

    add_cmt(params, n, "### MIDIOUT ENDPOINT")
    add_cmt(params, n, "   9, TUSB_DESC_ENDPOINT, epout, TUSB_XFER_BULK, U16_TO_U8S_LE(epsize), 0, 0, 0")
    n += 9
    add_cmt(params, n, "## MS Endpoint (connected to embedded jack)")
    add_cmt(params, n, "   (4 + ncables), TUSB_DESC_CS_ENDPOINT, MIDI_CS_ENDPOINT_GENERAL, ncables, [jackids]")
    n += (4 + ncables)

    midi_descr.extend([
    # MIDIOUT ENDPOINT
        # #define TUD_MIDI_DESC_EP(_epout, _epsize, _numcables) \
        #   /* Endpoint: Note Audio v1.0's endpoint has 9 bytes instead of 7 */\
        #   9, TUSB_DESC_ENDPOINT, _epout, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0, 0, 0, \
        9, TUSB_DESC_ENDPOINT, epout, TUSB_XFER_BULK, 'epsize', '', 0, 0, 0,
        #   /* MS Endpoint (connected to embedded jack) */\
        #   (uint8_t)(4 + (_numcables)), TUSB_DESC_CS_ENDPOINT, MIDI_CS_ENDPOINT_GENERAL, _numcables
        (4 + ncables), TUSB_DESC_CS_ENDPOINT, MIDI_CS_ENDPOINT_GENERAL, ncables
    ])
    for i in range(ncables):
        midi_descr.extend([
        # FOR EACH CABLE
            TUD_MIDI_JACKID_IN_EMB(i+1),
        ])

    add_cmt(params, n, "### MIDIIN ENDPOINT")
    add_cmt(params, n, "   9, TUSB_DESC_ENDPOINT, epin, TUSB_XFER_BULK, U16_TO_U8S_LE(epsize), 0, 0, 0")
    n += 9
    add_cmt(params, n, "## MS Endpoint (connected to embedded jack)")
    add_cmt(params, n, "   (4 + ncables), TUSB_DESC_CS_ENDPOINT, MIDI_CS_ENDPOINT_GENERAL, ncables, [jackids]")
    n += (4 + ncables)

    midi_descr.extend([
        # MIDIIN ENDPOINT
            # #define TUD_MIDI_DESC_EP(_epin, _epsize, _numcables) \
            #   /* Endpoint: Note Audio v1.0's endpoint has 9 bytes instead of 7 */\
            #   9, TUSB_DESC_ENDPOINT, _epin, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0, 0, 0, \
            9, TUSB_DESC_ENDPOINT, epin, TUSB_XFER_BULK, 'epsize', '', 0, 0, 0,
            #   /* MS Endpoint (connected to embedded jack) */\
            #   (uint8_t)(4 + (_numcables)), TUSB_DESC_CS_ENDPOINT, MIDI_CS_ENDPOINT_GENERAL, _numcables
            (4 + ncables), TUSB_DESC_CS_ENDPOINT, MIDI_CS_ENDPOINT_GENERAL, ncables
    ])
    for i in range(ncables):
        midi_descr.extend([
        # FOR EACH CABLE
            TUD_MIDI_JACKID_OUT_EMB(i+1),
        ])

    midi_descr = replace_params(midi_descr, {})
    # assert len(midi_descr) == 9 * 3 + 7 + ncables * TUD_MIDI_DESC_JACK_LEN + 2 * TUD_MIDI_DESC_EP_LEN(ncables)
    return midi_descr

def build_audio_descr(name, func, params):
    # build audio descriptor
    interfaces = sorted(func['interfaces'].items(), key=lambda x: (x[1]['type'], x[0]))
    params.defines['CFG_TUD_AUDIO'] += 1

    itf_audio = params.itfnum
    params.itf_use.append(name)
    params.itfnum += 1

    params.strings.append(name)
    stridx = params.strnum
    params.strnum += 1

    term_in = len([itf for itf in func['interfaces'].values() if itf['type'] == 'input'])
    term_out = len([itf for itf in func['interfaces'].values() if itf['type'] == 'output'])
    n_itf = term_in + term_out
    params.defines['CFG_TUD_AUDIO_FUNC_1_N_INPUTS'] = term_in
    params.defines['CFG_TUD_AUDIO_FUNC_1_N_OUTPUTS'] = term_out
    params.defines['CFG_TUD_AUDIO_FUNC_1_MAX_CHAN'] = max([itf['channels'] for itf in func['interfaces'].values()])

    ctrlch = 0
    if 'Volume' in func['features']:
        ctrlch |= AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS
    if 'Mute' in func['features']:
        ctrlch |= AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS
    #TODO: add support for other features
    ctrlch = U32_TO_U8S_LE(ctrlch)
    max_TUD_AUDIO_EP_SIZE = 0

    ent_clk = params.entnum
    params.ent_use.append(name + '_clock')
    params.entnum += 1

    str_ac = 0
    str_clk = 0

    pad = None

    loc_params = dummy()
    loc_params.audio_ac_hdr_len = 0

    n = 0
    add_cmt(params, n, f"#### AUDIO Descriptor: {name}")
    # add a json representation of func
    add_cmt(params, n, json.dumps(func, indent=4))

    add_cmt(params, n, "## AUDIO HEADER - IAD")
    add_cmt(params, n, "   8, TUSB_DESC_INTERFACE_ASSOCIATION, itf_audio, n_itf, TUSB_CLASS_AUDIO, AUDIO_FUNCTION_SUBCLASS_UNDEFINED, AUDIO_FUNC_PROTOCOL_CODE_V2, stridx")
    n += 8
    add_cmt(params, n, "## AUDIO CONTROL INTERFACE")
    add_cmt(params, n, "   9, TUSB_DESC_INTERFACE, itf_audio, 0x00, 0x00, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_CONTROL, AUDIO_INT_PROTOCOL_CODE_V2, str_ac")
    n += 9
    add_cmt(params, n, "## Class-Specific AC Interface Header Descriptor")
    add_cmt(params, n, "   9, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_HEADER, U16_TO_U8S_LE(0x0200), func['audio_func'], 'audio_ac_hdr_len', AUDIO_CS_AS_INTERFACE_CTRL_LATENCY_POS")
    n += 9

    audio_descr = [
    # AUDIO HEADER - IAD
        #   TUD_AUDIO_DESC_IAD_LEN, TUSB_DESC_INTERFACE_ASSOCIATION, _firstitf, _nitfs, TUSB_CLASS_AUDIO, AUDIO_FUNCTION_SUBCLASS_UNDEFINED, AUDIO_FUNC_PROTOCOL_CODE_V2, _stridx
        8, TUSB_DESC_INTERFACE_ASSOCIATION, itf_audio, n_itf+1, TUSB_CLASS_AUDIO, AUDIO_FUNCTION_SUBCLASS_UNDEFINED, AUDIO_FUNC_PROTOCOL_CODE_V2, stridx,

    # AUDIO CONTROL INTERFACE
        # /* Standard AC Interface Descriptor(4.7.1) */\
        #   TUD_AUDIO_DESC_STD_AC_LEN, TUSB_DESC_INTERFACE, _itfnum, /* fixed to zero */ 0x00, _nEPs, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_CONTROL, AUDIO_INT_PROTOCOL_CODE_V2, _stridx
        9, TUSB_DESC_INTERFACE, itf_audio, 0x00, 0x00, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_CONTROL, AUDIO_INT_PROTOCOL_CODE_V2, str_ac,
        
        # /* Class-Specific AC Interface Header Descriptor(4.7.2) */\
        #   TUD_AUDIO_DESC_CS_AC_LEN, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_HEADER, U16_TO_U8S_LE(_bcdADC), _category, U16_TO_U8S_LE(_totallen + TUD_AUDIO_DESC_CS_AC_LEN), _ctrl
        9, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_HEADER, U16_TO_U8S_LE(0x0200), func['audio_func'], 'audio_ac_hdr_len', AUDIO_CS_AS_INTERFACE_CTRL_LATENCY_POS,
    ]
    loc_params.audio_ac_hdr_len += TUD_AUDIO_DESC_CS_AC_LEN

    add_cmt(params, n, "## Clock Source Descriptor")
    add_cmt(params, n, "   8, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_CLOCK_SOURCE, ent_clk, AUDIO_CLOCK_SOURCE, AUDIO_CLOCK_SOURCE_CTRL, assocTerm, str_clk")
    n += 8

    audio_descr.extend([
        # /* Clock Source Descriptor(4.7.2.1) */\
        # Note: internal clock, fixed
        #   TUD_AUDIO_DESC_CLK_SRC_LEN, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_CLOCK_SOURCE, _clkid, _attr, _ctrl, _assocTerm, _stridx
        8, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_CLOCK_SOURCE, ent_clk, AUDIO_CLOCK_SOURCE_ATT_INT_FIX_CLK, (AUDIO_CTRL_R << AUDIO_CLOCK_SOURCE_CTRL_CLK_FRQ_POS), 0x00, str_clk,
    ])
    loc_params.audio_ac_hdr_len += TUD_AUDIO_DESC_CLK_SRC_LEN

    for tname, tdata in interfaces:
        tdata['ent_base'] = params.entnum
        ent_in = params.entnum
        params.ent_use.append(name + '_' + tname + '_input')
        params.entnum += 1
        ent_out = params.entnum
        params.ent_use.append(name + '_' + tname + '_output')
        params.entnum += 1
        ent_feature = params.entnum
        params.ent_use.append(name + '_' + tname + '_feature')
        params.entnum += 1            
        params.strings.append(tname)
        str_tname = params.strnum
        tdata['strnum'] = params.strnum
        params.strnum += 1

        if tdata['type'] == 'input':
            term_type_in = tdata['audio_term_type']
            term_type_out = AUDIO_TERM_TYPE_USB_STREAMING
            ctrl_in = U16_TO_U8S_LE(AUDIO_CTRL_R << AUDIO_IN_TERM_CTRL_CONNECTOR_POS)
            ctrl_out = U16_TO_U8S_LE(0x00)
        if tdata['type'] == 'output':
            term_type_in = AUDIO_TERM_TYPE_USB_STREAMING
            term_type_out = tdata['audio_term_type']
            ctrl_in = U16_TO_U8S_LE(0x00)
            ctrl_out = U16_TO_U8S_LE(AUDIO_CTRL_R << AUDIO_IN_TERM_CTRL_CONNECTOR_POS) #just the same as input
    
        add_cmt(params, n, f"### Interface controls {tname}")
        add_cmt(params, n, "## Input Terminal Descriptor")
        add_cmt(params, n, "   17, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_INPUT_TERMINAL, ent_in, U16_TO_U8S_LE(term_type_in), ent_out, ent_clk, tdata['channels'], U32_TO_U8S_LE(tdata['audio_channels_config']), 0x00, U16_TO_U8S_LE(ctrl), str_tname")
        n += 17

        audio_descr.extend([
            # /* Input Terminal Descriptor(4.7.2.4) */\
            #     TUD_AUDIO_DESC_INPUT_TERM(/*_termid*/ UAC2_ENTITY_MIC_INPUT_TERMINAL, /*_termtype*/ AUDIO_TERM_TYPE_IN_GENERIC_MIC, /*_assocTerm*/ 0x00, /*_clkid*/ UAC2_ENTITY_CLOCK, /*_nchannelslogical*/ 0x01, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_idxchannelnames*/ 0x00, /*_ctrl*/ 0 * (AUDIO_CTRL_R << AUDIO_IN_TERM_CTRL_CONNECTOR_POS), /*_stridx*/ 0x00),\
                # /* Input Terminal Descriptor(4.7.2.4) */
                # #define TUD_AUDIO_DESC_INPUT_TERM_LEN 17
                # #define TUD_AUDIO_DESC_INPUT_TERM(_termid, _termtype, _assocTerm, _clkid, _nchannelslogical, _channelcfg, _idxchannelnames, _ctrl, _stridx) \
                #   TUD_AUDIO_DESC_INPUT_TERM_LEN, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_INPUT_TERMINAL, _termid, U16_TO_U8S_LE(_termtype), _assocTerm, _clkid, _nchannelslogical, U32_TO_U8S_LE(_channelcfg), _idxchannelnames, U16_TO_U8S_LE(_ctrl), _stridx
            17, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_INPUT_TERMINAL, ent_in, U16_TO_U8S_LE(term_type_in), ent_out, ent_clk, tdata['channels'], U32_TO_U8S_LE(tdata['audio_channels_config']), 0x00, ctrl_in, str_tname,
        ])
        loc_params.audio_ac_hdr_len += 17

        add_cmt(params, n, "## Feature Unit Descriptor")
        add_cmt(params, n, "   (6+(tdata['channels']+1)*4), TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_FEATURE_UNIT, ent_feature, ent_in, ctrlch*tdata['channels'], str_tname")
        n += (6+(tdata['channels']+1)*4)

        audio_descr.extend([
            # /* Feature Unit Descriptor(4.7.2.8) */\
                # TUD_AUDIO_DESC_FEATURE_UNIT_LEN, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_FEATURE_UNIT, _unitid, _srcid, U32_TO_U8S_LE(_ctrlch0master), U32_TO_U8S_LE(_ctrlch1), [U32_TO_U8S_LE(_ctrlch2), U32_TO_U8S_LE(_ctrlch3), U32_TO_U8S_LE(_ctrlch4), ...], _stridx
                6+(tdata['channels']+1)*4, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_FEATURE_UNIT, ent_feature, ent_in, 
                ctrlch*(tdata['channels']+1),
                str_tname,
        ])
        loc_params.audio_ac_hdr_len += 6+(tdata['channels']+1)*4

        add_cmt(params, n, "## Output Terminal Descriptor")
        add_cmt(params, n, "   12, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_OUTPUT_TERMINAL, ent_out, U16_TO_U8S_LE(term_type_out), _assocTerm, srcid, ent_clk, U16_TO_U8S_LE(ctrl), str_tname")
        n += 12

        audio_descr.extend([
            # /* Output Terminal Descriptor(4.7.2.5) */\
            # TUD_AUDIO_DESC_OUTPUT_TERM(/*_termid*/ UAC2_ENTITY_MIC_OUTPUT_TERMINAL, /*_termtype*/ AUDIO_TERM_TYPE_USB_STREAMING, /*_assocTerm*/ 0x00, /*_srcid*/ UAC2_ENTITY_MIC_INPUT_TERMINAL, /*_clkid*/ UAC2_ENTITY_CLOCK, /*_ctrl*/ 0x0000, /*_stridx*/ 0x00),\
                # /* Output Terminal Descriptor(4.7.2.5) */
                # #define TUD_AUDIO_DESC_OUTPUT_TERM_LEN 12
                # #define TUD_AUDIO_DESC_OUTPUT_TERM(_termid, _termtype, _assocTerm, _srcid, _clkid, _ctrl, _stridx) \
                #   TUD_AUDIO_DESC_OUTPUT_TERM_LEN, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_OUTPUT_TERMINAL, _termid, U16_TO_U8S_LE(_termtype), _assocTerm, _srcid, _clkid, U16_TO_U8S_LE(_ctrl), _stridx
            12, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AC_INTERFACE_OUTPUT_TERMINAL, ent_out, U16_TO_U8S_LE(term_type_out), ent_in, ent_feature, ent_clk, ctrl_out, str_tname,
        ])
        loc_params.audio_ac_hdr_len += 12
    
    for tname, tdata in interfaces:
        itf = params.itfnum
        params.itf_use.append(name + '_' + tname)
        params.itfnum += 1
        str_tname = tdata['strnum']

        if tdata['type'] == 'output':
            ent = tdata['ent_base']
            ep = params.epnum
            params.ep_use.append(name + '_' + tname +'_out')
            params.epnum += 1

        if tdata['type'] == 'input':
            ent = tdata['ent_base']+1
            ep = 0x80 | params.epnum
            params.ep_use.append(name + '_' + tname +'_in')
            params.epnum += 1

        maxEPsize = TUD_AUDIO_EP_SIZE(func['rate'], tdata['bytesPerSample'], tdata['channels'])
        max_TUD_AUDIO_EP_SIZE = max(max_TUD_AUDIO_EP_SIZE, maxEPsize)

        add_cmt(params, n, f"### Interface {tname}")
        add_cmt(params, n, json.dumps(tdata, indent=4))
        add_cmt(params, n, "## Alternate 0 - default alternate setting with 0 bandwidth")
        add_cmt(params, n, "   9, TUSB_DESC_INTERFACE, itf, altn, neps, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_STREAMING, AUDIO_INT_PROTOCOL_CODE_V2, str_tname")
        n += 9
        add_cmt(params, n, "## Alternate 1 - alternate interface for data streaming")
        add_cmt(params, n, "   9, TUSB_DESC_INTERFACE, itf, altn, neps, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_STREAMING, AUDIO_INT_PROTOCOL_CODE_V2, str_tname")
        n += 9
        add_cmt(params, n, "## Class-Specific AS Interface Descriptor")
        add_cmt(params, n, "   16, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AS_INTERFACE_AS_GENERAL, ent, AUDIO_CTRL_NONE, AUDIO_FORMAT_TYPE_I, U32_TO_U8S_LE(AUDIO_DATA_FORMAT_TYPE_I_PCM), tdata['channels'], U32_TO_U8S_LE(tdata['audio_channels_config']), str_tname")
        n += 16
        add_cmt(params, n, "## Type I Format Type Descriptor")
        add_cmt(params, n, "   6, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AS_INTERFACE_FORMAT_TYPE, AUDIO_FORMAT_TYPE_I, tdata['bytesPerSample'], tdata['bitResolution']")
        n += 6
        add_cmt(params, n, "## Standard AS Isochronous Audio Data Endpoint Descriptor")
        add_cmt(params, n, "   7, TUSB_DESC_ENDPOINT, ep, TUSB_XFER_ISOCHRONOUS | TUSB_ISO_EP_ATT_ADAPTIVE | TUSB_ISO_EP_ATT_DATA, U16_TO_U8S_LE(epsize), 0x01")
        n += 7
        add_cmt(params, n, "## Class-Specific AS Isochronous Audio Data Endpoint Descriptor")
        add_cmt(params, n, "   8, TUSB_DESC_CS_ENDPOINT, AUDIO_CS_EP_SUBTYPE_GENERAL, AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK, AUDIO_CTRL_NONE, AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_MILLISEC, U16_TO_U8S_LE(0x0001)")
        n += 8

        audio_descr.extend([
        #     /* Interface 1, Alternate 0 - default alternate setting with 0 bandwidth */\
        #     /* Standard AS Interface Descriptor(4.9.1) */\
            #   TUD_AUDIO_DESC_STD_AS_INT_LEN, TUSB_DESC_INTERFACE, _itfnum, _altset, _nEPs, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_STREAMING, AUDIO_INT_PROTOCOL_CODE_V2, _stridx
            9, TUSB_DESC_INTERFACE, itf, 0x00, 0x00, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_STREAMING, AUDIO_INT_PROTOCOL_CODE_V2, str_tname,
    
        # /* Interface 1, Alternate 1 - alternate interface for data streaming */\
        #     /* Standard AS Interface Descriptor(4.9.1) */\
            #   TUD_AUDIO_DESC_STD_AS_INT_LEN, TUSB_DESC_INTERFACE, _itfnum, _altset, _nEPs, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_STREAMING, AUDIO_INT_PROTOCOL_CODE_V2, _stridx
            9, TUSB_DESC_INTERFACE, itf, 0x01, 0x01, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_STREAMING, AUDIO_INT_PROTOCOL_CODE_V2, str_tname,
        #     /* Class-Specific AS Interface Descriptor(4.9.2) */\
            #   TUD_AUDIO_DESC_CS_AS_INT_LEN, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AS_INTERFACE_AS_GENERAL, _termid, _ctrl, _formattype, U32_TO_U8S_LE(_formats), _nchannelsphysical, U32_TO_U8S_LE(_channelcfg), _stridx
            16, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AS_INTERFACE_AS_GENERAL, ent, AUDIO_CTRL_NONE, AUDIO_FORMAT_TYPE_I, U32_TO_U8S_LE(AUDIO_DATA_FORMAT_TYPE_I_PCM), tdata['channels'], U32_TO_U8S_LE(tdata['audio_channels_config']), str_tname,
        #     /* Type I Format Type Descriptor(2.3.1.6 - Audio Formats) */\
            #   TUD_AUDIO_DESC_TYPE_I_FORMAT_LEN, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AS_INTERFACE_FORMAT_TYPE, AUDIO_FORMAT_TYPE_I, _subslotsize, _bitresolution
            6, TUSB_DESC_CS_INTERFACE, AUDIO_CS_AS_INTERFACE_FORMAT_TYPE, AUDIO_FORMAT_TYPE_I, tdata['bytesPerSample'], tdata['bitResolution'],
        #     /* Standard AS Isochronous Audio Data Endpoint Descriptor(4.10.1.1) */\
            #   TUD_AUDIO_DESC_STD_AS_ISO_EP_LEN, TUSB_DESC_ENDPOINT, _ep, _attr, U16_TO_U8S_LE(_maxEPsize), _interval
            7, TUSB_DESC_ENDPOINT, ep, TUSB_XFER_ISOCHRONOUS | TUSB_ISO_EP_ATT_ADAPTIVE | TUSB_ISO_EP_ATT_DATA, U16_TO_U8S_LE(maxEPsize), 0x01,
        #     /* Class-Specific AS Isochronous Audio Data Endpoint Descriptor(4.10.1.2) */\
            #   TUD_AUDIO_DESC_CS_AS_ISO_EP_LEN, TUSB_DESC_CS_ENDPOINT, AUDIO_CS_EP_SUBTYPE_GENERAL, _attr, _ctrl, _lockdelayunit, U16_TO_U8S_LE(_lockdelay)
            8, TUSB_DESC_CS_ENDPOINT, AUDIO_CS_EP_SUBTYPE_GENERAL, AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK, AUDIO_CTRL_NONE, AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_UNDEFINED, U16_TO_U8S_LE(0x0000),
        
        # MORE ALTERNATES WOULD GO HERE
        ])
        
    loc_params.audio_ac_hdr_len = U16_TO_U8S_LE(loc_params.audio_ac_hdr_len)
    audio_descr = replace_params(audio_descr, loc_params)
    # assert len(audio_descr) == 9 + 8 + 17 * term_in + 12 * term_in + 17 * term_out + 12 * term_out + 9 * n_itf
    params.defines[f'CFG_TUD_AUDIO_FUNC_{params.defines['CFG_TUD_AUDIO']}_DESC_LEN'] = len(audio_descr)
    params.defines[f'CFG_TUD_AUDIO_FUNC_{params.defines['CFG_TUD_AUDIO']}_N_AS_INT'] = n_itf
    params.defines[f'CFG_TUD_AUDIO_FUNC_{params.defines['CFG_TUD_AUDIO']}_N_AS_INT'] = n_itf
    params.defines[f'CFG_TUD_AUDIO_FUNC_{params.defines['CFG_TUD_AUDIO']}_CTRL_BUF_SZ'] = 64
    params.defines[f'CFG_TUD_AUDIO_FUNC_{params.defines['CFG_TUD_AUDIO']}_SAMPLE_RATE'] = func['rate']


    if term_in > 0:
        params.defines['CFG_TUD_AUDIO_ENABLE_EP_IN'] = 1
        params.defines['CFG_TUD_AUDIO_EP_SZ_IN'] = max_TUD_AUDIO_EP_SIZE
        # params.defines['CFG_TUD_AUDIO_ENABLE_ENCODING'] = 0
        # params.defines['CFG_TUD_AUDIO_EP_IN_FLOW_CONTROL'] = 0
        params.defines['CFG_TUD_AUDIO_FUNC_1_EP_IN_SZ_MAX'] = 'CFG_TUD_AUDIO_EP_SZ_IN'
        params.defines['CFG_TUD_AUDIO_FUNC_1_EP_IN_SW_BUF_SZ'] = 'CFG_TUD_AUDIO_EP_SZ_IN'
        # params.defines['CFG_TUD_AUDIO_ENABLE_TYPE_I_ENCODING'] = 0
        # params.defines['CFG_TUD_AUDIO_FUNC_1_CHANNEL_PER_FIFO_TX'] = 2
        # params.defines['CFG_TUD_AUDIO_FUNC_1_N_TX_SUPP_SW_FIFO'] = 1

    if term_out > 0:
        params.defines['CFG_TUD_AUDIO_ENABLE_EP_OUT'] = 1
        params.defines['CFG_TUD_AUDIO_EP_SZ_OUT'] = max_TUD_AUDIO_EP_SIZE
        # params.defines['CFG_TUD_AUDIO_ENABLE_DECODING'] = 0
        # params.defines['CFG_TUD_AUDIO_EP_OUT_FLOW_CONTROL'] = 0
        params.defines['CFG_TUD_AUDIO_FUNC_1_EP_OUT_SZ_MAX'] = 'CFG_TUD_AUDIO_EP_SZ_OUT'
        params.defines['CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ'] = 'CFG_TUD_AUDIO_EP_SZ_OUT'
        # params.defines['CFG_TUD_AUDIO_ENABLE_TYPE_I_DECODING'] = 0
        # params.defines['CFG_TUD_AUDIO_FUNC_1_CHANNEL_PER_FIFO_RX'] = 2
        # params.defines['CFG_TUD_AUDIO_FUNC_1_N_RX_SUPP_SW_FIFO'] = 1

    return audio_descr

def build_msc_descr(name, func, params):
    params.defines['CFG_TUD_MSC'] += 1
    params.defines['CFG_TUD_MSC_EP_BUFSIZE'] = 512

def build_full_descriptor(data):
    # build descriptor
    descr = []
    
    params = dummy()
    params.epsize = U16_TO_U8S_LE(64)
    params.total_len = 0
    params.itfnum = 0
    params.epnum = 1 #0 is reserved
    params.entnum = 1
    params.strings = list(data['strings'])
    params.strnum = len(params.strings)
    params[''] = ()
    params.itf_use = []
    params.ent_use = ['reserved']
    params.ep_use = ['reserved']
    params.cmts = []
    params.defines = {
        'CFG_TUD_CDC': 0,
        'CFG_TUD_MIDI': 0,
        'CFG_TUD_AUDIO': 0,
        'CFG_TUD_MSC': 0,
        'CFG_TUD_ENDPOINT0_SIZE': 64,
    }
    
    conf_descr = build_conf_descr(data, params)
    params.total_len += len(conf_descr)
    descr.extend(conf_descr)

    for name, func in data['functions'].items():
        if func['type'] == 'CDC':
            cdc_descr = build_cdc_descr(name, func, params)
            params.total_len += len(cdc_descr)
            descr.extend(cdc_descr)
        elif func['type'] == 'MIDI':
            midi_descr = build_midi_descr(name, func, params)
            params.total_len += len(midi_descr)
            descr.extend(midi_descr)
        elif func['type'] == 'Audio':
            audio_descr = build_audio_descr(name, func, params)
            params.total_len += len(audio_descr)
            descr.extend(audio_descr)
        elif func['type'] == 'MSC':
            pass

    params.total_len = U16_TO_U8S_LE(params.total_len)

    descr = replace_params(descr, params)

    assert len(descr) == TU_U16(params.total_len[1], params.total_len[0])

    # return descriptor
    return bytes(descr), params

def save_desc_to_files(desc, param):
    # Save to tusb_config.h
    current_dir = os.path.dirname(os.path.abspath(__file__))
    file_path = os.path.join(current_dir, 'autogen_tusb_config.h')
    param.defines['STRING_ARRAY_SIZE'] = len(param.strings)

    with open(file_path, 'w') as file:
        # Your code here
        file.write('''
//--------------------------------------------------------------------
// START OF AUTOCONFIG_PY_SCRIPT - autogen_tusb_config.h
//--------------------------------------------------------------------
''')

        file.write(f'//------------- DEFINES -------------//\n')
        for k, v in sorted(param.defines.items()):
            file.write(f'#define {k} {v}\n')
        file.write('\n')
        
        file.write(f"/* INTERFACES */\n")
        file.write('enum {\n')
        for i, s in enumerate(param.itf_use):
            file.write(f'    ITF_NUM_{s} = {i},\n')
        file.write('};\n\n')

        if len(param.ep_use) > 0:
            file.write(f"/* ENDPOINTS */\n")
            file.write('enum {\n')
            for i, s in enumerate(param.ep_use):
                file.write(f'    EP_NUM_{s} = {i},\n')
            file.write('};\n\n')

        if len(param.ent_use) > 0:
            file.write(f"/* AUDIO ENTITIES */\n")
            file.write('enum {\n')
            for i, s in enumerate(param.ent_use):
                file.write(f'    ENT_NUM_{s} = {i},\n')
            file.write('};\n\n') 

        file.write('''
//--------------------------------------------------------------------
// END OF AUTOCONFIG_PY_SCRIPT
//--------------------------------------------------------------------
''')

    # Save to usb_descriptors.cpp
    file_path = os.path.join(current_dir, 'autogen_usb_descriptors.h')
    with open(file_path, 'w') as file:
        file.write('''
//--------------------------------------------------------------------
// START OF AUTOCONFIG_PY_SCRIPT - autogen_usb_descriptors.h
//--------------------------------------------------------------------
''')

        file.write('#include "cinttypes"\n')
        file.write('#define NULL __null\n')
        file.write('\n')

        file.write(
        '''
//--------------------------------------------------------------------+
// String array
//--------------------------------------------------------------------+
''')

        file.write('static char const * const string_desc_arr[] =\n')
        file.write('{\n')
        for i, s in enumerate(param.strings):
            if i==0:
                file.write('    (const char[]){0x09, 0x04},\n')
                continue
            if s is None:
                file.write('    NULL,\n')
                continue
            file.write(f'    "{s}",\n')
        file.write('};\n\n')

        file.write('''
//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+
''')

        file.write('static const uint8_t desc_fs_configuration[] = {\n')
        cmts = list(param.cmts)
        cmt_pos, cmt_txt = cmts.pop(0)
        i=0
        endl = True
        for n in desc:
            while cmt_pos != -1 and i == cmt_pos:
                if i == cmt_pos:
                    if not endl:
                        file.write('\n')
                    file.write(f"/* {cmt_txt} */\\\n")
                    endl = True
                if len(cmts) > 0:
                    cmt_pos, cmt_txt = cmts.pop(0)
                else:
                    cmt_pos = -1
            file.write(f"{hex(n)}, ")
            endl = False
            i+=1
        if not endl:
            file.write('\n')
        file.write('};\n')

        file.write(
        '''
//--------------------------------------------------------------------
// END OF AUTOCONFIG_PY_SCRIPT
//--------------------------------------------------------------------
''')

    # return

    # print('desc_fs_configuration: ')
    # cmts = list(param.cmts)
    # cmt_pos, cmt_txt = cmts.pop(0)
    # i=0
    # endl = True
    # for n in desc:
    #     cmt = ''
    #     while cmt_pos != -1 and i == cmt_pos:
    #         if i == cmt_pos:
    #             cmt += f"/* {cmt_txt} */"
    #         if len(cmts) > 0:
    #             cmt_pos, cmt_txt = cmts.pop(0)
    #         else:
    #             cmt_pos = -1
    #     if cmt != '':
    #         print(f"0x{hex(n)[2:].zfill(2)}, {cmt}")
    #     else:
    #         print(f"0x{hex(n)[2:].zfill(2)}")
    #     i+=1

desc, param = build_full_descriptor(conf_descr_data)

save_desc_to_files(desc, param)