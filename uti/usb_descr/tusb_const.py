def TU_MIN(_x, _y):
    return _x if _x < _y else _y

def TU_MAX(_x, _y):
    return _x if _x > _y else _y

def TU_DIV_CEIL(n, d):
    return (n + d - 1) // d

def TU_U16(_high, _low):
    return (_high << 8) | _low

def TU_U16_HIGH(_u16):
    return (_u16 >> 8) & 0x00ff

def TU_U16_LOW(_u16):
    return _u16 & 0x00ff

def U16_TO_U8S_BE(_u16):
    return TU_U16_HIGH(_u16), TU_U16_LOW(_u16)

def U16_TO_U8S_LE(_u16):
    return TU_U16_LOW(_u16), TU_U16_HIGH(_u16)

def TU_U32_BYTE3(_u32):
    return (_u32 >> 24) & 0x000000ff

def TU_U32_BYTE2(_u32):
    return (_u32 >> 16) & 0x000000ff

def TU_U32_BYTE1(_u32):
    return (_u32 >> 8) & 0x000000ff

def TU_U32_BYTE0(_u32):
    return _u32 & 0x000000ff

def U32_TO_U8S_BE(_u32):
    return TU_U32_BYTE3(_u32), TU_U32_BYTE2(_u32), TU_U32_BYTE1(_u32), TU_U32_BYTE0(_u32)

def U32_TO_U8S_LE(_u32):
    return TU_U32_BYTE0(_u32), TU_U32_BYTE1(_u32), TU_U32_BYTE2(_u32), TU_U32_BYTE3(_u32)

def TU_BIT(n):
    return 1 << n

def TU_GENMASK(h, l):
    return (0xffffffff << l) & (0xffffffff >> (31 - h))

TUSB_CLASS_UNSPECIFIED          = 0   
TUSB_CLASS_AUDIO                = 1   
TUSB_CLASS_CDC                  = 2   
TUSB_CLASS_HID                  = 3   
TUSB_CLASS_RESERVED_4           = 4   
TUSB_CLASS_PHYSICAL             = 5   
TUSB_CLASS_IMAGE                = 6   
TUSB_CLASS_PRINTER              = 7   
TUSB_CLASS_MSC                  = 8   
TUSB_CLASS_HUB                  = 9   
TUSB_CLASS_CDC_DATA             = 10  
TUSB_CLASS_SMART_CARD           = 11  
TUSB_CLASS_RESERVED_12          = 12  
TUSB_CLASS_CONTENT_SECURITY     = 13  
TUSB_CLASS_VIDEO                = 14  
TUSB_CLASS_PERSONAL_HEALTHCARE  = 15  
TUSB_CLASS_AUDIO_VIDEO          = 16  
TUSB_CLASS_DIAGNOSTIC           = 0xDC
TUSB_CLASS_WIRELESS_CONTROLLER  = 0xE0
TUSB_CLASS_MISC                 = 0xEF
TUSB_CLASS_APPLICATION_SPECIFIC = 0xFE
TUSB_CLASS_VENDOR_SPECIFIC      = 0xFF

TUSB_DESC_DEVICE = 0x01
TUSB_DESC_CONFIGURATION = 0x02
TUSB_DESC_STRING = 0x03
TUSB_DESC_INTERFACE = 0x04
TUSB_DESC_ENDPOINT = 0x05
TUSB_DESC_DEVICE_QUALIFIER = 0x06
TUSB_DESC_OTHER_SPEED_CONFIG = 0x07
TUSB_DESC_INTERFACE_POWER = 0x08
TUSB_DESC_OTG = 0x09
TUSB_DESC_DEBUG = 0x0A
TUSB_DESC_INTERFACE_ASSOCIATION = 0x0B
TUSB_DESC_BOS = 0x0F
TUSB_DESC_DEVICE_CAPABILITY = 0x10
TUSB_DESC_FUNCTIONAL = 0x21
TUSB_DESC_CS_DEVICE = 0x21
TUSB_DESC_CS_CONFIGURATION = 0x22
TUSB_DESC_CS_STRING = 0x23
TUSB_DESC_CS_INTERFACE = 0x24
TUSB_DESC_CS_ENDPOINT = 0x25
TUSB_DESC_SUPERSPEED_ENDPOINT_COMPANION = 0x30
TUSB_DESC_SUPERSPEED_ISO_ENDPOINT_COMPANION = 0x31

