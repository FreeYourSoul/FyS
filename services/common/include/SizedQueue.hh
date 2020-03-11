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


#ifndef FYS_ONLINE_SIZEDQUEUE_HH
#define FYS_ONLINE_SIZEDQUEUE_HH

#include <optional>
#include <array>

namespace fys::common {

    /**
     * A Queue (aligned in memory, implemented through an array) with a fixed max.
     * If the queue is fulled, value are not override (do not work like a ringbuffer)
     *
     * @note Not thread safe (for thread safe usage, a LockFreeQueue should be used)
     * @tparam TypeContainer type the queue contains
     * @tparam SIZE_QUEUE maximum size of the queue, defaulted to 100
     */
    template <typename TypeContainer, unsigned int SIZE_QUEUE = 100>
    class SizedQueue {

    static_assert(SIZE_QUEUE < std::numeric_limits<unsigned int>::max());

    public:
        /**
         * Pop the next element of the queue
         * @return the next element of the queue, if no such element exist, return an empty optional (nullopt)
         */
        std::optional<TypeContainer> pop() {
            if (_head < _tail) {
                return _queue[getIndex(_head++)];
            }
            _head = 0;
            _tail = 0;
            return std::nullopt;
        }

        /**
         * Push an element into the queue
         *
         * @param elem element to insert into the queue
         * @return true if the element has been pushed correctly, false otherwise
         */
        bool push(TypeContainer && elem) {
            if (size() >= SIZE_QUEUE)
                return false;
            _queue[getIndex(_tail)] = std::forward<TypeContainer>(elem);
            ++_tail;
            return true;
        }

        /**
         * Get the number of element in the queue to be popped
         * @return number of element that can be popped out of the queue
         */
        uint size() const { return _tail - _head; }

    private:
        inline uint getIndex(const uint index) const {
            return index % SIZE_QUEUE;
        }

    private:
        uint _tail = 0;
        uint _head = 0;

        std::array<TypeContainer, SIZE_QUEUE> _queue;
    };

}

#endif //FYS_ONLINE_SIZEDQUEUE_HH
