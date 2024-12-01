#ifndef mod_UART_H
#define mod_UART_H

#include "Module.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hwConfig.h"
#include "mod_Stdio.h"
extern "C" {
#include "embedded_cli.h"
}

//Wait for either midi messages or a \n\n\n escape sequence, which would trigger interaction with a embedded-cli

class mod_UART : public Module, public LogTarget {
public:
    void Init() override;
    void Tick() override;
    
    void print_fn(LogFilter filter, const char *buf, int len) override;
    static void handleCliCommand(EmbeddedCli *cli, CliCommand *command);
    static void onHelp(EmbeddedCli *cli, char *tokens, void *context);
    static void writeChar_u0(EmbeddedCli *cli, char c);
    static void writeChar_u1(EmbeddedCli *cli, char c);

    void clear_terminal();
public:
    char linebuffer[1024];
    int linebuffer_pos = 0;
    EmbeddedCli *cli;
    uart_inst_t *uart_instance;
};

#endif // mod_UART_H