TUSB_XFER_CONTROL = 0
TUSB_XFER_ISOCHRONOUS = 1
TUSB_XFER_BULK = 2
TUSB_XFER_INTERRUPT = 3

TUSB_ISO_EP_ATT_NO_SYNC = 0x00
TUSB_ISO_EP_ATT_ASYNCHRONOUS = 0x04
TUSB_ISO_EP_ATT_ADAPTIVE = 0x08
TUSB_ISO_EP_ATT_SYNCHRONOUS = 0x0C
TUSB_ISO_EP_ATT_DATA = 0x00
TUSB_ISO_EP_ATT_EXPLICIT_FB = 0x10
TUSB_ISO_EP_ATT_IMPLICIT_FB = 0x20


TUD_AUDIO_DESC_IAD_LEN = 8
TUD_AUDIO_DESC_STD_AC_LEN = 9
TUD_AUDIO_DESC_CS_AC_LEN = 9
TUD_AUDIO_DESC_CLK_SRC_LEN = 8

AUDIO_FUNCTION_SUBCLASS_UNDEFINED = 0x00

AUDIO_SUBCLASS_UNDEFINED = 0x00
AUDIO_SUBCLASS_CONTROL   = 0x01
AUDIO_SUBCLASS_STREAMING       =0x02
AUDIO_SUBCLASS_MIDI_STREAMING  =0x03

AUDIO_FUNC_UNDEF              = 0x00
AUDIO_FUNC_DESKTOP_SPEAKER    = 0x01
AUDIO_FUNC_HOME_THEATER       = 0x02
AUDIO_FUNC_MICROPHONE         = 0x03
AUDIO_FUNC_HEADSET            = 0x04
AUDIO_FUNC_TELEPHONE          = 0x05
AUDIO_FUNC_CONVERTER          = 0x06
AUDIO_FUNC_SOUND_RECODER      = 0x07
AUDIO_FUNC_IO_BOX             = 0x08
AUDIO_FUNC_MUSICAL_INSTRUMENT = 0x09
AUDIO_FUNC_PRO_AUDIO          = 0x0A
AUDIO_FUNC_AUDIO_VIDEO        = 0x0B
AUDIO_FUNC_CONTROL_PANEL      = 0x0C
AUDIO_FUNC_OTHER              = 0xFF

AUDIO_FUNC_PROTOCOL_CODE_UNDEF       = 0x00
AUDIO_FUNC_PROTOCOL_CODE_V2          = 0x20

AUDIO_CS_AS_INTERFACE_CTRL_LATENCY_POS  = 0

AUDIO_INT_PROTOCOL_CODE_UNDEF       = 0x00
AUDIO_INT_PROTOCOL_CODE_V2          = 0x20

AUDIO_CS_AC_INTERFACE_AC_DESCRIPTOR_UNDEF = 0x00
AUDIO_CS_AC_INTERFACE_HEADER = 0x01
AUDIO_CS_AC_INTERFACE_INPUT_TERMINAL = 0x02
AUDIO_CS_AC_INTERFACE_OUTPUT_TERMINAL = 0x03
AUDIO_CS_AC_INTERFACE_MIXER_UNIT = 0x04
AUDIO_CS_AC_INTERFACE_SELECTOR_UNIT = 0x05
AUDIO_CS_AC_INTERFACE_FEATURE_UNIT = 0x06
AUDIO_CS_AC_INTERFACE_EFFECT_UNIT = 0x07
AUDIO_CS_AC_INTERFACE_PROCESSING_UNIT = 0x08
AUDIO_CS_AC_INTERFACE_EXTENSION_UNIT = 0x09
AUDIO_CS_AC_INTERFACE_CLOCK_SOURCE = 0x0A
AUDIO_CS_AC_INTERFACE_CLOCK_SELECTOR = 0x0B
AUDIO_CS_AC_INTERFACE_CLOCK_MULTIPLIER = 0x0C
AUDIO_CS_AC_INTERFACE_SAMPLE_RATE_CONVERTER = 0x0D

AUDIO_CS_AS_INTERFACE_AS_DESCRIPTOR_UNDEF = 0x00
AUDIO_CS_AS_INTERFACE_AS_GENERAL = 0x01
AUDIO_CS_AS_INTERFACE_FORMAT_TYPE = 0x02
AUDIO_CS_AS_INTERFACE_ENCODER = 0x03
AUDIO_CS_AS_INTERFACE_DECODER = 0x04

