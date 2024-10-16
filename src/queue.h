#ifndef QUEUE_H
#define QUEUE_H

#include <cstddef>

class Message;

class Queue {
public:
    static Queue& GetInQueue();
    static Queue& GetOutQueue();

    Message* Dequeue();
    bool IsEmpty() const;
    size_t AvailableSpace() const;

    // Allocation itself counts as enqueueing
    void* Allocate(size_t size);
    void Deallocate(void* ptr);
    bool Owns(void* ptr) const;

private:
    Queue(size_t buffer_size);
    ~Queue();

    static Queue* in_queue;
    static Queue* out_queue;

    char* buffer;
    size_t buffer_size;
    char* next_free;
    char* first_allocated;
    char* next_dequeue;
    char* allocated_wrap_point;
    char* queued_wrap_point;
};

#endif // QUEUE_H