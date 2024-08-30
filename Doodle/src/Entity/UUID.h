#pragma once

#include "pch.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

class UUID
{
public:
    // 默认构造函数，生成一个随机 UUID
    UUID()
    {
        m_generator = boost::uuids::random_generator();
        m_uuid = m_generator();
    }

    // 从字符串构造 UUID
    UUID(const std::string &uuidStr)
    {
        m_uuid = boost::uuids::string_generator()(uuidStr);
    }

    // 返回 UUID 的字符串表示
    std::string ToString() const
    {
        return boost::uuids::to_string(m_uuid);
    }

    // 生成新的 UUID
    void Generate()
    {
        m_uuid = m_generator();
    }

    // 重载 == 运算符
    bool operator==(const UUID &other) const
    {
        return m_uuid == other.m_uuid;
    }

    // 重载 != 运算符
    bool operator!=(const UUID &other) const
    {
        return m_uuid != other.m_uuid;
    }

    // 重载 < 运算符
    bool operator<(const UUID &other) const
    {
        return m_uuid < other.m_uuid;
    }

    // 重载 > 运算符
    bool operator>(const UUID &other) const
    {
        return m_uuid > other.m_uuid;
    }

    // 重载 <= 运算符
    bool operator<=(const UUID &other) const
    {
        return m_uuid <= other.m_uuid;
    }

    // 重载 >= 运算符
    bool operator>=(const UUID &other) const
    {
        return m_uuid >= other.m_uuid;
    }

private:
    boost::uuids::uuid m_uuid;                  // 存储 UUID
    boost::uuids::random_generator m_generator; // UUID 生成器
};

// 为 UUID 提供 std::hash 特化
namespace std
{
template <> struct hash<UUID>
{
    std::size_t operator()(const UUID &uuid) const noexcept
    {
        return std::hash<std::string>()(uuid.ToString());
    }
};
} // namespace std