AUDIO_TERM_TYPE_USB_UNDEFINED = 0x0100
AUDIO_TERM_TYPE_USB_STREAMING = 0x0101
AUDIO_TERM_TYPE_USB_VENDOR_SPEC = 0x01FF

AUDIO_TERM_TYPE_IN_UNDEFINED = 0x0200
AUDIO_TERM_TYPE_IN_GENERIC_MIC = 0x0201
AUDIO_TERM_TYPE_IN_DESKTOP_MIC = 0x0202
AUDIO_TERM_TYPE_IN_PERSONAL_MIC = 0x0203
AUDIO_TERM_TYPE_IN_OMNI_MIC = 0x0204
AUDIO_TERM_TYPE_IN_ARRAY_MIC = 0x0205
AUDIO_TERM_TYPE_IN_PROC_ARRAY_MIC = 0x0206

AUDIO_TERM_TYPE_OUT_UNDEFINED = 0x0300
AUDIO_TERM_TYPE_OUT_GENERIC_SPEAKER = 0x0301
AUDIO_TERM_TYPE_OUT_HEADPHONES = 0x0302
AUDIO_TERM_TYPE_OUT_HEAD_MNT_DISP_AUIDO = 0x0303
AUDIO_TERM_TYPE_OUT_DESKTOP_SPEAKER = 0x0304
AUDIO_TERM_TYPE_OUT_ROOM_SPEAKER = 0x0305
AUDIO_TERM_TYPE_OUT_COMMUNICATION_SPEAKER = 0x0306
AUDIO_TERM_TYPE_OUT_LOW_FRQ_EFFECTS_SPEAKER = 0x0307

AUDIO_CHANNEL_CONFIG_NON_PREDEFINED             = 0x00000000
AUDIO_CHANNEL_CONFIG_FRONT_LEFT                 = 0x00000001
AUDIO_CHANNEL_CONFIG_FRONT_RIGHT                = 0x00000002
AUDIO_CHANNEL_CONFIG_FRONT_CENTER               = 0x00000004
AUDIO_CHANNEL_CONFIG_LOW_FRQ_EFFECTS            = 0x00000008
AUDIO_CHANNEL_CONFIG_BACK_LEFT                  = 0x00000010
AUDIO_CHANNEL_CONFIG_BACK_RIGHT                 = 0x00000020
AUDIO_CHANNEL_CONFIG_FRONT_LEFT_OF_CENTER       = 0x00000040
AUDIO_CHANNEL_CONFIG_FRONT_RIGHT_OF_CENTER      = 0x00000080
AUDIO_CHANNEL_CONFIG_BACK_CENTER                = 0x00000100
AUDIO_CHANNEL_CONFIG_SIDE_LEFT                  = 0x00000200
AUDIO_CHANNEL_CONFIG_SIDE_RIGHT                 = 0x00000400
AUDIO_CHANNEL_CONFIG_TOP_CENTER                 = 0x00000800
AUDIO_CHANNEL_CONFIG_TOP_FRONT_LEFT             = 0x00001000
AUDIO_CHANNEL_CONFIG_TOP_FRONT_CENTER           = 0x00002000
AUDIO_CHANNEL_CONFIG_TOP_FRONT_RIGHT            = 0x00004000
AUDIO_CHANNEL_CONFIG_TOP_BACK_LEFT              = 0x00008000
AUDIO_CHANNEL_CONFIG_TOP_BACK_CENTER            = 0x00010000
AUDIO_CHANNEL_CONFIG_TOP_BACK_RIGHT             = 0x00020000
AUDIO_CHANNEL_CONFIG_TOP_FRONT_LEFT_OF_CENTER   = 0x00040000
AUDIO_CHANNEL_CONFIG_TOP_FRONT_RIGHT_OF_CENTER  = 0x00080000
AUDIO_CHANNEL_CONFIG_LEFT_LOW_FRQ_EFFECTS       = 0x00100000
AUDIO_CHANNEL_CONFIG_RIGHT_LOW_FRQ_EFFECTS      = 0x00200000
AUDIO_CHANNEL_CONFIG_TOP_SIDE_LEFT              = 0x00400000
AUDIO_CHANNEL_CONFIG_TOP_SIDE_RIGHT             = 0x00800000
AUDIO_CHANNEL_CONFIG_BOTTOM_CENTER              = 0x01000000
AUDIO_CHANNEL_CONFIG_BACK_LEFT_OF_CENTER        = 0x02000000
AUDIO_CHANNEL_CONFIG_BACK_RIGHT_OF_CENTER       = 0x04000000
AUDIO_CHANNEL_CONFIG_RAW_DATA                   = 0x80000000

