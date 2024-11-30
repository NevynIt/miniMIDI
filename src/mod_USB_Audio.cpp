#include "mod_USB_Audio.h"
#include "App.h"
#include "stdio.h"
#include "midi_frequencies.h"
#include "dsp/wave.h"

// Audio controls
#define CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX 2

// Current states
bool mute[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX + 1];       // +1 for master channel 0
uint16_t volume[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX + 1]; // +1 for master channel 0
uint32_t sampFreq = CFG_TUD_AUDIO_FUNC_1_SAMPLE_RATE;
uint8_t clkValid = 1;

// Range states
audio_control_range_2_n_t(1) volumeRng[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX + 1]; // Volume range state
audio_control_range_4_n_t(1) sampleFreqRng;                                     // Sample frequency range state

void mod_USB_Audio::Init()
{
    // Initialization code for USB Audio

}

void mod_USB_Audio::Tick()
{
    // Periodic tasks for USB Audio
}

int alt_in = 0;
int alt_out = 0;

void mod_USB_Audio::Test()
{
    INTERVALCHECK(1)

    int8_t slot = mMApp.dsp.getSlotRelative(2);

    static auto wave_left = dsp::RBJFilterWave<dsp::amModWave<dsp::gainModWave<dsp::inharmonicWave<5>>, dsp::envelopeBase>>();
    static auto wave_right = dsp::RBJFilterWave<dsp::amModWave<dsp::gainModWave<dsp::harmonicWave<5>>, dsp::envelopeBase>>();
    static bool setup_done = false;
    if (!setup_done)
    {
        float gains[5] = {1, 0.5, 0.25, 0.125, 0.0625};
        dsp::normalize(gains);
        wave_right.c.setHarmonics(gains);

        float ratios[5] = {1,3,5,10,15};
        wave_left.c.setup(ratios, gains);

        wave_left.m.setEnvTimes(0.025, 0.2, 0.1, SAMPLE_RATE);
        wave_left.m.setSustainLevel(dsp::SampleMax / 5);
        wave_right.m.setEnvTimes(0.025, 0.2, 0.1, SAMPLE_RATE);
        wave_right.m.setSustainLevel(dsp::SampleMax / 5);

        wave_left.lowpass(dsp::normalizeFreq(1000, SAMPLE_RATE), 5);
        wave_right.highpass(dsp::normalizeFreq(1000, SAMPLE_RATE), 5);
        setup_done = true;
    }

    static int ms = 0;
    if (ms>500)
    {
        wave_left.m.release();
        wave_right.m.release();
    }
    if (ms > 800)
    {
        wave_left.m.attack();
        wave_right.m.attack();
        ms = 0;
    }
    ms++;

    auto left = mMApp.dsp.buffers[track_out[0]][slot];
    auto right = mMApp.dsp.buffers[track_out[1]][slot];

    int noteL = mMApp.encoders.count[0]/4 + 69;
    auto ampL = dsp::SampleMax / 100 * (mMApp.encoders.count[1]/4 + 50);
    int noteR = mMApp.encoders.count[2]/4 + 69;
    auto ampR = dsp::SampleMax / 100 * (mMApp.encoders.count[3]/4 + 50);

    if (noteL < 0) noteL = 0;
    if (noteL > 127) noteL = 127;
    if (noteR < 0) noteR = 0;
    if (noteR > 127) noteR = 127;
    if (ampL < 0) ampL = 0;
    if (ampL > dsp::SampleMax) ampL = dsp::SampleMax;
    if (ampR < 0) ampR = 0;
    if (ampR > dsp::SampleMax) ampR = dsp::SampleMax;

    wave_left.c.setIncrement(dsp::inc_from_freq(midi_frequencies[noteL], SAMPLE_RATE));
    wave_right.c.setIncrement(dsp::inc_from_freq(midi_frequencies[noteR], SAMPLE_RATE));

    wave_left.c.setLevel(ampL);
    wave_right.c.setLevel(ampR);

    dsp::fillBuffer(left, wave_left, AUDIO_BUFFER_SAMPLES);
    dsp::fillBuffer(right, wave_right, AUDIO_BUFFER_SAMPLES);
}

