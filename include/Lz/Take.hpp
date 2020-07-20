#pragma once

#include <vector>
#include <array>

#include <Lz/detail/TakeIterator.hpp>
#include <Lz/detail/BasicIteratorView.hpp>


namespace lz {
    template<class Iterator, class Function>
    class Take final : public detail::BasicIteratorView<detail::TakeIterator<Iterator, Function>> {
    public:
        using iterator = detail::TakeIterator<Iterator, Function>;
        using const_iterator = iterator;

        using value_type = typename std::iterator_traits<Iterator>::value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        /**
         * @brief Takes elements from an iterator from [begin, ...) while the function returns true. If the function
         * returns false, the iterator stops.
         * @param begin The beginning of the iterator.
         * @param end The ending of the iterator.
         * @param function Function that must contain a the value type in its arguments and must return a bool. If the
         * function returns false, the iterator stops.
         */
        Take(Iterator begin, Iterator end, Function function) :
            _begin(begin, end, function),
            _end(end, end, function) {
        }

        /**
         * @brief Returns the beginning of the iterator.
         * @return The beginning of the iterator.
         */
        iterator begin() const override {
            return _begin;
        }

        /**
         * @brief Returns the ending of the iterator.
         * @return The ending of the iterator.
         */
        iterator end() const override {
            return _end;
        }
    };

    // Start of group
    /**
     * @defgroup ItFns Iterator free functions.
     * These are the iterator functions and can all be used to iterate over in a
     * `for (auto var : lz::someiterator(...))`-like fashion. Also, all objects contain a `toVector`,
     * `toVector<Allocator>`, `toArray<N>`, `to<container>` (specifying its value type of the container is not
     *  necessary, so e.g. `to<std::list>()` will do), `begin()`, `end()` methods and `value_type` and `iterator`
     *  typedefs.
     * @{
     */

    /**
     * @brief Takes elements from an iterator from [begin, ...) while the function returns true. If the function
     * returns false, the iterator stops. Its `begin()` function returns a random access iterator.
     * If MSVC and the type is an STL iterator, pass a pointer iterator, not an actual iterator object.
     * @tparam Iterator Is automatically deduced.
     * @tparam Function Is automatically deduced.
     * @param begin The beginning of the iterator.
     * @param end The beginning of the iterator.
     * @param predicate A function that returns a bool and passes a value type in its argument. If the function returns
     * false, the iterator stops.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::takewhilerange(...))`.
     */
    template<class Iterator, class Function>
    auto takewhilerange(Iterator begin, Iterator end, Function predicate) {
        return Take<Iterator, Function>(begin, end, predicate);
    }

    /**
     * @brief This function does the same as `lz::takewhilerange` except that it takes an iterable as parameter.
     * Its `begin()` function returns a random access iterator.
     * @tparam Iterator Is automatically deduced.
     * @tparam Function Is automatically deduced.
     * @param iterable An object that has methods `begin()` and `end()`.
     * @param predicate A function that returns a bool and passes a value type in its argument. If the function returns
     * false, the iterator stops.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::takewhile(...))`.
     */
    template<class Iterable, class Function>
#ifdef _MSC_VER
    typename std::enable_if<detail::IsContiguousContainer<Iterable>::value,
        decltype(takewhilerange(&*std::begin(std::declval<Iterable>()),
                                &*std::begin(std::declval<Iterable>()),
                                std::declval<Function>()))>::type
#else
    auto
#endif
    takewhile(Iterable&& iterable, Function predicate) {
#ifdef _MSC_VER
        // If MSVC Compiler is the defined, the operator + of an arbitrary STL container contains a
        // _Verify_Offset(size_t) method which causes the program to crash if the amount added to the iterator is
        // past-the-end and also causing the operator>= never to be used.
        auto begin = std::begin(iterable);
        auto end = std::end(iterable);

        if (begin == end) {  // Prevent UB when subtracting 1 and dereference it
            return takewhilerange(&(*begin), &(*end), predicate);
        }

        --end;
        return takewhilerange(&(*begin), &(*end) + 1, predicate);
#else
        return takewhilerange(std::begin(iterable), std::end(iterable), predicate);
#endif
    }

#ifdef _MSC_VER
    template<class Iterable, class Function>
    typename std::enable_if<!detail::IsContiguousContainer<Iterable>::value,
        decltype(takewhilerange(std::declval<Iterable>().begin(),
                                std::declval<Iterable>().begin(),
                                std::declval<Function>()))>::type
    takewhile(Iterable&& iterable, Function predicate) {
        return takewhilerange(std::begin(iterable), std::end(iterable), predicate);
    }
#endif

    /**
     * @brief This function takes a range between two iterators from [begin, end). Its `begin()` function returns a
     * random access iterator. If MSVC and the type is an STL iterator, pass a pointer iterator, not an actual
     * iterator object.
     * @tparam Iterator Is automatically deduced.
     * @param begin The beginning of the 'view'.
     * @param end The ending of the 'view'.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::takerange(...))`.
     */
    template<class Iterator>
    auto takerange(Iterator begin, Iterator end) {
        return takewhilerange(begin, end, [](const auto&) { return true; });
    }

    /**
     * @brief This function takes an iterable and slices `amount` from the beginning of the array. Essentially it is
     * equivalent to [`iterable.begin(), iterable.begin() + amount`). Its `begin()` function returns a random
     * access iterator.
     * @tparam Iterable Is automatically deduced.
     * @param iterable An iterable with method `begin()`.
     * @param amount The amount of elements to take from the beginning of the `iterable`.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::take(...))`.
     */
    template<class Iterable>
    auto take(Iterable&& iterable, const size_t amount) {
        auto begin = std::begin(iterable);
        return takerange(begin, std::next(begin, amount));
    }

    /**
     * @brief This function slices an iterable. It is equivalent to [`begin() + from, begin() + to`).
     * Its `begin()` function returns a random access iterator.
     * @tparam Iterable Is automatically deduced.
     * @param iterable An iterable with method `begin()`.
     * @param from The offset from the beginning of the iterable.
     * @param to The offset from the beginning to take. `from` must be higher than `to`.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::slice(...))`.
     */
    template<class Iterable>
    auto slice(Iterable&& iterable, const size_t from, const size_t to) {
        auto begin = std::begin(iterable);
        return takerange(std::next(begin, from), std::next(begin, to));
    }

    // End of group
    /**
     * @}
     */
}