AUDIO_FEATURE_UNIT_CTRL_MUTE_POS            = 0
AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS          = 2
AUDIO_FEATURE_UNIT_CTRL_BASS_POS            = 4
AUDIO_FEATURE_UNIT_CTRL_MID_POS             = 6
AUDIO_FEATURE_UNIT_CTRL_TREBLE_POS          = 8
AUDIO_FEATURE_UNIT_CTRL_GRAPHIC_EQU_POS     = 10
AUDIO_FEATURE_UNIT_CTRL_AGC_POS             = 12
AUDIO_FEATURE_UNIT_CTRL_DELAY_POS           = 14
AUDIO_FEATURE_UNIT_CTRL_BASS_BOOST_POS      = 16
AUDIO_FEATURE_UNIT_CTRL_LOUDNESS_POS        = 18
AUDIO_FEATURE_UNIT_CTRL_INPUT_GAIN_POS      = 20
AUDIO_FEATURE_UNIT_CTRL_INPUT_GAIN_PAD_POS  = 22
AUDIO_FEATURE_UNIT_CTRL_PHASE_INV_POS       = 24
AUDIO_FEATURE_UNIT_CTRL_UNDERFLOW_POS       = 26
AUDIO_FEATURE_UNIT_CTRL_OVERFLOW_POS        = 28

AUDIO_CTRL_NONE = 0x00
AUDIO_CTRL_R = 0x01
AUDIO_CTRL_RW = 0x03

AUDIO_IN_TERM_CTRL_CPY_PROT_POS = 0
AUDIO_IN_TERM_CTRL_CONNECTOR_POS = 2
AUDIO_IN_TERM_CTRL_OVERLOAD_POS = 4
AUDIO_IN_TERM_CTRL_CLUSTER_POS = 6
AUDIO_IN_TERM_CTRL_UNDERFLOW_POS = 8
AUDIO_IN_TERM_CTRL_OVERFLOW_POS = 10

AUDIO_CLOCK_SOURCE_ATT_EXT_CLK = 0x00
AUDIO_CLOCK_SOURCE_ATT_INT_FIX_CLK = 0x01
AUDIO_CLOCK_SOURCE_ATT_INT_VAR_CLK = 0x02
AUDIO_CLOCK_SOURCE_ATT_INT_PRO_CLK = 0x03
AUDIO_CLOCK_SOURCE_ATT_CLK_SYC_SOF = 0x04

AUDIO_CLOCK_SOURCE_CTRL_CLK_FRQ_POS = 0
AUDIO_CLOCK_SOURCE_CTRL_CLK_VAL_POS = 2

AUDIO_FORMAT_TYPE_UNDEFINED = 0x00
AUDIO_FORMAT_TYPE_I = 0x01
AUDIO_FORMAT_TYPE_II = 0x02
AUDIO_FORMAT_TYPE_III = 0x03
AUDIO_FORMAT_TYPE_IV = 0x04
AUDIO_EXT_FORMAT_TYPE_I = 0x81
AUDIO_EXT_FORMAT_TYPE_II = 0x82
AUDIO_EXT_FORMAT_TYPE_III = 0x83

AUDIO_DATA_FORMAT_TYPE_I_PCM = 0x00000001
AUDIO_DATA_FORMAT_TYPE_I_PCM8 = 0x00000002
AUDIO_DATA_FORMAT_TYPE_I_IEEE_FLOAT = 0x00000004
AUDIO_DATA_FORMAT_TYPE_I_ALAW = 0x00000008
AUDIO_DATA_FORMAT_TYPE_I_MULAW = 0x00000010
AUDIO_DATA_FORMAT_TYPE_I_RAW_DATA = 0x80000000

AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_UNDEFINED = 0x00
AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_MILLISEC = 0x01
AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_PCM_SAMPLES = 0x02

