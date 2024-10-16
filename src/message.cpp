#include "Message.h"
#include "Queue.h"

void* Message::operator new(size_t size, Queue& queue) {
    return queue.Allocate(size);
}

void Message::operator delete(void* ptr) {
    if (Queue::GetInQueue().Owns(ptr)) {
        Queue::GetInQueue().Deallocate(ptr);
    } else if (Queue::GetOutQueue().Owns(ptr)) {
        Queue::GetOutQueue().Deallocate(ptr);
    }
}