void mod_USB_Audio::reset_slot_in()
{
    slot_in = -1;
}

void mod_USB_Audio::reset_slot_out()
{
    slot_out = -1;
}

void mod_USB_Audio::start_slot_in()
{
    slot_in = mMApp.dsp.getSlotRelative(2);
}

void mod_USB_Audio::start_slot_out()
{
    slot_out = mMApp.dsp.getSlotRelative(-1);
}

void mod_USB_Audio::next_slot_in()
{
    slot_in = (slot_in + 1) % AUDIO_BUFFER_SLOTS;
}

void mod_USB_Audio::next_slot_out()
{
    slot_out = (slot_out + 1) % AUDIO_BUFFER_SLOTS;
}

inline sample_ptr mod_USB_Audio::getOutBuffer(uint8_t channel) const
{
    if (slot_out < 0)
        __breakpoint();
    return mMApp.dsp.buffers[track_out[channel]][slot_out];
}

inline sample_ptr mod_USB_Audio::getInBuffer(uint8_t channel) const
{
    if (slot_in < 0)
        __breakpoint();
    return mMApp.dsp.buffers[track_in[channel]][slot_in];
}

//--------------------------------------------------------------------+
// Application Callback API Implementations
//--------------------------------------------------------------------+

// Invoked when audio class specific set request received for an EP
bool tud_audio_set_req_ep_cb(uint8_t rhport, tusb_control_request_t const *p_request, uint8_t *pBuff)
{
    (void)rhport;
    (void)pBuff;

    // We do not support any set range requests here, only current value requests
    TU_VERIFY(p_request->bRequest == AUDIO_CS_REQ_CUR);

    uint8_t channelNum = TU_U16_LOW(p_request->wValue);
    uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
    uint8_t ep = TU_U16_LOW(p_request->wIndex);

    //LOG_VERBOSE("Set request for EP: %u, channel: %u, control: %u\r\n", ep, channelNum, ctrlSel);

    (void)channelNum;
    (void)ctrlSel;
    (void)ep;

    return false; // Yet not implemented
}

// Invoked when audio class specific set request received for an interface
bool tud_audio_set_req_itf_cb(uint8_t rhport, tusb_control_request_t const *p_request, uint8_t *pBuff)
{
    (void)rhport;
    (void)pBuff;

    // We do not support any set range requests here, only current value requests
    TU_VERIFY(p_request->bRequest == AUDIO_CS_REQ_CUR);

    uint8_t channelNum = TU_U16_LOW(p_request->wValue);
    uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
    uint8_t itf = TU_U16_LOW(p_request->wIndex);

    (void)channelNum;
    (void)ctrlSel;
    (void)itf;

    //LOG_VERBOSE("Set request for interface: %u, channel: %u, control: %u\r\n", itf, channelNum, ctrlSel);

    return false; // Yet not implemented
}

// Invoked when audio class specific get request received for an EP
bool tud_audio_get_req_ep_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    (void)rhport;

    uint8_t channelNum = TU_U16_LOW(p_request->wValue);
    uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
    uint8_t ep = TU_U16_LOW(p_request->wIndex);

    (void)channelNum;
    (void)ctrlSel;
    (void)ep;

    //LOG_VERBOSE("Get request for EP: %u, channel: %u, control: %u\r\n", ep, channelNum, ctrlSel);
    //	return tud_control_xfer(rhport, p_request, &tmp, 1);

    return false; // Yet not implemented
}

// Invoked when audio set interface request received
bool tud_audio_set_itf_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    (void)rhport;
    uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
    uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

    //LOG_VERBOSE("Audio set interface: itf=%d, alt=%d\r\n", itf, alt);

    if (itf == ITF_NUM_mM_Sound_LineIn)
    {
        // Line In
        mMApp.usbAudio.start_slot_out();
        alt_out = alt;
    }
    else if (itf == ITF_NUM_mM_Sound_LineOut)
    {
        // Line Out
        mMApp.usbAudio.start_slot_in();
        alt_in = alt;
    }
    return true;
}

