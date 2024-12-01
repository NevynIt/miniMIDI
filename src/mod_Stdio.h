#pragma once

#include "Module.h"
#include <string>
#include <stdint.h>
#include <map>
#include "pico/mutex.h"
#include <cstdarg> 
#include <vector>
#include <cstring>

#define LOG_VERBOSE(...)  mMApp.stdio.print(LogLevel::ll_verbose, __VA_ARGS__)
#define LOG_DEBUG(...)    mMApp.stdio.print(LogLevel::ll_debug, __VA_ARGS__)
#define LOG_INFO(...)     mMApp.stdio.print(LogLevel::ll_info, __VA_ARGS__)
#define LOG_WARNING(...)  mMApp.stdio.print(LogLevel::ll_warning, __VA_ARGS__)
#define LOG_ERROR(...)    mMApp.stdio.print(LogLevel::ll_error, __VA_ARGS__)
#define LOG_PANIC(...)    mMApp.stdio.print(LogLevel::ll_panic, __VA_ARGS__)

enum LogLevel : uint8_t {
    ll_panic = 0,
    ll_error,
    ll_warning,
    ll_standard, //normal printf level
    ll_info,    //more verbose, but still not debug level
    ll_debug,
    ll_verbose, //as much as possible
    ll_all = 0xFF,
};

enum LogChannel : uint8_t
{
    lc_none = 0, //if something is sent to channel 0 only, it will be caught only by a filter with channel 0 as well 
    lc_boot = 1,
    lc_display = 2,
    lc_uart = 4,
    lc_cdc = 8,
    lc_all = 0xFF,
};

struct LogFilter{
    LogChannel channel = LogChannel::lc_none;
    LogLevel level = LogLevel::ll_standard;
};

class LogTarget
{
public:
    virtual void print_fn(LogFilter filter, const char *buf, int len) { }
    virtual void panic_fn() { }

    virtual void filter_and_print(LogFilter filter, const char *buf, int len)
    {
        if ((filter.channel == log_mask.channel || (filter.channel & log_mask.channel)) && filter.level <= log_mask.level)
            print_fn(filter, buf, len);
    }

    LogFilter log_mask = {LogChannel::lc_all, LogLevel::ll_all};
};

class stdio_watchdog : public LogTarget
{
public:
    void filter_and_print(LogFilter filter, const char *buf, int len) override
    {   //no filtering
        print_fn(filter, buf, len);
    }
    void print_fn(LogFilter filter, const char *buf, int len) override;
    
    bool exitOnError = false;
    bool pauseOnWarning = false;
    bool pauseOnChannelNone = false;
    uint32_t bytes_sent_on_none = 0;
};

class mod_Stdio : public Module
{
public:
    void Init() override;
    void Tick() override;
    void Test() override;

    int in_chars(char *buf, int len);
    void out_flush();
    void set_chars_available_callback(void (*fn)(void*), void *param);

    //the characters pushed will be available as input
    void push(char c);
    void push(std::string_view str);
    void push(const char *str);
    void push(const char *str, int len);

    bool format_string_expects_parameters(const char *format) {
        bool in_format = false;
        for (const char *p = format; *p; p++) {
            const char ch =*p;
            if (in_format) {
                // Check for valid format specifiers
                if (ch == 'd' || ch == 'i' || ch == 'u' || ch == 'f' || ch == 'F' ||
                    ch == 'e' || ch == 'E' || ch == 'g' || ch == 'G' || ch == 'x' ||
                    ch == 'X' || ch == 'o' || ch == 's' || ch == 'c' || ch == 'p' ||
                    ch == 'a' || ch == 'A' || ch == 'n') {
                    return true;
                }
                // Reset if not a valid specifier
                in_format = false;
            } else if (ch == '%') {
                in_format = true;
            }
        }
        return false;
    }

    void vprint(LogChannel channel, LogLevel level, const char *fmt, va_list args)
    {
        if (format_string_expects_parameters(fmt))
        {
            va_list args_copy;
            va_copy(args_copy, args);
            int len = vsnprintf(nullptr, 0, fmt, args);
            char *buf = new char[len+1];
            vsnprintf(buf, len+1, fmt, args_copy);
            out_chars(channel, level, buf, len);
            delete[] buf;
            va_end(args_copy);
        }
        else
        {
            out_chars(channel, level, fmt, strlen(fmt));
        }
    }

