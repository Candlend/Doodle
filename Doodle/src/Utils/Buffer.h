#pragma once

#include "Log.h"
#include "pch.h"
#include <cstddef>

namespace Doodle
{

struct Buffer
{
    void *Data = nullptr;
    uint64_t Size = 0;

    Buffer() = default;

    explicit Buffer(const void *data, uint64_t size = 0) : Data(const_cast<void *>(data)), Size(size)
    {
    }

    ~Buffer()
    {
        DOO_CORE_DEBUG("Buffer destroyed");
    }

    static Buffer Copy(const Buffer &other)
    {
        Buffer buffer;
        buffer.Allocate(other.Size);
        memcpy(buffer.Data, other.Data, other.Size);
        return buffer;
    }

    static Buffer Copy(const void *data, uint64_t size)
    {
        Buffer buffer;
        buffer.Allocate(size);
        memcpy(buffer.Data, data, size);
        return buffer;
    }

    void Allocate(uint64_t size)
    {
        delete[] static_cast<std::byte *>(Data);
        Data = nullptr;
        Size = size;

        if (size == 0)
            return;

        Data = new std::byte[size];
    }

    void Release()
    {
        delete[] static_cast<std::byte *>(Data);
        Data = nullptr;
        Size = 0;
    }

    void ZeroInitialize() // NOLINT
    {
        if (Data)
            memset(Data, 0, Size);
    }

    template <typename T> T &Read(uint64_t offset = 0)
    {
        return *reinterpret_cast<T *>(static_cast<std::byte *>(Data) + offset);
    }

    template <typename T> const T &Read(uint64_t offset = 0) const
    {
        return *reinterpret_cast<const T *>(static_cast<std::byte *>(Data) + offset);
    }

    std::byte *ReadBytes(uint64_t size, uint64_t offset) const
    {
        DOO_CORE_ASSERT(offset + size <= Size, "Buffer overflow!");
        std::byte *buffer = new std::byte[size];
        memcpy(buffer, static_cast<std::byte *>(Data) + offset, size);
        return buffer;
    }

    void Write(const void *data, uint64_t size, uint64_t offset = 0) // NOLINT
    {
        DOO_CORE_ASSERT(offset + size <= Size, "Buffer overflow!");
        memcpy(static_cast<std::byte *>(Data) + offset, data, size);
    }

    operator bool() const
    {
        return static_cast<bool>(Data);
    }

    std::byte &operator[](int index) // NOLINT
    {
        return (static_cast<std::byte *>(Data))[index];
    }

    std::byte operator[](int index) const
    {
        return (static_cast<std::byte *>(Data))[index];
    }

    template <typename T> T *As() const
    {
        return static_cast<T *>(Data);
    }

    inline uint64_t GetSize() const
    {
        return Size;
    }

    operator std::byte *() const
    {
        return static_cast<std::byte *>(Data);
    }

    void PrintBinary()
    {
        uint8_t *bytePtr = reinterpret_cast<uint8_t *>(const_cast<void *>(Data)); // 将 void * 转换为 uint8_t *
        std::ostringstream oss;
        for (size_t i = 0; i < Size; i++)
        {
            for (int j = 7; j >= 0; j--)
            { // 从高位到低位输出每个字节
                oss << ((bytePtr[i] & (1 << j)) ? '1' : '0');
            }
            oss << ' '; // 每个字节之间加个空格
        }
        DOO_CORE_DEBUG("Buffer: {0}", oss.str());
    }
};

struct BufferSafe : public Buffer
{
    ~BufferSafe()
    {
        Release();
    }

    static BufferSafe Copy(const void *data, uint64_t size)
    {
        BufferSafe buffer;
        buffer.Allocate(size);
        memcpy(buffer.Data, data, size);
        return buffer;
    }
};

} // namespace Doodle