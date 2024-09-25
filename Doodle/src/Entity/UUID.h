#pragma once

#include "pch.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <rfl.hpp>

namespace Doodle
{

class DOO_API UUID
{
public:
    static UUID Nil()
    {
        return UUID(boost::uuids::nil_uuid());
    }

    // 默认构造函数，生成一个随机 UUID
    UUID()
    {
        m_generator = boost::uuids::random_generator();
        m_uuid = m_generator();
    }

    UUID(boost::uuids::uuid uuid) : m_uuid(uuid)
    {
    }

    // 从字符串构造 UUID
    explicit UUID(const std::string &uuidStr)
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

    operator std::string() const
    {
        return ToString();
    }

private:
    boost::uuids::uuid m_uuid;                  // 存储 UUID
    boost::uuids::random_generator m_generator; // UUID 生成器
};

inline std::ostream &operator<<(std::ostream &os, const UUID &uuid)
{
    return os << uuid.ToString();
}

} // namespace Doodle

// 为 UUID 提供 std::hash 特化
namespace std
{
template <> struct hash<Doodle::UUID>
{
    std::size_t operator()(const Doodle::UUID &uuid) const noexcept
    {
        return std::hash<std::string>()(uuid.ToString());
    }
};

} // namespace std

namespace rfl
{

template <> struct Reflector<Doodle::UUID>
{
    struct ReflType
    {
        std::string UUIDStr;
    };

    static Doodle::UUID to(const ReflType &v) noexcept
    {
        return Doodle::UUID(v.UUIDStr);
    }

    static ReflType from(const Doodle::UUID &v)
    {
        return {v.ToString()};
    }
};
} // namespace rfl
