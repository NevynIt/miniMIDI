#include "Queue.h"
#include "Message.h"
#include <cstring>

Queue* Queue::in_queue = nullptr;
Queue* Queue::out_queue = nullptr;

#define DEFAULT_BUFFER_SIZE 1024*4

Queue& Queue::GetInQueue() {
    if (!in_queue) {
        in_queue = new Queue(DEFAULT_BUFFER_SIZE); // Example buffer size
    }
    return *in_queue;
}

Queue& Queue::GetOutQueue() {
    if (!out_queue) {
        out_queue = new Queue(DEFAULT_BUFFER_SIZE); // Example buffer size
    }
    return *out_queue;
}

Queue::Queue(size_t buffer_size)
    : buffer_size(buffer_size), next_free(nullptr), first_allocated(nullptr), next_dequeue(nullptr) {
    buffer = new char[buffer_size];
    next_free = buffer;
    first_allocated = buffer;
    next_dequeue = buffer;
    allocated_wrap_point = buffer + buffer_size;
    queued_wrap_point = buffer + buffer_size;
}

Queue::~Queue() {
    delete[] buffer;
}

Message* Queue::Dequeue() {
    if (IsEmpty()) {
        return nullptr;
    }
    if (next_dequeue == queued_wrap_point) {
        next_dequeue = buffer; // Wrap around
        queued_wrap_point = buffer + buffer_size;
    }
    Message* message = reinterpret_cast<Message*>(next_dequeue);
    next_dequeue += message->GetSize();
    return message;
}

bool Queue::IsEmpty() const {
    return next_dequeue == next_free;
}

size_t Queue::AvailableSpace() const {
    if (next_free >= first_allocated) {
        return buffer_size - (next_free - first_allocated);
    } else {
        return first_allocated - next_free;
    }
}

void* Queue::Allocate(size_t size) {
    // assume that size is always significantly less than buffer_size
    // Check if there is enough space from next_free to the end of the buffer
    if (next_free + size > buffer + buffer_size) {
        // Check if there is enough space from the start of the buffer to the wrap point
        while (buffer + size > first_allocated) {
            // If there isn't enough space, deallocate the block at first_allocated
            if (first_allocated == next_dequeue) {
                Dequeue(); // Dequeue the block if it hasn't been dequeued yet
            }
            Deallocate(first_allocated);
        }
        // Wrap around if we reach the end of the buffer
        queued_wrap_point = next_free;
        allocated_wrap_point = next_free;

        next_free = buffer;
    }

    // Check if the new allocation would overlap with the first allocated block
    while (next_free + size > first_allocated && next_free < first_allocated) {
        // If there isn't enough space, deallocate the block at first_allocated
        if (first_allocated == next_dequeue) {
            Dequeue(); // Dequeue the block if it hasn't been dequeued yet
        }
        Deallocate(first_allocated);
    }

    void* ptr = next_free;
    next_free += size;
    return ptr;
}

void Queue::Deallocate(void* ptr) {
    // assume that deallocation is sequential, i.e. the block allocated earlier is deallocated first
    first_allocated = (char *)ptr + reinterpret_cast<Message*>(ptr)->GetSize();
    if (first_allocated == allocated_wrap_point) {
        allocated_wrap_point = buffer + buffer_size;
        first_allocated = buffer;
    }
}

bool Queue::Owns(void* ptr) const {
    return ptr >= buffer && ptr < buffer + buffer_size;
}