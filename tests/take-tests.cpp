#include <list>

#include <catch.hpp>
#include <Lz/Take.hpp>


TEST_CASE("Take changing and creating elements", "[Take][Basic functionality]") {
    std::array<int, 10> array = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    SECTION("Should take 3 elements, 0 offset") {
        auto taken = lz::take(array, 3);
        auto it = taken.begin();

        CHECK(*it == 1);
        ++it;
        CHECK(*it == 2);
        ++it;
        CHECK(*it == 3);
        ++it;
        CHECK(it == taken.end());
    }

    SECTION("Should be by ref") {
        auto taken = lz::take(array, 3);
        auto actual = taken.begin();
        *actual = 0;
        CHECK(array[0] == 0);
    }

    SECTION("Should slice 2 elements, 2 offset") {
        auto taken = lz::slice(array, 2, 4);
        auto it = taken.begin();

        CHECK(*it == 3);
        ++it;
        CHECK(*it == 4);
        ++it;
        CHECK(it == taken.end());
    }

    SECTION("Should take while range") {
        auto taken = lz::takewhile(array, [](int i) { return i != 5; });

        int expected = 1;
        for (int actual : taken) {
            CHECK(expected == actual);
            ++expected;
        }
    }
}


TEST_CASE("Take binary operations", "[Take][Binary ops]") {
    constexpr size_t size = 3;
    std::array<int, size> array = {
        1, 2, 3
    };

    auto taken = lz::take(array, 2);
    auto it = taken.begin();

    SECTION("Operator++") {
        ++it;
        CHECK(*it == 2);
    }

    SECTION("Operator--") {
        ++it;
        --it;
        CHECK(*it == 1);
    }

    SECTION("Operator+(int) offset, tests += as well") {
        CHECK(*(it + 1) == 2);
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        ++it;
        CHECK(*(it - 1) == 1);
    }

    SECTION("Operator-(Iterator)") {
        CHECK(std::distance(taken.begin(), taken.end()) == 2);
    }

    SECTION("Operator[]()") {
        CHECK(taken.begin()[0] == 1);
    }

    SECTION("Operator<, checks all '<, <=, >, >=' operators") {
        CHECK(taken.begin() < taken.end());
    }
}


TEST_CASE("Take to containers", "[Take][To container]") {
    constexpr size_t size = 3;
    std::array<int, size> array = {
        1, 2, 3
    };


    SECTION("To array") {
        constexpr size_t newSize = 2;
        auto actual = lz::take(array, newSize).toArray<newSize>();
        std::array<int, 2> expected = {1, 2};

        CHECK(actual == expected);
    }

    SECTION("To vector") {
        auto actual = lz::take(array, 2).toVector();
        std::vector<int> expected = {1, 2};

        CHECK(actual == expected);
    }

    SECTION("To other container using to<>()") {
        auto actual = lz::take(array, 2).to<std::list>();
        std::list<int> expected = {1, 2};

        CHECK(expected == actual);
    }
}