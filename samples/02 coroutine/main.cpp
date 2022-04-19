/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#include <gba/gba.hpp>

int main() {
    using namespace gba;
    using namespace agbabi;

    auto stack = std::array<int, 200>{};

    auto myFiber = fiber(stack, [](auto& yield){
        yield();
    });

    myFiber();

    // Fibonacci pull
    auto fibPull = coroutine<int>::pull_type(stack, [](auto& yield){
        auto first = 1, second = 1;
        yield(first);
        yield(second);
        for (int i = 0; i < 8; ++i) {
            const auto third = first + second;
            first = second;
            second = third;
            yield(third);
        }
    });

    int a = 0;
    for (auto i : fibPull) {
        a += i;
    }

    // Fibonacci push
    a = 0;
    auto fibPush = coroutine<int>::push_type(stack, [&a](auto& yield) {
        for (int i : yield) {
            a += i;
        }
    });

    auto first = 1, second = 1;
    fibPush(first);
    fibPush(second);
    for (int i = 0; i < 8; ++i) {
        const auto third = first + second;
        first = second;
        second = third;
        fibPush(third);
    }

    return 0;
}