// Invoked when audio class specific get request received for an interface
bool tud_audio_get_req_itf_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    (void)rhport;

    uint8_t channelNum = TU_U16_LOW(p_request->wValue);
    uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
    uint8_t itf = TU_U16_LOW(p_request->wIndex);

    (void)channelNum;
    (void)ctrlSel;
    (void)itf;

    //LOG_VERBOSE("Get request for interface: %u, channel: %u, control: %u\r\n", itf, channelNum, ctrlSel);
    return false; // Yet not implemented
}

bool tud_audio_tx_done_pre_load_cb(uint8_t rhport, uint8_t itf, uint8_t ep_in, uint8_t cur_alt_setting)
{
    (void)rhport;
    (void)itf;
    (void)ep_in;
    (void)cur_alt_setting;

    // //LOG_VERBOSE("tud_audio_tx_done_pre_load_cb\r\n");

    if (mMApp.usbAudio.slot_out < 0)
    {
        __breakpoint();
        return false;
    }

    if (alt_out == 0)
        return true;

    auto buf = mMApp.usbAudio.buffer_out;
    sample_ptr left = mMApp.usbAudio.getOutBuffer(0);
    sample_ptr right = mMApp.usbAudio.getOutBuffer(1);
    for (size_t cnt = 0; cnt < AUDIO_BUFFER_SAMPLES * 2;)
    {
        buf[cnt++] = *left++;
        buf[cnt++] = *right++;
    }
    mMApp.usbAudio.next_slot_out();
    
    tud_audio_write((uint8_t *)buf, 2 * AUDIO_BUFFER_SAMPLES * BITS_PER_SAMPLE / 8);

    return true;
}

bool tud_audio_tx_done_post_load_cb(uint8_t rhport, uint16_t n_bytes_copied, uint8_t itf, uint8_t ep_in, uint8_t cur_alt_setting)
{
    (void)rhport;
    (void)n_bytes_copied;
    (void)itf;
    (void)ep_in;
    (void)cur_alt_setting;

    return true;
}

// Invoked when audio set interface request received which closes an EP
bool tud_audio_set_itf_close_EP_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    (void)rhport;
    (void)p_request;
    uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
    uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

    //LOG_VERBOSE("tud_audio_set_itf_close_EP_cb itf=%d, alt=%d\r\n", itf, alt);

    if (itf == ITF_NUM_mM_Sound_LineIn)
    {
        // Line In
        mMApp.usbAudio.reset_slot_out();
    }
    else if (itf == ITF_NUM_mM_Sound_LineOut)
    {
        // Line Out
        mMApp.usbAudio.reset_slot_in();
    }
    return true;
}

bool tud_audio_rx_done_pre_read_cb(uint8_t rhport, uint16_t n_bytes_received, uint8_t func_id, uint8_t ep_out, uint8_t cur_alt_setting)
{
    (void)rhport;
    (void)n_bytes_received;
    (void)func_id;
    (void)ep_out;
    (void)cur_alt_setting;

    //LOG_VERBOSE("tud_audio_rx_done_pre_read_cb\r\n");

    if (mMApp.usbAudio.slot_in < 0)
        return false;

    auto buf = mMApp.usbAudio.buffer_in;
    uint16_t num_read = tud_audio_read(buf, 2 * AUDIO_BUFFER_SAMPLES * BITS_PER_SAMPLE / 8);

    sample_ptr left = mMApp.usbAudio.getInBuffer(0);
    sample_ptr right = mMApp.usbAudio.getInBuffer(1);
    for (size_t cnt = 0; cnt < AUDIO_BUFFER_SAMPLES * 2;)
    {
        *left++ = buf[cnt++];
        *right++ = buf[cnt++];
    }
    mMApp.usbAudio.next_slot_in();

    return true;
}

