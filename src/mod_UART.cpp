#include "mod_UART.h"
#include "msg_MIDI.h"
#include "hardware/uart.h"
#include "pico/stdio.h"
#include "stdio.h"
#include "App.h"
#include "pico/stdio/driver.h"
#include "lua.hpp"

extern "C" {
    void stdio_uart_init_full(struct uart_inst *uart, uint baud_rate, int tx_pin, int rx_pin);
}

lua_State *L;

void writeChar(EmbeddedCli *cli, char c) {
    uart_putc(UART_INST, c);
}

void handleCliCommand(EmbeddedCli *cli, CliCommand *command) {
    // lua_getglobal(L, "handleCommand");
    // merge command name and args
    if (command->args[0] != 0)
    {
        std::string commandStr = command->name;
        commandStr += " ";
        commandStr += command->args;
	    luaL_loadstring(L, commandStr.c_str());
    }
    else
    {
        luaL_loadstring(L, command->name);
    }

    if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK) {
        printf("Lua error: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

namespace uart_stdio
{
    char linebuffer[1024];
    int linebuffer_pos = 0;

    void out_flush(void)
    {
    }

    void out_chars(const char *buf, int len)
    {
        for (int i = 0; i < len; i++)
        {
            if (buf[i] == '\n' || linebuffer_pos == sizeof(linebuffer) - 1)
            {
                linebuffer[linebuffer_pos] = 0;
                embeddedCliPrint(mMApp.uart.cli, linebuffer);
                linebuffer_pos = 0;
            }
            else
                linebuffer[linebuffer_pos++] = buf[i];
        }
    }

    stdio_driver_t mM_uart = {
        .out_chars = out_chars,
        .out_flush = out_flush,
        .in_chars = nullptr,
        .set_chars_available_callback = nullptr,
        .next = nullptr,
    };
}

void mod_UART::out(const void *buffer, size_t count) {
    uart_stdio::out_chars((const char *)buffer, count);
}

void mod_UART::Init() {
    // Initialize UART
    gpio_set_function(GPIO_UART_TX, GPIO_FUNC_UART);
    gpio_set_function(GPIO_UART_RX, GPIO_FUNC_UART);
    uart_init(UART_INST, UART_BAUD_RATE);

    // Initialize embedded CLI
    cli = embeddedCliNewDefault();
    cli->writeChar = writeChar;
    cli->onCommand = handleCliCommand;

    // register UART stdio driver
    stdio_set_driver_enabled(&uart_stdio::mM_uart, true);
    printf("\n\n\nUART STDIO initialized\n");

    // Initialize Lua
    L = luaL_newstate();
    luaL_openlibs(L);

    printf("Lua initialized\n");

    // Load Lua script
    if (luaL_dofile(L, "script.lua") != LUA_OK) {
        printf("Error loading Lua script: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }

}

void mod_UART::Tick() {
    while (uart_is_readable(UART_INST)) {
        uint8_t c = uart_getc(UART_INST);
        embeddedCliReceiveChar(cli, c);
    }
    embeddedCliProcess(cli);
//     lua_getglobal(L, "tick");
//     if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
//         printf("Error running Lua tick: %s\n", lua_tostring(L, -1));
//         lua_pop(L, 1);
//     }
}