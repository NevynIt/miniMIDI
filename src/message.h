#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstddef>
#include <new>

class Queue;

class Message {
public:
    virtual ~Message() = default;
    virtual void Process() = 0;
    virtual size_t GetSize() const = 0;

    void* operator new(size_t size, Queue& queue);
    void operator delete(void* ptr);
};

#endif // MESSAGE_H