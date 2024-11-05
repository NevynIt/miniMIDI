#include "mod_USB_Audio.h"
#include "App.h"
#include "stdio.h"
#include "midi_frequencies.h"
#include "WT_BASE.h"

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

void mod_USB_Audio::Test()
{
    static fp_int phaseL(0);
    static fp_int phaseR(1000);
    static sample_ptr last_left_buffer = nullptr;

    auto left = getOutBuffer(0);

    if (last_left_buffer != left)
    {
        // int noteL = app.encoders.count[0]/4 + 60;
        // fp_int ampL = fp_int(app.encoders.count[1]/4 + 50) / 100;
        // int noteR = app.encoders.count[2]/4 + 60;
        // fp_int ampR = fp_int(app.encoders.count[3]/4 + 50) / 100;

        int noteL = 57;
        fp_int ampL = fp_int(0.75f);
        int noteR = 69;
        fp_int ampR = ampL;

        if (noteL < 0) noteL = 0;
        if (noteL > 127) noteL = 127;
        if (noteR < 0) noteR = 0;
        if (noteR > 127) noteR = 127;
        if (ampL < 0) ampL = 0;
        if (ampL > 1) ampL = 1;
        if (ampR < 0) ampR = 0;
        if (ampR > 1) ampR = 1;

        auto right = getOutBuffer(1);
        for (int i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
        {
            left[i] = (sample_t)phaseL;
            right[i] = (sample_t)phaseR;
        }
        phaseL = phaseL + 1;
        phaseR = phaseR + 1;

        if (phaseL > SAMPLE_MAX)
            phaseL = -SAMPLE_MAX;
        if (phaseR > SAMPLE_MAX)
            phaseR = -SAMPLE_MAX;

        // phaseL = app.dsp.GenerateSquareWave(left, midi_frequencies[noteL], ampL, phaseL);
        // phaseR = app.dsp.GenerateSquareWave(right, midi_frequencies[noteR], ampR, phaseR);
        // phaseL = app.dsp.GenerateSineWave(left, midi_frequencies[noteL], ampL, phaseL);
        // phaseR = app.dsp.GenerateSineWave(right, midi_frequencies[noteR], ampR, phaseR);
        // phaseL = app.dsp.GenerateWave(left, WT_BASE, 0, midi_frequencies[noteL], ampL, phaseL);
        // phaseR = app.dsp.GenerateWave(right, WT_BASE, 0, midi_frequencies[noteR], ampR, phaseR);

        last_left_buffer = left;
    }
}

inline sample_ptr mod_USB_Audio::getOutBuffer(uint8_t channel) const
{
    auto slot = app.dsp.getSlotRelative(2, app.usb.SOF_sync_us);
    return app.dsp.buffers[track_out[channel]][slot];
}

inline sample_ptr mod_USB_Audio::getInBuffer(uint8_t channel) const
{
    auto slot = app.dsp.getSlotRelative(-1, app.usb.SOF_sync_us);
    return app.dsp.buffers[track_in[channel]][slot];
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

    printf("Set request for EP: %u, channel: %u, control: %u\r\n", ep, channelNum, ctrlSel);

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

    printf("Set request for interface: %u, channel: %u, control: %u\r\n", itf, channelNum, ctrlSel);

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

    printf("Get request for EP: %u, channel: %u, control: %u\r\n", ep, channelNum, ctrlSel);
    //	return tud_control_xfer(rhport, p_request, &tmp, 1);

    return false; // Yet not implemented
}

// Invoked when audio set interface request received
bool tud_audio_set_itf_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    (void)rhport;
    uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
    uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

    printf("Audio set interface: itf=%d, alt=%d\r\n", itf, alt);

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

    printf("Get request for interface: %u, channel: %u, control: %u\r\n", itf, channelNum, ctrlSel);
    return false; // Yet not implemented
}