bool tud_audio_rx_done_post_read_cb(uint8_t rhport, uint16_t n_bytes_received, uint8_t func_id, uint8_t ep_out, uint8_t cur_alt_setting)
{
    (void)rhport;
    (void)n_bytes_received;
    (void)func_id;
    (void)ep_out;
    (void)cur_alt_setting;

    //LOG_VERBOSE("tud_audio_rx_done_post_read_cb\r\n");

    return true;
}

//////////////////////////SECOND IMPLEMENTATION//////////////////////////

// Helper for clock get requests
static bool tud_audio_clock_get_request(uint8_t rhport, audio_control_request_t const *request)
{
    // //LOG_VERBOSE("tud_audio_clock_get_request\r\n");

    TU_ASSERT(request->bEntityID == ENT_NUM_mM_Sound_clock);

    if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ)
    {
        if (request->bRequest == AUDIO_CS_REQ_CUR)
        {
            TU_LOG2("Clock get current freq %" PRIu32 "\r\n", sampFreq);

            audio_control_cur_4_t curf = {(int32_t)tu_htole32(sampFreq)};
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &curf, sizeof(curf));
        }
        else if (request->bRequest == AUDIO_CS_REQ_RANGE)
        {
            audio_control_range_4_n_t(1) rangef =
                {
                    .wNumSubRanges = tu_htole16(1)};
            TU_LOG2("Clock get %d freq ranges\r\n", 1);
            for (uint8_t i = 0; i < 1; i++)
            {
                rangef.subrange[i].bMin = (int32_t)SAMPLE_RATE;
                rangef.subrange[i].bMax = (int32_t)SAMPLE_RATE;
                rangef.subrange[i].bRes = 0;
                TU_LOG2("Range %d (%d, %d, %d)\r\n", i, (int)rangef.subrange[i].bMin, (int)rangef.subrange[i].bMax, (int)rangef.subrange[i].bRes);
            }

            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &rangef, sizeof(rangef));
        }
    }
    else if (request->bControlSelector == AUDIO_CS_CTRL_CLK_VALID &&
             request->bRequest == AUDIO_CS_REQ_CUR)
    {
        audio_control_cur_1_t cur_valid = {.bCur = 1};
        TU_LOG2("Clock get is valid %u\r\n", cur_valid.bCur);
        return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_valid, sizeof(cur_valid));
    }
    TU_LOG1("Clock get request not supported, entity = %u, selector = %u, request = %u\r\n",
            request->bEntityID, request->bControlSelector, request->bRequest);
    return false;
}

// Helper for clock set requests
static bool tud_audio_clock_set_request(uint8_t rhport, audio_control_request_t const *request, uint8_t const *buf)
{
    (void)rhport;

    //LOG_VERBOSE("tud_audio_clock_set_request\r\n");

    TU_ASSERT(request->bEntityID == ENT_NUM_mM_Sound_clock);
    TU_VERIFY(request->bRequest == AUDIO_CS_REQ_CUR);

    if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ)
    {
        TU_VERIFY(request->wLength == sizeof(audio_control_cur_4_t));

        sampFreq = (uint32_t)((audio_control_cur_4_t const *)buf)->bCur;

        TU_LOG2("Clock set current freq: %" PRIu32 "\r\n", sampFreq);

        return true;
    }
    else
    {
        TU_LOG1("Clock set request not supported, entity = %u, selector = %u, request = %u\r\n",
                request->bEntityID, request->bControlSelector, request->bRequest);
        return false;
    }
}

enum
{
    VOLUME_CTRL_0_DB = 0,
    VOLUME_CTRL_10_DB = 2560,
    VOLUME_CTRL_20_DB = 5120,
    VOLUME_CTRL_30_DB = 7680,
    VOLUME_CTRL_40_DB = 10240,
    VOLUME_CTRL_50_DB = 12800,
    VOLUME_CTRL_60_DB = 15360,
    VOLUME_CTRL_70_DB = 17920,
    VOLUME_CTRL_80_DB = 20480,
    VOLUME_CTRL_90_DB = 23040,
    VOLUME_CTRL_100_DB = 25600,
    VOLUME_CTRL_SILENCE = 0x8000,
};

