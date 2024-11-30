#include "mod_UART.h"
#include "msg_MIDI.h"
#include "hardware/uart.h"
#include "pico/stdio.h"
#include "stdio.h"
#include "App.h"
#include "pico/stdio/driver.h"
#include "lua.hpp"

lua_State *L;

uart_inst_t *uart_instance;

void writeChar(EmbeddedCli *cli, char c) {
    uart_putc(uart_instance, c);
}

void handleBootlogCommand(EmbeddedCli *cli, char *args, void *context) {
    if (strcmp(args, "print") == 0) {
        mMApp.sd.Mount();
        FIL *file = mMApp.sd.fopen("bootlog.txt", "r");
        if (file == nullptr) {
            printf("Error: cannot open bootlog.txt\n");
            return;
        }

        char buffer[256];
        while (mMApp.sd.fgets(buffer, sizeof(buffer), file) != nullptr) {
            printf("%s", buffer);
        }

        mMApp.sd.fclose(file);
        mMApp.sd.Unmount();
    } else if (strcmp(args, "delete") == 0) {
        if (mMApp.sd.remove("bootlog.txt") == 0) {
            printf("bootlog.txt deleted successfully\n");
        } else {
            printf("Error: cannot delete bootlog.txt\n");
        }
    } else {
        printf("Invalid argument for bootlog command\n");
    }
}

void handlePauseCommand(EmbeddedCli *cli, char *args, void *context) {
    mMApp.stdio.pause();
}

void handleResumeCommand(EmbeddedCli *cli, char *args, void *context) {
    mMApp.stdio.resume();
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
}

void mod_UART::out(const void *buffer, size_t count) {
    uart_stdio::out_chars((const char *)buffer, count);
}

struct UartPinMapping {
    int tx_pin;
    int rx_pin;
    uart_inst_t *uart_instance;
};

static const UartPinMapping uart_pin_map[] = {
    {0, 1, uart0},
    {4, 5, uart1},
    {8, 9, uart1},
    {12, 13, uart0},
    {16, 17, uart0},
    {20, 21, uart1},
    {24, 25, uart1},
    {28, 29, uart0}
};

uart_inst_t* get_uart_instance(int tx_pin, int rx_pin) {
    for (const auto& mapping : uart_pin_map) {
        if (mapping.tx_pin == tx_pin && mapping.rx_pin == rx_pin) {
            return mapping.uart_instance;
        }
    }
    return nullptr;
}

void mod_UART::Init() {
    // Initialize UART
    gpio_set_function(mMApp.hwConfig->gpio_uart_tx, GPIO_FUNC_UART);
    gpio_set_function(mMApp.hwConfig->gpio_uart_rx, GPIO_FUNC_UART);

    // Determine UART instance from pin numbers
    uart_instance = get_uart_instance(mMApp.hwConfig->gpio_uart_tx, mMApp.hwConfig->gpio_uart_rx);

    if (uart_instance == nullptr) {
        LOG_ERROR("Error: Invalid UART pin configuration\n");
        return;
    }

    uart_init(uart_instance, mMApp.hwConfig->uart_baudrate);

    // Initialize embedded CLI
    cli = embeddedCliNewDefault();
    cli->writeChar = writeChar;
    cli->onCommand = handleCliCommand;

    embeddedCliAddBinding(cli, {
        "bootlog",
        "bootlog print   -- Prints the bootlog from the SD card\n\
        bootlog delete  -- Deletes the bootlog from the SD card",
        false,
        nullptr,
        handleBootlogCommand
    });

    embeddedCliAddBinding(cli, {
        "pause",
        "pause  -- Pauses the stdio output",
        false,
        nullptr,
        handlePauseCommand
    });

    embeddedCliAddBinding(cli, {
        "resume",
        "resume  -- Resumes the stdio output",
        false,
        nullptr,
        handleResumeCommand
    });

    // register UART stdio callback
    mMApp.stdio.registerPrintCallback(uart_stdio::out_chars, mod_Stdio::verbose);
    printf("\n\n\nUART STDIO initialized\n");

    // Initialize Lua
    L = luaL_newstate();
    luaL_openlibs(L);

    printf("Lua initialized\n");

    // Load Lua script
    if (luaL_dofile(L, "script.lua") != LUA_OK) {
        LOG_WARNING("Warning: cannot load Lua script: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

void mod_UART::Tick() {
    while (uart_is_readable(uart_instance)) {
        uint8_t c = uart_getc(uart_instance);
        embeddedCliReceiveChar(cli, c);
    }
    embeddedCliProcess(cli);
}