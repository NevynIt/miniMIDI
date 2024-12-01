#include "App.h"
#include "tusb.h"
#include "embedded_cli.h"

char mod_USB_CDC::usb_buffer[1024];
int mod_USB_CDC::usb_buffer_head = 0;
int mod_USB_CDC::usb_buffer_tail = 0;

void mod_USB_CDC::writeChar(EmbeddedCli *cli, char c) {
    int next_head = (usb_buffer_head + 1) % sizeof(usb_buffer);
    if (next_head == usb_buffer_tail)
        return; // drop the overflow
    usb_buffer[usb_buffer_head] = c;
    usb_buffer_head = next_head;
}

void mod_USB_CDC::onHelp(EmbeddedCli *cli, char *tokens, void *context) {
    mMApp.stdio.addLogChannel(LogChannel::lc_cdc);
    printf(helpMsg);
    mMApp.stdio.removeLogChannel(LogChannel::lc_cdc);
}

void mod_USB_CDC::handleCliCommand(EmbeddedCli *cli, CliCommand *command) {
    std::string commandStr = command->name;
    if (command->args != nullptr)
        commandStr += " ";
        commandStr += command->args;

    if (commandStr[0] == '?')
    {
        if (commandStr.size() == 1)
        {
            onHelp(cli, nullptr, nullptr);
            return;
        }
        else
            commandStr = "print(" + commandStr.substr(1) + ")";
    }

    mMApp.stdio.addLogChannel(LogChannel::lc_cdc);
    mMApp.lua.doString(commandStr.c_str());
    mMApp.stdio.removeLogChannel(LogChannel::lc_cdc);
}

void mod_USB_CDC::print_fn(LogFilter filter, const char *buf, int len) {
    for (int i = 0; i < len; i++) {
        if (buf[i] == '\n' || linebuffer_pos == sizeof(linebuffer) - 1) {
            linebuffer[linebuffer_pos] = 0;
            embeddedCliPrint(cli, linebuffer);
            linebuffer_pos = 0;
        } else {
            linebuffer[linebuffer_pos++] = buf[i];
        }
    }
}

void mod_USB_CDC::clear_terminal() {
    embeddedCliPrint(cli, "\x1B[2J\x1B[H");
}

void mod_USB_CDC::Init() {
    // Initialize embedded CLI
    auto c = embeddedCliDefaultConfig();
    c->enableAutoComplete = false;
    cli = embeddedCliNew(c);
    cli->writeChar = mod_USB_CDC::writeChar;
    cli->onCommand = mod_USB_CDC::handleCliCommand;

    embeddedCliAddBinding(cli, {
            "help",
            "Print a help message",
            false,
            nullptr,
            mod_USB_CDC::onHelp
    });

    // register USB CDC stdio callback
    mMApp.stdio.registerTarget(this);
    log_mask.channel = LogChannel::lc_cdc;

    printf("USB CDC STDIO initialized\n");

    clear_terminal();
    onHelp(cli, nullptr, nullptr);
}

void mod_USB_CDC::Tick() {
    embeddedCliProcess(cli);
    int count = 0;
    if (usb_buffer_tail > usb_buffer_head)
    {
        count = tud_cdc_write(usb_buffer + usb_buffer_tail, sizeof(usb_buffer) - usb_buffer_tail);
        usb_buffer_tail += count;
        if (usb_buffer_tail == sizeof(usb_buffer))
            usb_buffer_tail = 0;
    }
    if (count < 64 && usb_buffer_tail < usb_buffer_head)
    {
        usb_buffer_tail += tud_cdc_write(usb_buffer + usb_buffer_tail, usb_buffer_head - usb_buffer_tail);
    }
    tud_cdc_write_flush();
}

void tud_cdc_rx_cb(uint8_t itf)
{
    while (tud_cdc_available())
        embeddedCliReceiveChar(mMApp.usbCdc.cli, tud_cdc_read_char());
}

// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
  (void)itf;

  // connected
  if (dtr && rts) {
    // print initial message when connected
    mMApp.stdio.addLogChannel(LogChannel::lc_cdc);
    printf("\x1B[2J\x1B[H");
    printf(mMApp.usbCdc.helpMsg);
    mMApp.stdio.removeLogChannel(LogChannel::lc_cdc);
  }
}