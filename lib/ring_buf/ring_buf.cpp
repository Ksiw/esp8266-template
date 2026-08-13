#include "ring_buf.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

RingBuf::RingBuf(size_t element_size, size_t capacity) :
    storage_(nullptr), element_size_(element_size), capacity_(capacity), head_(0), count_(0)
{
    if (element_size_ == 0 || capacity_ == 0 || capacity_ > SIZE_MAX / element_size_)
        return;

    storage_ = static_cast<uint8_t *>(malloc(element_size_ * capacity_));
    if (storage_ == nullptr)
        capacity_ = 0;
}
//-------------------------------------------------------------------------------

RingBuf::~RingBuf()
{
    free(storage_);
}
//-------------------------------------------------------------------------------

bool RingBuf::push(const void *element)
{
    if (storage_ == nullptr || element == nullptr)
        return false;

    size_t index = (head_ + count_) % capacity_;
    if (count_ == capacity_) {
        index = head_;
        head_ = (head_ + 1) % capacity_;
    } else
        ++count_;

    memcpy(storage_ + index * element_size_, element, element_size_);
    return true;
}
//-------------------------------------------------------------------------------

bool RingBuf::pop(void *element)
{
    const void *first = front();
    if (first == nullptr || element == nullptr)
        return false;

    memcpy(element, first, element_size_);
    return discard();
}
//-------------------------------------------------------------------------------

const void *RingBuf::front() const
{
    if (empty())
        return nullptr;

    return storage_ + head_ * element_size_;
}
//-------------------------------------------------------------------------------

bool RingBuf::discard()
{
    if (empty())
        return false;

    head_ = (head_ + 1) % capacity_;
    --count_;
    return true;
}
//-------------------------------------------------------------------------------

void RingBuf::clear()
{
    head_  = 0;
    count_ = 0;
}
//-------------------------------------------------------------------------------

size_t RingBuf::count() const
{
    return count_;
}
//-------------------------------------------------------------------------------

size_t RingBuf::capacity() const
{
    return capacity_;
}
//-------------------------------------------------------------------------------

size_t RingBuf::available() const
{
    return capacity_ - count_;
}
//-------------------------------------------------------------------------------

bool RingBuf::empty() const
{
    return count_ == 0;
}
//-------------------------------------------------------------------------------

bool RingBuf::full() const
{
    return count_ == capacity_;
}
//-------------------------------------------------------------------------------
