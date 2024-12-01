#include "App.h"
#include "pico/stdio.h"
#include "stdio.h"
#include "pico/stdio/driver.h"
#include "mod_stdio.h"
#include <cstdarg>
#include <cstdio>

int in_chars(char *buf, int len)
{
    return mMApp.stdio.in_chars(buf, len);
}

void set_chars_available_callback(void (*fn)(void*), void *param)
{
    mMApp.stdio.set_chars_available_callback(fn, param);
}

void out_flush(void)
{
    mMApp.stdio.out_flush();
}

void out_chars(const char *buf, int len)
{
    mMApp.stdio.out_chars(buf, len);
}

stdio_driver_t mM_stdio_driver = {
    .out_chars = out_chars,
    .out_flush = out_flush,
    .in_chars = in_chars,
    .set_chars_available_callback = set_chars_available_callback,
    .next = nullptr,
};

void mod_Stdio::Init()
{
    stdio_set_driver_enabled(&mM_stdio_driver, true);
    recursive_mutex_init(&mutex);
    registerTarget(&watchdog);
}

void mod_Stdio::Tick()
{
}

void mod_Stdio::Test()
{
}

int mod_Stdio::in_chars(char *buf, int len)
{
    recursive_mutex_enter_blocking(&mutex);
    int l = in_buffer.length();
    if (l > len)
        l = len;
    in_buffer.copy(buf, l);
    in_buffer.erase(0, l);
    recursive_mutex_exit(&mutex);
    return l;
}

void mod_Stdio::out_flush()
{
}

void mod_Stdio::set_chars_available_callback(void (*fn)(void*), void *param)
{
    chars_available_callback = fn;
    chars_available_param = param;
}

void mod_Stdio::push(char c)
{
    recursive_mutex_enter_blocking(&mutex);
    in_buffer.push_back(c);
    recursive_mutex_exit(&mutex);

    if (chars_available_callback)
    {
        chars_available_callback(chars_available_param);
    }
}

void mod_Stdio::push(std::string_view str)
{
    recursive_mutex_enter_blocking(&mutex);
    in_buffer.append(str);
    recursive_mutex_exit(&mutex);

    if (chars_available_callback)
    {
        chars_available_callback(chars_available_param);
    }
}

void mod_Stdio::push(const char *str)
{
    recursive_mutex_enter_blocking(&mutex);
    in_buffer.append(str);
    recursive_mutex_exit(&mutex);

    if (chars_available_callback)
    {
        chars_available_callback(chars_available_param);
    }
}

void mod_Stdio::push(const char *str, int len)
{
    recursive_mutex_enter_blocking(&mutex);
    in_buffer.append(str, len);
    recursive_mutex_exit(&mutex);
    
    if (chars_available_callback)
    {
        chars_available_callback(chars_available_param);
    }
}

void stdio_watchdog::print_fn(LogFilter filter, const char *buf, int len)
{
    if (filter.channel == LogChannel::lc_none)
    {
        bytes_sent_on_none += len;
        if (pauseOnChannelNone)
            __breakpoint();
    }

    if (filter.level == LogLevel::ll_panic)
    {
        mMApp.stdio.panic();
        exit(1);
    }
    else if (filter.level == LogLevel::ll_error)
    {
        __breakpoint();
        if (exitOnError)
        {
            mMApp.stdio.panic();
            exit(1);
        }
    }
    else if (filter.level == LogLevel::ll_warning)
    {
        if (pauseOnWarning)
            __breakpoint();
    }
}

void buffering_target::print_fn(LogFilter filter, const char *buf, int len)
{
    buffer.append(buf, len);
    if (buffer.length() > max_size)
    {
        if (full_callback)
            full_callback(this);
        if (buffer.length() > max_size)
            buffer.erase(0, buffer.length() - max_size);
    }
}

void catchword_target::print_fn(LogFilter filter, const char *buffer, int len)
{
    int w = 0;
    const char *word;
    while (word = catchwords[w++])
    {
        const char *p = strstr(buffer, word);
        if (p)
        {
            if (catch_callback)
                catch_callback(this, p);
            else
                __breakpoint();
            return;
        }
    }
}

// extern "C" {

//     void my_printf(const char *fmt, ...) //printf from the spi sd library
//     {
//         va_list args;
//         va_start(args, fmt);
//         mMApp.stdio.vprint(LogLevel::info, fmt, args);
//         va_end(args);
//     }

//     void my_assert_func(const char *file, int line, const char *func,
//                         const char *pred) {
//         LOG_PANIC("assertion \"%s\" failed: file \"%s\", line %d, function: %s\n",
//             pred, file, line, func);
//     }

//     void __assert_func(const char *file, int line, const char *func, const char *failedexpr) {
//         my_assert_func(file, line, func, failedexpr);
//     }

// }