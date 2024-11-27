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
    mutex_init(&mutex);
}

void mod_Stdio::Tick()
{
}

void mod_Stdio::Test()
{
}

int mod_Stdio::in_chars(char *buf, int len)
{
    mutex_enter_blocking(&mutex);
    int l = in_buffer.length();
    if (l > len)
        l = len;
    in_buffer.copy(buf, l);
    in_buffer.erase(0, l);
    mutex_exit(&mutex);
    return l;
}

void mod_Stdio::out_chars(const char *buf, int len)
{
    mutex_enter_blocking(&mutex);
    for (int l = level[get_core_num()]; l < LogLevel::LOG_LEVEL_COUNT; l++)
    {
        for (auto &fn : printCallbacks[l])
        {
            fn(buf, len);
        }
    }
    mutex_exit(&mutex);
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
    mutex_enter_blocking(&mutex);
    printCallbacks[level].push_back(fn);
    mutex_exit(&mutex);
}

void mod_Stdio::unregisterPrintCallback(print_fn fn, LogLevel level)
{
    mutex_enter_blocking(&mutex);
    auto &callbacks = printCallbacks[level];
    for (auto it = callbacks.begin(); it != callbacks.end(); it++)
    {
        if (*it == fn)
        {
            callbacks.erase(it);
            break;
        }
    }
    mutex_exit(&mutex);
}

void mod_Stdio::registerPanicCallback(panic_fn fn)
{
    mutex_enter_blocking(&mutex);
    panicCallbacks.push_back(fn);
    mutex_exit(&mutex);
}

void mod_Stdio::unregisterPanicCallback(panic_fn fn)
{
    mutex_enter_blocking(&mutex);
    for (auto it = panicCallbacks.begin(); it != panicCallbacks.end(); it++)
    {
        if (*it == fn)
        {
            panicCallbacks.erase(it);
            break;
        }
    }
    mutex_exit(&mutex);
}

void mod_Stdio::push(char c)
{
    mutex_enter_blocking(&mutex);
    in_buffer.push_back(c);
    mutex_exit(&mutex);

    if (chars_available_callback)
    {
        chars_available_callback(chars_available_param);
    }
}

void mod_Stdio::push(std::string_view str)
{
    mutex_enter_blocking(&mutex);
    in_buffer.append(str);
    mutex_exit(&mutex);

    if (chars_available_callback)
    {
        chars_available_callback(chars_available_param);
    }
}

void mod_Stdio::push(const char *str)
{
    mutex_enter_blocking(&mutex);
    in_buffer.append(str);
    mutex_exit(&mutex);

    if (chars_available_callback)
    {
        chars_available_callback(chars_available_param);
    }
}

void mod_Stdio::push(const char *str, int len)
{
    mutex_enter_blocking(&mutex);
    in_buffer.append(str, len);
    mutex_exit(&mutex);
    
    if (chars_available_callback)
    {
        chars_available_callback(chars_available_param);
    }
}