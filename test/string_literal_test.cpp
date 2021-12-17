#include "fixed_containers/string_literal.hpp"

#include <gtest/gtest.h>

#include <atomic>
#include <map>
#include <span>

namespace fixed_containers
{
namespace
{
// Order is intentional! It is to ensure we are not relying on the pointer address
// (which the default comparison ops would use)
constexpr StringLiteral BBBBB1 = "bbbb1_unique_string_to_avoid_string_interning";
constexpr StringLiteral AAAAA1 = "aaaa1_unique_string_to_avoid_string_interning";
constexpr StringLiteral CCCCC1 = "cccc1_unique_string_to_avoid_string_interning";
}  // namespace

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

TEST(Utilities, StringLiteral_Equality)
{
    static constexpr const char* CHAR_PTR = "blah";
    static constexpr const char* CHAR_PTR2 = "blah2";

    static constexpr const char MY_LITERAL[5] = "blah";  // 4 chars + null terminator
    constexpr StringLiteral s = MY_LITERAL;

    constexpr StringLiteral s1 = "blah";
    constexpr std::string_view s2 = "blah";
    constexpr StringLiteral s3 = "blah3";
    constexpr std::string_view s4 = "blah4";

    static_assert(s == s);
    static_assert(s == s1);
    static_assert(s1 == s);

    {
        static_assert(s == s2);
        static_assert(s == s2);
        static_assert(s == CHAR_PTR);
        static_assert(s != CHAR_PTR2);
        static_assert(s != s3);
        static_assert(s != s4);

        static_assert(s2 == s);
        static_assert(s2 == s);
        static_assert(CHAR_PTR == s);
        static_assert(CHAR_PTR2 != s);
        static_assert(s3 != s);
        static_assert(s4 != s);
    }

    {
        static_assert(s1 == s2);
        static_assert(s1 == CHAR_PTR);
        static_assert(s1 != CHAR_PTR2);
        static_assert(s1 != s3);
        static_assert(s1 != s4);

        static_assert(s2 == s1);
        static_assert(CHAR_PTR == s1);
        static_assert(CHAR_PTR2 != s1);
        static_assert(s3 != s1);
        static_assert(s4 != s1);
    }
}

TEST(Utilities, StringLiteral_Comparison)
{
    static constexpr const char* CHAR_PTR = "blah";
    static constexpr const char* CHAR_PTR2 = "blah2";

    static constexpr const char MY_LITERAL[5] = "blah";  // 4 chars + null terminator
    constexpr StringLiteral s = MY_LITERAL;

    constexpr StringLiteral s1 = "blah";
    constexpr std::string_view s2 = "blah";
    constexpr StringLiteral s3 = "blah3";
    constexpr std::string_view s4 = "blah4";

    static_assert(s >= s && s <= s);
    static_assert(s >= s1 && s <= s1);
    static_assert(s1 >= s && s1 <= s);

    {
        static_assert(s >= s2 && s <= s2);
        static_assert(s >= CHAR_PTR && s <= CHAR_PTR);
        static_assert(s < CHAR_PTR2);
        static_assert(s < s3);
        static_assert(s < s4);

        static_assert(s2 >= s && s2 <= s);
        static_assert(CHAR_PTR >= s && CHAR_PTR <= s);
        static_assert(CHAR_PTR2 > s);
        static_assert(s3 > s);
        static_assert(s4 > s);
    }

    {
        static_assert(s1 >= s2 && s1 <= s2);
        static_assert(s1 >= CHAR_PTR && s1 <= CHAR_PTR);
        static_assert(s1 < CHAR_PTR2);
        static_assert(s1 < s3);
        static_assert(s1 < s4);

        static_assert(s2 >= s1 && s2 <= s1);
        static_assert(CHAR_PTR >= s1 && CHAR_PTR >= s1);
        static_assert(CHAR_PTR2 > s1);
        static_assert(s3 > s1);
        static_assert(s4 > s1);
    }

    {
        static_assert(AAAAA1 < BBBBB1);
        static_assert(AAAAA1 < CCCCC1);
        static_assert(BBBBB1 < CCCCC1);

        static_assert(BBBBB1 > AAAAA1);
        static_assert(CCCCC1 > AAAAA1);
        static_assert(CCCCC1 > BBBBB1);
    }
}

TEST(Utilities, StringLiteral_UsageInMap)
{
    std::map<StringLiteral, int> my_map{{StringLiteral{"a"}, 1}};

    my_map[AAAAA1] = 11;
    my_map[BBBBB1] = 21;
    my_map[CCCCC1] = 31;
    my_map["c"] = 3;

    ASSERT_EQ("a", my_map.begin()->first);
    ASSERT_EQ(AAAAA1, std::next(my_map.begin(), 1)->first);
    ASSERT_EQ(BBBBB1, std::next(my_map.begin(), 2)->first);
    ASSERT_EQ("c", std::next(my_map.begin(), 3)->first);
    ASSERT_EQ(CCCCC1, std::next(my_map.begin(), 4)->first);
}

TEST(Utilities, StringLiteral_UsageInMap_Transparent)
{
    std::map<StringLiteral, int, std::less<>> my_map{{StringLiteral{"aa"}, 1}};

    my_map[AAAAA1] = 11;
    my_map[BBBBB1] = 21;
    my_map[CCCCC1] = 31;
    my_map["c"] = 3;

    {
        for (auto&& [key, value] : my_map)
        {
            std::cout << key << std::endl;
        }

        // std::string_view
        std::string lookup{"c"};
        auto it = my_map.find(lookup);
        ASSERT_NE(it, my_map.cend());
        ASSERT_EQ(it->first, "c");
    }

    {
        // std::string_view
        std::string lookup{"c"};
        auto it = my_map.find(std::string_view{lookup});
        ASSERT_NE(it, my_map.cend());
        ASSERT_EQ(it->first, "c");
    }

    {
        // const char *
        auto it = my_map.find("c");
        ASSERT_NE(it, my_map.cend());
        ASSERT_EQ(it->first, "c");
    }

    ASSERT_EQ("a", my_map.begin()->first);
    ASSERT_EQ(AAAAA1, std::next(my_map.begin(), 1)->first);
    ASSERT_EQ(BBBBB1, std::next(my_map.begin(), 2)->first);
    ASSERT_EQ("c", std::next(my_map.begin(), 3)->first);
    ASSERT_EQ(CCCCC1, std::next(my_map.begin(), 4)->first);
}

}  // namespace fixed_containers