    void vprint(LogLevel level, const char *fmt, va_list args)
    {
        vprint(filter[get_core_num()].channel, level, fmt, args);
    }

    void vprint(const char *fmt, va_list args)
    {
        vprint(filter[get_core_num()].channel, filter[get_core_num()].level, fmt, args);
    }

    void print(LogChannel channel, LogLevel level, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        vprint(channel, level, fmt, args);
        va_end(args);
    }
    void print(LogLevel level, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        vprint(filter[get_core_num()].channel, level, fmt, args);
        va_end(args);
    }
    void print(const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        vprint(filter[get_core_num()].channel, filter[get_core_num()].level, fmt, args);
        va_end(args);
    }

    void out_chars(const char *buf, int len)
    {
        out_chars(filter[get_core_num()].channel, filter[get_core_num()].level, buf, len);
    }
    void out_chars(LogChannel channel, LogLevel level, const char *buf, int len)
    {
        recursive_mutex_enter_blocking(&mutex);
        for (auto tgt : targets)
            tgt->filter_and_print({channel, level}, buf, len);
        recursive_mutex_exit(&mutex);
    }

    void panic()
    {
        recursive_mutex_enter_blocking(&mutex);
        for (auto tgt : targets)
            tgt->panic_fn();
        recursive_mutex_exit(&mutex);
    }

    inline LogFilter getLogFilter() {
        return this->filter[get_core_num()];
    }

    inline LogLevel getLogLevel() {
        return this->filter[get_core_num()].level;
    }

    inline LogChannel getLogChannel() {
        return this->filter[get_core_num()].channel;
    }

    inline LogLevel setLogLevel(LogLevel level) {
        auto tmp = this->filter[get_core_num()].level;
        this->filter[get_core_num()].level = level;
        return tmp;
    }

    inline LogChannel setLogChannel(LogChannel channel) {
        auto tmp = this->filter[get_core_num()].channel;
        this->filter[get_core_num()].channel = channel;
        return tmp;
    }

    inline void addLogChannel(LogChannel channel) {
        this->filter[get_core_num()].channel = (LogChannel)(this->filter[get_core_num()].channel | channel);
    }

    inline void removeLogChannel(LogChannel channel) {
        this->filter[get_core_num()].channel = (LogChannel)(this->filter[get_core_num()].channel & ~channel);
    }

    inline LogFilter setLogFilter(LogFilter filter) {
        auto tmp = this->filter[get_core_num()];
        this->filter[get_core_num()] = filter;
        return tmp;
    }

    void registerTarget(LogTarget *tgt)
    {
        recursive_mutex_enter_blocking(&mutex);
        //check if it's already registered
        for (auto t : targets)
        {
            if (t == tgt)
            {
                recursive_mutex_exit(&mutex);
                return;
            }
        }
        targets.push_back(tgt);
        recursive_mutex_exit(&mutex);
    }

    void unregisterTarget(LogTarget *tgt)
    {
        recursive_mutex_enter_blocking(&mutex);
        for (auto it = targets.begin(); it != targets.end(); it++)
        {
            if ((*it) == tgt)
            {
                targets.erase(it);
                break;
            }
        }
        recursive_mutex_exit(&mutex);
    }

public:
    LogFilter filter[2]; //one per core
    std::string in_buffer;
    void (*chars_available_callback)(void*) = nullptr;
    void *chars_available_param = nullptr;
    recursive_mutex_t mutex;
    std::vector<LogTarget *> targets;
    stdio_watchdog watchdog;
};

class buffering_target : public LogTarget
{
public:
    buffering_target(uint16_t max_size = 2048) : max_size(max_size) { }
    void print_fn(LogFilter filter, const char *buf, int len) override;

    std::string buffer;
    uint16_t max_size;
    void (*full_callback)(buffering_target*) = nullptr;
};

class catchword_target : public LogTarget
{
public:
    catchword_target(const char **catchwords) : catchwords(catchwords) { }
    void print_fn(LogFilter filter, const char *buf, int len) override;

    const char **catchwords;
    void (*catch_callback)(catchword_target*, const char *p) = nullptr;
};