AUDIO_CS_EP_SUBTYPE_UNDEF = 0x00
AUDIO_CS_EP_SUBTYPE_GENERAL = 0x01

AUDIO_CS_AS_ISO_DATA_EP_ATT_MAX_PACKETS_ONLY = 0x80
AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK = 0x00

def TUD_AUDIO_EP_SIZE(_maxFrequency, _nBytesPerSample, _nChannels):
    return ((((_maxFrequency + 999) // 1000) + 1) * _nBytesPerSample * _nChannels)

MIDI_JACK_EMBEDDED = 0x01
MIDI_JACK_EXTERNAL = 0x02

MIDI_CS_INTERFACE_HEADER    = 0x01
MIDI_CS_INTERFACE_IN_JACK   = 0x02
MIDI_CS_INTERFACE_OUT_JACK  = 0x03
MIDI_CS_INTERFACE_ELEMENT   = 0x04

MIDI_CS_ENDPOINT_GENERAL = 0x01

TUD_MIDI_DESC_HEAD_LEN = (9 + 9 + 9 + 7)
TUD_MIDI_DESC_JACK_LEN = (6 + 6 + 9 + 9)

def TUD_MIDI_DESC_EP_LEN(_numcables):
    return (9 + 4 + (_numcables))

def TUD_MIDI_JACKID_IN_EMB(_cablenum):
    return (((_cablenum) - 1) * 4 + 1)

def TUD_MIDI_JACKID_IN_EXT(_cablenum):
    return (((_cablenum) - 1) * 4 + 2)

def TUD_MIDI_JACKID_OUT_EMB(_cablenum):
    return (((_cablenum) - 1) * 4 + 3)

def TUD_MIDI_JACKID_OUT_EXT(_cablenum):
    return (((_cablenum) - 1) * 4 + 4)

TUD_CDC_DESC_LEN = (8+9+5+5+4+5+7+9+7+7)

CDC_COMM_SUBCLASS_DIRECT_LINE_CONTROL_MODEL      = 0x01  # Direct Line Control Model         [USBPSTN1.2]
CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL         = 0x02  # Abstract Control Model            [USBPSTN1.2]
CDC_COMM_SUBCLASS_TELEPHONE_CONTROL_MODEL        = 0x03  # Telephone Control Model           [USBPSTN1.2]
CDC_COMM_SUBCLASS_MULTICHANNEL_CONTROL_MODEL     = 0x04  # Multi-Channel Control Model       [USBISDN1.2]
CDC_COMM_SUBCLASS_CAPI_CONTROL_MODEL             = 0x05  # CAPI Control Model                [USBISDN1.2]
CDC_COMM_SUBCLASS_ETHERNET_CONTROL_MODEL         = 0x06  # Ethernet Networking Control Model [USBECM1.2]
CDC_COMM_SUBCLASS_ATM_NETWORKING_CONTROL_MODEL   = 0x07  # ATM Networking Control Model      [USBATM1.2]
CDC_COMM_SUBCLASS_WIRELESS_HANDSET_CONTROL_MODEL = 0x08  # Wireless Handset Control Model    [USBWMC1.1]
CDC_COMM_SUBCLASS_DEVICE_MANAGEMENT              = 0x09  # Device Management                 [USBWMC1.1]
CDC_COMM_SUBCLASS_MOBILE_DIRECT_LINE_MODEL       = 0x0A  # Mobile Direct Line Model          [USBWMC1.1]
CDC_COMM_SUBCLASS_OBEX                           = 0x0B  # OBEX                              [USBWMC1.1]
CDC_COMM_SUBCLASS_ETHERNET_EMULATION_MODEL       = 0x0C  # Ethernet Emulation Model          [USBEEM1.0]
CDC_COMM_SUBCLASS_NETWORK_CONTROL_MODEL          = 0x0D  # Network Control Model             [USBNCM1.0]

CDC_COMM_PROTOCOL_NONE                          = 0x00  # No specific protocol
CDC_COMM_PROTOCOL_ATCOMMAND                     = 0x01  # AT Commands: V.250 etc
CDC_COMM_PROTOCOL_ATCOMMAND_PCCA_101            = 0x02  # AT Commands defined by PCCA-101
CDC_COMM_PROTOCOL_ATCOMMAND_PCCA_101_AND_ANNEXO = 0x03  # AT Commands defined by PCCA-101 & Annex O
CDC_COMM_PROTOCOL_ATCOMMAND_GSM_707             = 0x04  # AT Commands defined by GSM 07.07
CDC_COMM_PROTOCOL_ATCOMMAND_3GPP_27007          = 0x05  # AT Commands defined by 3GPP 27.007
CDC_COMM_PROTOCOL_ATCOMMAND_CDMA                = 0x06  # AT Commands defined by TIA for CDMA
CDC_COMM_PROTOCOL_ETHERNET_EMULATION_MODEL      = 0x07  # Ethernet Emulation Model

CDC_FUNC_DESC_HEADER = 0x00  # Header Functional Descriptor, marks the beginning of the concatenated set of functional descriptors for the interface.
CDC_FUNC_DESC_CALL_MANAGEMENT                                  = 0x01  # Call Management Functional Descriptor
CDC_FUNC_DESC_ABSTRACT_CONTROL_MANAGEMENT                      = 0x02  # Abstract Control Management Functional Descriptor
CDC_FUNC_DESC_DIRECT_LINE_MANAGEMENT                           = 0x03  # Direct Line Management Functional Descriptor
CDC_FUNC_DESC_TELEPHONE_RINGER                                 = 0x04  # Telephone Ringer Functional Descriptor
CDC_FUNC_DESC_TELEPHONE_CALL_AND_LINE_STATE_REPORTING_CAPACITY = 0x05  # Telephone Call and Line State Reporting Capabilities Functional Descriptor
CDC_FUNC_DESC_UNION                                            = 0x06  # Union Functional Descriptor
CDC_FUNC_DESC_COUNTRY_SELECTION                                = 0x07  # Country Selection Functional Descriptor
CDC_FUNC_DESC_TELEPHONE_OPERATIONAL_MODES                      = 0x08  # Telephone Operational ModesFunctional Descriptor
CDC_FUNC_DESC_USB_TERMINAL                                     = 0x09  # USB Terminal Functional Descriptor
CDC_FUNC_DESC_NETWORK_CHANNEL_TERMINAL                         = 0x0A  # Network Channel Terminal Descriptor
CDC_FUNC_DESC_PROTOCOL_UNIT                                    = 0x0B  # Protocol Unit Functional Descriptor
CDC_FUNC_DESC_EXTENSION_UNIT                                   = 0x0C  # Extension Unit Functional Descriptor
CDC_FUNC_DESC_MULTICHANEL_MANAGEMENT                           = 0x0D  # Multi-Channel Management Functional Descriptor
CDC_FUNC_DESC_CAPI_CONTROL_MANAGEMENT                          = 0x0E  # CAPI Control Management Functional Descriptor
CDC_FUNC_DESC_ETHERNET_NETWORKING                              = 0x0F  # Ethernet Networking Functional Descriptor
CDC_FUNC_DESC_ATM_NETWORKING                                   = 0x10  # ATM Networking Functional Descriptor
CDC_FUNC_DESC_WIRELESS_HANDSET_CONTROL_MODEL                   = 0x11  # Wireless Handset Control Model Functional Descriptor
CDC_FUNC_DESC_MOBILE_DIRECT_LINE_MODEL                         = 0x12  # Mobile Direct Line Model Functional Descriptor
CDC_FUNC_DESC_MOBILE_DIRECT_LINE_MODEL_DETAIL                  = 0x13  # MDLM Detail Functional Descriptor
CDC_FUNC_DESC_DEVICE_MANAGEMENT_MODEL                          = 0x14  # Device Management Model Functional Descriptor
CDC_FUNC_DESC_OBEX                                             = 0x15  # OBEX Functional Descriptor
CDC_FUNC_DESC_COMMAND_SET                                      = 0x16  # Command Set Functional Descriptor
CDC_FUNC_DESC_COMMAND_SET_DETAIL                               = 0x17  # Command Set Detail Functional Descriptor
CDC_FUNC_DESC_TELEPHONE_CONTROL_MODEL                          = 0x18  # Telephone Control Model Functional Descriptor
CDC_FUNC_DESC_OBEX_SERVICE_IDENTIFIER                          = 0x19  # OBEX Service Identifier Functional Descriptor
CDC_FUNC_DESC_NCM                                              = 0x1A  # NCM Functional Descriptor


