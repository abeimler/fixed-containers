#include "fixed_containers/queue_adapter.hpp"

#include "fixed_containers/fixed_deque.hpp"

#include <gtest/gtest.h>

#include <queue>

namespace fixed_containers
{
static_assert(TriviallyCopyable<std::queue<int, FixedDeque<int, 5>>>);
#if defined(_MSC_VER)
static_assert(ConstexprDefaultConstructible<std::queue<int, FixedDeque<int, 5>>>);
#else
static_assert(!ConstexprDefaultConstructible<std::queue<int, FixedDeque<int, 5>>>);
#endif

using QueueType = QueueAdapter<FixedDeque<int, 5>>;
static_assert(TriviallyCopyable<QueueType>);
static_assert(NotTrivial<QueueType>);
static_assert(StandardLayout<QueueType>);
static_assert(IsStructuralType<QueueType>);
static_assert(ConstexprDefaultConstructible<QueueType>);

}  // namespace fixed_containers
