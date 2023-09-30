#pragma once

#include "fixed_containers/source_location.hpp"
#include "fixed_containers/type_name.hpp"

#include <cstddef>
#include <cstdlib>

namespace fixed_containers::customize
{
template <class T, class K>
concept MapChecking =
    requires(K key, std::size_t size, const std_transition::source_location& loc) {
        T::out_of_range(key, size, loc);  // ~ std::out_of_range
        T::length_error(size, loc);       // ~ std::length_error
    };

template <class K, class V, std::size_t /*MAXIMUM_SIZE*/>
struct MapAbortChecking
{
    // KEY_TYPE_NAME, VALUE_TYPE_NAME, MAXIMUM_SIZE are not used, but are meant as an example
    // for Checking implementations that will utilize this information.
    static constexpr auto KEY_TYPE_NAME = fixed_containers::type_name<K>();
    static constexpr auto VALUE_TYPE_NAME = fixed_containers::type_name<V>();

    [[noreturn]] static void out_of_range(const K& /*key*/,
                                          const std::size_t /*size*/,
                                          const std_transition::source_location& /*loc*/)
    {
        std::abort();
    }

    [[noreturn]] static void length_error(const std::size_t /*target_capacity*/,
                                          const std_transition::source_location& /*loc*/)
    {
        std::abort();
    }
};
}  // namespace fixed_containers::customize
