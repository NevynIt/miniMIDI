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

void mod_Stdio::pause()
{
    recursive_mutex_enter_blocking(&mutex);
    paused = true;
    recursive_mutex_exit(&mutex);
}

void mod_Stdio::resume()
{
    recursive_mutex_enter_blocking(&mutex);
    paused = false;
    for (auto &p : logBuffer)
    {
        for (int l = p.second; l < LogLevel::LOG_LEVEL_COUNT; l++)
        {
            for (auto &fn : printCallbacks[l])
            {
                fn(p.first.c_str(), p.first.length());
            }
        }
    }
    logBuffer.clear();
    recursive_mutex_exit(&mutex);
}

void mod_Stdio::out_chars(const char *buf, int len)
{
    if (paused)
    {
        recursive_mutex_enter_blocking(&mutex);
        logBuffer.push_back(std::pair<std::string, LogLevel>(std::string(buf, len), level[get_core_num()]));
        recursive_mutex_exit(&mutex);
        return;
    }
    
    recursive_mutex_enter_blocking(&mutex);
    for (int l = level[get_core_num()]; l < LogLevel::LOG_LEVEL_COUNT; l++)
    {
        for (auto &fn : printCallbacks[l])
        {
            fn(buf, len);
        }
    }
    recursive_mutex_exit(&mutex);
}

void mod_Stdio::out_flush()
{
}

void mod_Stdio::set_chars_available_callback(void (*fn)(void*), void *param)
{
    chars_available_callback = fn;
    chars_available_param = param;
}

void mod_Stdio::registerPrintCallback(print_fn fn, LogLevel level)
{
    recursive_mutex_enter_blocking(&mutex);
    unregisterPrintCallback(fn);
    printCallbacks[level].push_back(fn);
    recursive_mutex_exit(&mutex);
}

void mod_Stdio::unregisterPrintCallback(print_fn fn)
{
    recursive_mutex_enter_blocking(&mutex);
    for (int l= 0; l< LogLevel::LOG_LEVEL_COUNT; l++)
    {
        auto &callbacks = printCallbacks[l];
        for (auto it = callbacks.begin(); it != callbacks.end(); it++)
        {
            if (*it == fn)
            {
                callbacks.erase(it);
                break;
            }
        }
    }
    recursive_mutex_exit(&mutex);
}

void mod_Stdio::registerPanicCallback(panic_fn fn)
{
    recursive_mutex_enter_blocking(&mutex);
    panicCallbacks.push_back(fn);
    recursive_mutex_exit(&mutex);
}

void mod_Stdio::unregisterPanicCallback(panic_fn fn)
{
    recursive_mutex_enter_blocking(&mutex);
    for (auto it = panicCallbacks.begin(); it != panicCallbacks.end(); it++)
    {
        if (*it == fn)
        {
            panicCallbacks.erase(it);
            break;
        }
    }
    recursive_mutex_exit(&mutex);
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


extern "C" {

void my_printf(const char *fmt, ...) //printf from the spi sd library
{
    const mod_Stdio::LogLevel logLevel = mMApp.stdio.getLogLevel();
    mMApp.stdio.setLogLevel(mod_Stdio::LogLevel::verbose);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    mMApp.stdio.setLogLevel(logLevel);
}

void my_assert_func(const char *file, int line, const char *func,
                    const char *pred) {
    LOG_PANIC("assertion \"%s\" failed: file \"%s\", line %d, function: %s\n",
           pred, file, line, func);
}

void __assert_func(const char *file, int line, const char *func, const char *failedexpr) {
    my_assert_func(file, line, func, failedexpr);
}

}