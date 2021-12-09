#include "fixed_containers/string_literal.hpp"

#include <gtest/gtest.h>

#include <atomic>
#include <map>

namespace fixed_containers
{
// Static assert for expected type properties
static_assert(std::is_trivially_copyable_v<StringLiteral>);
static_assert(!std::is_trivial_v<StringLiteral>);
static_assert(std::is_standard_layout_v<StringLiteral>);

TEST(Utilities, StringLiteral_DefaultCtor)
{
    {
        constexpr StringLiteral ZERO;
        static_assert(ZERO.size() == 0);
    }
    {
        StringLiteral zero{};
        EXPECT_EQ(0, zero.size());
    }
}

TEST(Utilities, StringLiteral_Ctor)
{
    static constexpr const char MY_LITERAL[5] = "blah";  // 4 chars + null terminator

    constexpr StringLiteral s = MY_LITERAL;
    constexpr StringLiteral s2 = s;

    static_assert(s.as_view() == MY_LITERAL);
    static_assert(s.as_view() == s2);
    static_assert(s == s2.as_view());

    static_assert(s.size() == 4);
    static_assert(s2.size() == 4);
}

TEST(Utilities, StringLiteral_CopyandMoveCtor)
{
    struct MyStruct
    {
        StringLiteral a;
        StringLiteral b;
    };

    constexpr MyStruct t{"foo", "bar"};

    static_assert(t.a.as_view() == "foo");
    static_assert(t.b.as_view() == "bar");

    // For trivial types, move == copy.
    constexpr MyStruct h{t};
    static_assert(h.a.as_view() == "foo");
    static_assert(h.b.as_view() == "bar");
}

TEST(Utilities, StringLiteral_CStr)
{
    static constexpr const char MY_LITERAL[5] = "blah";  // 4 chars + null terminator
    constexpr StringLiteral s = MY_LITERAL;
    static_assert(s.as_view() == MY_LITERAL);
    static_assert(s.size() == 4);

    const std::string no_string_interning = std::string{"bla"} + std::string{"h"};
    const char* as_auto_converted_char = s;
    EXPECT_TRUE((std::string{as_auto_converted_char} == no_string_interning));
    EXPECT_TRUE((std::string{s.c_str()} == no_string_interning));
}

TEST(Utilities, StringLiteral_UsageInMap)
{
    {
        std::map<StringLiteral, int> my_map{{StringLiteral{"a"}, 1}};
        StringLiteral s1 = "a";
        StringLiteral s2 = "b";

        my_map[s1] = 1;
        my_map[s2] = 2;
        my_map["c"] = 3;

        ASSERT_EQ("a", my_map.begin()->first);
        ASSERT_EQ("b", std::next(my_map.begin(), 1)->first);
        ASSERT_EQ("c", std::next(my_map.begin(), 2)->first);
    }

    {
        // Transparent
        std::map<StringLiteral, int, std::less<>> my_map{{StringLiteral{"a"}, 1}};
        StringLiteral s1 = "a";
        StringLiteral s2 = "b";

        my_map[s1] = 1;
        my_map[s2] = 2;
        my_map["c"] = 3;

        auto it = my_map.find(std::string_view{"c"});
        ASSERT_EQ(it->first, "c");

        auto it2 = my_map.find("c");
        ASSERT_EQ(it2->first, "c");

        ASSERT_EQ("a", my_map.begin()->first);
        ASSERT_EQ("b", std::next(my_map.begin(), 1)->first);
        ASSERT_EQ("c", std::next(my_map.begin(), 2)->first);
    }
}

}  // namespace fixed_containers
