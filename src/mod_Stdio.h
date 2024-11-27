#pragma once

#include "Module.h"
#include <string>
#include <stdint.h>
#include <map>
#include "pico/mutex.h"

typedef void (*print_fn)(const char *buf, int len);
typedef void (*panic_fn)();

#define LOG_DEBUG(...) mMApp.stdio.log(mod_Stdio::LogLevel::debug, __VA_ARGS__)
#define LOG_INFO(...) mMApp.stdio.log(mod_Stdio::LogLevel::info, __VA_ARGS__)
#define LOG_WARNING(...) mMApp.stdio.log(mod_Stdio::LogLevel::warning, __VA_ARGS__)
#define LOG_ERROR(...) mMApp.stdio.log(mod_Stdio::LogLevel::error, __VA_ARGS__)
#define LOG_PANIC(...) mMApp.stdio.log(mod_Stdio::LogLevel::panic, __VA_ARGS__)

class mod_Stdio : public Module
{
public:
    enum LogLevel {
        panic,
        error,
        warning,
        info,
        debug,
        LOG_LEVEL_COUNT
    };

    void Init() override;
    void Tick() override;
    void Test() override;

    int in_chars(char *buf, int len);
    void out_chars(const char *buf, int len);
    void out_flush();
    void set_chars_available_callback(void (*fn)(void*), void *param);

    //the characters pushed will be available as input
    void push(char c);
    void push(std::string_view str);
    void push(const char *str);
    void push(const char *str, int len);
    
    void log(LogLevel level, const char *fmt, ...)
    {
        const LogLevel logLevel = getLogLevel();
        setLogLevel(level);
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
        setLogLevel(logLevel);
        switch(level)
        {
            case LogLevel::panic:
                for (auto &fn : panicCallbacks)
                    fn();
                __breakpoint();
                exit(1);
                break;
            case LogLevel::error:
                __breakpoint();
                break;
        }
    }

    inline LogLevel getLogLevel() {
        return this->level[get_core_num()];
    }

    inline void setLogLevel(LogLevel level) {
        this->level[get_core_num()] = level;
    }

    void registerPrintCallback(print_fn fn, LogLevel level);
    void unregisterPrintCallback(print_fn fn, LogLevel level);

    void registerPanicCallback(panic_fn fn);
    void unregisterPanicCallback(panic_fn fn);

private:
    LogLevel level[2] = {LogLevel::info, LogLevel::info}; //one per core
    std::string in_buffer;
    void (*chars_available_callback)(void*) = nullptr;
    void *chars_available_param = nullptr;
    mutex_t mutex;
    std::vector<print_fn> printCallbacks[LogLevel::LOG_LEVEL_COUNT];
    std::vector<panic_fn> panicCallbacks;
};