// Helper for feature unit get requests
static bool tud_audio_feature_unit_get_request(uint8_t rhport, audio_control_request_t const *request)
{
    // //LOG_VERBOSE("tud_audio_feature_unit_get_request\r\n");

    // TU_ASSERT(request->bEntityID == ENT_NUM_mM_Sound_LineIn_feature);

    if (request->bControlSelector == AUDIO_FU_CTRL_MUTE && request->bRequest == AUDIO_CS_REQ_CUR)
    {
        audio_control_cur_1_t mute1 = {.bCur = mute[request->bChannelNumber]};
        TU_LOG2("Get channel %u mute %d\r\n", request->bChannelNumber, mute1.bCur);
        return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &mute1, sizeof(mute1));
    }
    else if (request->bControlSelector == AUDIO_FU_CTRL_VOLUME)
    {
        if (request->bRequest == AUDIO_CS_REQ_RANGE)
        {
            audio_control_range_2_n_t(1) range_vol;
            range_vol.wNumSubRanges = tu_htole16(1);
            range_vol.subrange[0] = {tu_htole16(-VOLUME_CTRL_50_DB), tu_htole16(VOLUME_CTRL_0_DB), tu_htole16(256)};
            TU_LOG2("Get channel %u volume range (%d, %d, %u) dB\r\n", request->bChannelNumber,
                    range_vol.subrange[0].bMin / 256, range_vol.subrange[0].bMax / 256, range_vol.subrange[0].bRes / 256);
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &range_vol, sizeof(range_vol));
        }
        else if (request->bRequest == AUDIO_CS_REQ_CUR)
        {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
            audio_control_cur_2_t cur_vol = {.bCur = tu_htole16(volume[request->bChannelNumber])};
            TU_LOG2("Get channel %u volume %d dB\r\n", request->bChannelNumber, cur_vol.bCur / 256);
#pragma GCC diagnostic pop
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_vol, sizeof(cur_vol));
        }
    }
    TU_LOG1("Feature unit get request not supported, entity = %u, selector = %u, request = %u\r\n",
            request->bEntityID, request->bControlSelector, request->bRequest);

    return false;
}

static bool tud_audio_terminal_unit_get_request(uint8_t rhport, audio_control_request_t const *request)
{
    // //LOG_VERBOSE("tud_audio_terminal_unit_get_request: entity = %u, selector = %u, request = %u\r\n",
    //         request->bEntityID, request->bControlSelector, request->bRequest);

    // TU_ASSERT(request->bEntityID == ENT_NUM_mM_Sound_LineIn_input || request->bEntityID == ENT_NUM_mM_Sound_LineIn_output);

    if (request->bControlSelector == AUDIO_TE_CTRL_COPY_PROTECT && request->bRequest == AUDIO_CS_REQ_CUR)
    {
        audio_control_cur_1_t cur_cp = {.bCur = 0};
        TU_LOG2("Get copy protect %u\r\n", cur_cp.bCur);
        return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_cp, sizeof(cur_cp));
    }
    else if (request->bControlSelector == AUDIO_TE_CTRL_CONNECTOR && request->bRequest == AUDIO_CS_REQ_CUR)
    {
        audio_desc_channel_cluster_t ret;

        // Those are dummy values for now
        ret.bNrChannels = 2;
        ret.bmChannelConfig = (audio_channel_config_t) 0x03;
        ret.iChannelNames = 0;

        TU_LOG2("    Get terminal connector\r\n");

        return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, (void*) &ret, sizeof(ret));
    }
    TU_LOG1("Terminal unit get request not supported, entity = %u, selector = %u, request = %u\r\n",
            request->bEntityID, request->bControlSelector, request->bRequest);

    return false;
}

