// MIT License
//
// Copyright (c) 2019 Quentin Balland
// Repository : https://github.com/FreeYourSoul/FyS
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
//         of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
//         to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//         copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
//         copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//         AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#ifndef FYS_SERVICE_ALGORITHM_HH
#define FYS_SERVICE_ALGORITHM_HH

#include <type_traits>

namespace fys
{

    template<typename T, typename = void>
    struct is_iterator
    {
        static constexpr bool value = false;
    };

    template<typename T>
    struct is_iterator<T, std::enable_if_t<!std::is_same<typename std::iterator_traits<T>::value_type, void>::value>>
    {
        static constexpr bool value = true;
    };
    
    /**
     * @brief Get the most suitable (suitable being defined by a comparator) of a given container.
     * The most suitable follow the comparator.
     *
     * For example the most suitable following this comparator [](auto currentSuitable, auto next) { return currentSuitable < next; }
     * will be the biggest element of the container.
     *
     * @param first iterator start of the container
     * @param last iterator end of the container
     * @param predicate predicate that takes the current most suitable and the next value to do a comparison
     * @return iterator pointing on the maximum value described by the predicate, if container is empty, last is returned
     */
    template <typename InputIt, typename ComparePredicate>
    InputIt find_most_suitable(InputIt first, InputIt last, ComparePredicate && comp, InputIt start) {
        static_assert(is_iterator<InputIt>::value);

        auto suitable = start;
        while (first != last) {
            if (std::forward<ComparePredicate>(comp)(*suitable, *first)) {
                suitable = first;
            }
            ++first;
        }
        return suitable;
    }
    template <typename InputIt, typename ComparePredicate>
    InputIt find_most_suitable(InputIt first, InputIt last, ComparePredicate && comp) {
        return find_most_suitable(first, last, std::forward<ComparePredicate>(comp), first);
    }

    /**
     *
     *
     * @param first
     * @param last
     * @param retriever
     * @param algo
     * @return
     */
    template <typename InputIt, typename RetrieverLower, typename AlgorithmPredicate>
    InputIt compose_most_suitable(InputIt first, InputIt last, RetrieverLower && retriever, AlgorithmPredicate && algo) {
        static_assert(is_iterator<InputIt>::value);
//        static_assert(is_unique_composable<AlgorithmPredicate>());

        if (first == last)
            return first;

        auto suitable = retriever(*first).begin();
        while (first != last) {
            auto &innerContainer = std::forward(retriever)(*first);
            if (auto suitableTmp = std::forward(algo)(innerContainer.begin(), innerContainer.end(), suitable);
                    suitableTmp != innerContainer.end())
                suitable = suitableTmp;
            ++first;
        }
        return suitable;
    }

}

#endif //FYS_SERVICE_ALGORITHM_HH