bool tud_audio_tx_done_pre_load_cb(uint8_t rhport, uint8_t itf, uint8_t ep_in, uint8_t cur_alt_setting)
{
    (void)rhport;
    (void)itf;
    (void)ep_in;
    (void)cur_alt_setting;

    // printf("tud_audio_tx_done_pre_load_cb\r\n");

    auto buf = app.usbAudio.buffer_out;
    sample_ptr left = app.usbAudio.getOutBuffer(0);
    sample_ptr right = app.usbAudio.getOutBuffer(1);
    for (size_t cnt = 0; cnt < AUDIO_BUFFER_SAMPLES * 2;)
    {
        buf[cnt++] = *left++;
        buf[cnt++] = *right++;
    }

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

    printf("tud_audio_set_itf_close_EP_cb\r\n");

    return true;
}

bool tud_audio_rx_done_pre_read_cb(uint8_t rhport, uint16_t n_bytes_received, uint8_t func_id, uint8_t ep_out, uint8_t cur_alt_setting)
{
    (void)rhport;
    (void)n_bytes_received;
    (void)func_id;
    (void)ep_out;
    (void)cur_alt_setting;

    printf("tud_audio_rx_done_pre_read_cb\r\n");

    auto buf = app.usbAudio.buffer_in;
    uint16_t num_read = tud_audio_read(buf, 2 * AUDIO_BUFFER_SAMPLES * BITS_PER_SAMPLE / 8);

    sample_ptr left = app.usbAudio.getInBuffer(0);
    sample_ptr right = app.usbAudio.getInBuffer(1);
    for (size_t cnt = 0; cnt < AUDIO_BUFFER_SAMPLES * 2;)
    {
        *left++ = buf[cnt++];
        *right++ = buf[cnt++];
    }

    return true;
}

bool tud_audio_rx_done_post_read_cb(uint8_t rhport, uint16_t n_bytes_received, uint8_t func_id, uint8_t ep_out, uint8_t cur_alt_setting)
{
    (void)rhport;
    (void)n_bytes_received;
    (void)func_id;
    (void)ep_out;
    (void)cur_alt_setting;

    printf("tud_audio_rx_done_post_read_cb\r\n");

    return true;
}

//////////////////////////SECOND IMPLEMENTATION//////////////////////////

