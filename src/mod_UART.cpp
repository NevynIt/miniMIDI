#include "App.h"
#include "hardware/uart.h"
#include "embedded_cli.h"

void mod_UART::writeChar_u0(EmbeddedCli *cli, char c) {
    uart_putc(uart0, c);
}

void mod_UART::writeChar_u1(EmbeddedCli *cli, char c) {
    uart_putc(uart1, c);
}

void mod_UART::onHelp(EmbeddedCli *cli, char *tokens, void *context) {
    mMApp.stdio.addLogChannel(LogChannel::lc_uart);
    printf("Welcome to miniMIDI Serial interface\n"
           "  Use the Lua language to interact with the system\n"
           "  System functions are under the global table mM\n"
           "  ?expr is a shorthand for print(expr)\n"
           );
    mMApp.stdio.removeLogChannel(LogChannel::lc_uart);
}

void mod_UART::handleCliCommand(EmbeddedCli *cli, CliCommand *command)
{
    std::string commandStr = command->name;
    if (command->args != nullptr)
        commandStr += " ";
        commandStr += command->args;

    if (commandStr[0] == '?')
    {
        if (commandStr.size() == 1)
        {
            mod_UART::onHelp(cli, nullptr, nullptr);
            return;
        }
        else
            commandStr = "print(" + commandStr.substr(1) + ")";
    }

    mMApp.stdio.addLogChannel(LogChannel::lc_uart);
    mMApp.lua.doString(commandStr.c_str());
    mMApp.stdio.removeLogChannel(LogChannel::lc_uart);
}

void mod_UART::print_fn(LogFilter filter, const char *buf, int len)
{
    for (int i = 0; i < len; i++)
    {
        if (buf[i] == '\n' || linebuffer_pos == sizeof(linebuffer) - 1)
        {
            linebuffer[linebuffer_pos] = 0;
            embeddedCliPrint(cli, linebuffer);
            linebuffer_pos = 0;
        }
        else
            linebuffer[linebuffer_pos++] = buf[i];
    }
}

void mod_UART::clear_terminal() {
    embeddedCliPrint(cli, "\x1B[2J\x1B[H");
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
    auto c = embeddedCliDefaultConfig();
    c->enableAutoComplete = false;
    c->cmdBufferSize = 256;
    c->historyBufferSize = 1024;
    cli = embeddedCliNew(c);
    if (uart_instance == uart0)
        cli->writeChar = mod_UART::writeChar_u0;
    else
        cli->writeChar = mod_UART::writeChar_u1;
    cli->onCommand = mod_UART::handleCliCommand;

    embeddedCliAddBinding(cli, {
            "help",
            "Print a help message",
            false,
            nullptr,
            mod_UART::onHelp
    });

    // register UART stdio callback
    mMApp.stdio.registerTarget(this);
    log_mask.channel = LogChannel::lc_uart;

    printf("UART STDIO initialized\n");

    clear_terminal();
    onHelp(cli, nullptr, nullptr);
}

void mod_UART::Tick() {
    while (uart_is_readable(uart_instance)) {
        uint8_t c = uart_getc(uart_instance);
        embeddedCliReceiveChar(cli, c);
    }
    embeddedCliProcess(cli);
}