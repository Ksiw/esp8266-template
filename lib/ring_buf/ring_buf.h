#pragma once

#include <stddef.h>
#include <stdint.h>

// FIFO-буфер фиксированного размера. При заполнении push() заменяет старый элемент.
// Fixed-size FIFO buffer. When full, push() replaces the oldest element.
class RingBuf {
public:
    // Выделяет память для capacity элементов длиной element_size байт.
    // Allocates storage for capacity elements, each element_size bytes long.
    RingBuf(size_t element_size, size_t capacity);
    ~RingBuf();

    RingBuf(const RingBuf &)            = delete;
    RingBuf &operator=(const RingBuf &) = delete;

    // Добавляет элемент. Возвращает false, если буфер не инициализирован.
    // Adds an element. Returns false if the buffer was not initialized.
    bool push(const void *element);

    // Копирует и удаляет старый элемент. Возвращает false для пустого буфера.
    // Copies and removes the oldest element. Returns false when empty.
    bool pop(void *element);

    // Указатель на старый элемент либо nullptr для пустого буфера.
    // Pointer to the oldest element, or nullptr when empty.
    // Указатель действителен до следующего push(), pop(), discard() или clear().
    // The pointer remains valid until the next push(), pop(), discard(), or clear().
    const void *front() const;

    // Удаляет старый элемент. Возвращает false для пустого буфера.
    // Removes the oldest element. Returns false when empty.
    bool discard();
    void clear();

    size_t count() const;
    size_t capacity() const;
    size_t available() const;
    bool empty() const;
    bool full() const;

private:
    uint8_t *storage_;
    size_t element_size_;
    size_t capacity_;
    size_t head_;
    size_t count_;
};