// Helper for clock get requests
static bool tud_audio_clock_get_request(uint8_t rhport, audio_control_request_t const *request)
{
    // printf("tud_audio_clock_get_request\r\n");

    TU_ASSERT(request->bEntityID == ENT_NUM_mM_Sound_clock);

    if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ)
    {
        if (request->bRequest == AUDIO_CS_REQ_CUR)
        {
            TU_LOG1("Clock get current freq %" PRIu32 "\r\n", sampFreq);

            audio_control_cur_4_t curf = {(int32_t)tu_htole32(sampFreq)};
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &curf, sizeof(curf));
        }
        else if (request->bRequest == AUDIO_CS_REQ_RANGE)
        {
            audio_control_range_4_n_t(1) rangef =
                {
                    .wNumSubRanges = tu_htole16(1)};
            TU_LOG1("Clock get %d freq ranges\r\n", 1);
            for (uint8_t i = 0; i < 1; i++)
            {
                rangef.subrange[i].bMin = (int32_t)SAMPLE_RATE;
                rangef.subrange[i].bMax = (int32_t)SAMPLE_RATE;
                rangef.subrange[i].bRes = 0;
                TU_LOG1("Range %d (%d, %d, %d)\r\n", i, (int)rangef.subrange[i].bMin, (int)rangef.subrange[i].bMax, (int)rangef.subrange[i].bRes);
            }

            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &rangef, sizeof(rangef));
        }
    }
    else if (request->bControlSelector == AUDIO_CS_CTRL_CLK_VALID &&
             request->bRequest == AUDIO_CS_REQ_CUR)
    {
        audio_control_cur_1_t cur_valid = {.bCur = 1};
        TU_LOG1("Clock get is valid %u\r\n", cur_valid.bCur);
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

    printf("tud_audio_clock_set_request\r\n");

    TU_ASSERT(request->bEntityID == ENT_NUM_mM_Sound_clock);
    TU_VERIFY(request->bRequest == AUDIO_CS_REQ_CUR);

    if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ)
    {
        TU_VERIFY(request->wLength == sizeof(audio_control_cur_4_t));

        sampFreq = (uint32_t)((audio_control_cur_4_t const *)buf)->bCur;

        TU_LOG1("Clock set current freq: %" PRIu32 "\r\n", sampFreq);

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
    // printf("tud_audio_feature_unit_get_request\r\n");

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
    // printf("tud_audio_terminal_unit_get_request: entity = %u, selector = %u, request = %u\r\n",
    //         request->bEntityID, request->bControlSelector, request->bRequest);

    // TU_ASSERT(request->bEntityID == ENT_NUM_mM_Sound_LineIn_input || request->bEntityID == ENT_NUM_mM_Sound_LineIn_output);

    if (request->bControlSelector == AUDIO_TE_CTRL_COPY_PROTECT && request->bRequest == AUDIO_CS_REQ_CUR)
    {
        audio_control_cur_1_t cur_cp = {.bCur = 0};
        TU_LOG1("Get copy protect %u\r\n", cur_cp.bCur);
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
    printf("tud_audio_feature_unit_set_request\r\n");

    (void)rhport;

    // TU_ASSERT(request->bEntityID == ENT_NUM_mM_Sound_LineOut_feature);
    TU_VERIFY(request->bRequest == AUDIO_CS_REQ_CUR);

    if (request->bControlSelector == AUDIO_FU_CTRL_MUTE)
    {
        TU_VERIFY(request->wLength == sizeof(audio_control_cur_1_t));

        mute[request->bChannelNumber] = ((audio_control_cur_1_t const *)buf)->bCur;

        TU_LOG1("Set channel %d Mute: %d\r\n", request->bChannelNumber, mute[request->bChannelNumber]);

        return true;
    }
    else if (request->bControlSelector == AUDIO_FU_CTRL_VOLUME)
    {
        TU_VERIFY(request->wLength == sizeof(audio_control_cur_2_t));

        volume[request->bChannelNumber] = ((audio_control_cur_2_t const *)buf)->bCur;

        TU_LOG1("Set channel %d volume: %d dB\r\n", request->bChannelNumber, volume[request->bChannelNumber] / 256);

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

    printf("Set request for entity: %u, channel: %u, control: %u, interface: %u\r\n", entityID, channelNum, ctrlSel, itf);

    audio_control_request_t const *request = (audio_control_request_t const *)p_request;

    if (request->bEntityID == ENT_NUM_mM_Sound_LineIn_feature || request->bEntityID == ENT_NUM_mM_Sound_LineOut_feature)    
        return tud_audio_feature_unit_set_request(rhport, request, buf);
    if (request->bEntityID == ENT_NUM_mM_Sound_clock)
        return tud_audio_clock_set_request(rhport, request, buf);
    TU_LOG1("Set request not handled, entity = %d, selector = %d, request = %d\r\n",
            request->bEntityID, request->bControlSelector, request->bRequest);

    return false;
}

// bool tud_audio_set_itf_cb(uint8_t rhport, tusb_control_request_t const *p_request)
// {
//     (void)rhport;
//     uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
//     uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

//     TU_LOG2("Set interface %d alt %d\r\n", itf, alt);
//     if (ITF_NUM_AUDIO_STREAMING_SPK == itf && alt != 0)
//     {
//         // Audio streaming start
//         blink_interval_ms = BLINK_STREAMING;
//     }

//     // Clear buffer when streaming format is changed
//     spk_data_size = 0;
//     if (alt != 0)
//     {
//         current_resolution = resolutions_per_format[alt - 1];
//     }

//     return true;
// }

// bool tud_audio_rx_done_pre_read_cb(uint8_t rhport, uint16_t n_bytes_received, uint8_t func_id, uint8_t ep_out, uint8_t cur_alt_setting)
// {
//     (void)rhport;
//     (void)func_id;
//     (void)ep_out;
//     (void)cur_alt_setting;

//     spk_data_size = tud_audio_read(spk_buf, n_bytes_received);
//     tud_audio_write(spk_buf, n_bytes_received);

//     return true;
// }

// bool tud_audio_tx_done_pre_load_cb(uint8_t rhport, uint8_t itf, uint8_t ep_in, uint8_t cur_alt_setting)
// {
//     (void)rhport;
//     (void)itf;
//     (void)ep_in;
//     (void)cur_alt_setting;

//     // This callback could be used to fill microphone data separately
//     return true;
// }
