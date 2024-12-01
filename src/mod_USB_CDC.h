#ifndef MOD_USB_UART_H
#define MOD_USB_UART_H

#include "Module.h"
#include "tusb.h"
#include "mod_Stdio.h"
extern "C" {
#include "embedded_cli.h"
}

class mod_USB_CDC : public Module, public LogTarget {
public:
    void Init() override;
    void Tick() override;
    
    void print_fn(LogFilter filter, const char *buf, int len) override;

    static void handleCliCommand(EmbeddedCli *cli, CliCommand *command);
    static void onHelp(EmbeddedCli *cli, char *tokens, void *context);
    static void writeChar(EmbeddedCli *cli, char c);

    void clear_terminal();

    static constexpr auto helpMsg = "Welcome to miniMIDI USB interface\n"
                                    "  Use the Lua language to interact with the system\n"
                                    "  System functions are under the global table mM\n"
                                    "  ?expr is a shorthand for print(expr)\n";

public:
    char linebuffer[1024];
    int linebuffer_pos = 0;
    static char usb_buffer[1024];
    static int usb_buffer_head;
    static int usb_buffer_tail;
    EmbeddedCli *cli;
};

#endif // MOD_USB_UART_H