// Helper for feature unit set requests
static bool tud_audio_feature_unit_set_request(uint8_t rhport, audio_control_request_t const *request, uint8_t const *buf)
{
    //LOG_VERBOSE("tud_audio_feature_unit_set_request\r\n");

    (void)rhport;

    // TU_ASSERT(request->bEntityID == ENT_NUM_mM_Sound_LineOut_feature);
    TU_VERIFY(request->bRequest == AUDIO_CS_REQ_CUR);

    if (request->bControlSelector == AUDIO_FU_CTRL_MUTE)
    {
        TU_VERIFY(request->wLength == sizeof(audio_control_cur_1_t));

        mute[request->bChannelNumber] = ((audio_control_cur_1_t const *)buf)->bCur;

        TU_LOG2("Set channel %d Mute: %d\r\n", request->bChannelNumber, mute[request->bChannelNumber]);

        return true;
    }
    else if (request->bControlSelector == AUDIO_FU_CTRL_VOLUME)
    {
        TU_VERIFY(request->wLength == sizeof(audio_control_cur_2_t));

        volume[request->bChannelNumber] = ((audio_control_cur_2_t const *)buf)->bCur;

        TU_LOG2("Set channel %d volume: %d dB\r\n", request->bChannelNumber, volume[request->bChannelNumber] / 256);

        return true;
    }
    else
    {
        TU_LOG1("Feature unit set request not supported, entity = %u, selector = %u, request = %u\r\n",
                request->bEntityID, request->bControlSelector, request->bRequest);
        return false;
    }
}

//--------------------------------------------------------------------+
// Application Callback API Implementations
//--------------------------------------------------------------------+

// Invoked when audio class specific get request received for an entity
bool tud_audio_get_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    audio_control_request_t const *request = (audio_control_request_t const *)p_request;

    if (request->bEntityID == ENT_NUM_mM_Sound_clock)
        return tud_audio_clock_get_request(rhport, request);
    if (request->bEntityID == ENT_NUM_mM_Sound_LineIn_feature || request->bEntityID == ENT_NUM_mM_Sound_LineOut_feature)    
        return tud_audio_feature_unit_get_request(rhport, request);
    if (request->bEntityID == ENT_NUM_mM_Sound_LineIn_input || request->bEntityID == ENT_NUM_mM_Sound_LineIn_output)
        return tud_audio_terminal_unit_get_request(rhport, request);
    if (request->bEntityID == ENT_NUM_mM_Sound_LineOut_input || request->bEntityID == ENT_NUM_mM_Sound_LineOut_output)
        return tud_audio_terminal_unit_get_request(rhport, request);

    TU_LOG1("Get request not handled, entity = %d, selector = %d, request = %d\r\n",
            request->bEntityID, request->bControlSelector, request->bRequest);
    return false;
}

// Invoked when audio class specific set request received for an entity
bool tud_audio_set_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request, uint8_t *buf)
{

    uint8_t channelNum = TU_U16_LOW(p_request->wValue);
    uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
    uint8_t itf = TU_U16_LOW(p_request->wIndex);
    uint8_t entityID = TU_U16_HIGH(p_request->wIndex);

    //LOG_VERBOSE("Set request for entity: %u, channel: %u, control: %u, interface: %u\r\n", entityID, channelNum, ctrlSel, itf);

    audio_control_request_t const *request = (audio_control_request_t const *)p_request;

    if (request->bEntityID == ENT_NUM_mM_Sound_LineIn_feature || request->bEntityID == ENT_NUM_mM_Sound_LineOut_feature)    
        return tud_audio_feature_unit_set_request(rhport, request, buf);
    if (request->bEntityID == ENT_NUM_mM_Sound_clock)
        return tud_audio_clock_set_request(rhport, request, buf);
    TU_LOG1("Set request not handled, entity = %d, selector = %d, request = %d\r\n",
            request->bEntityID, request->bControlSelector, request->bRequest);

    return false;
}