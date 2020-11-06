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

#include <flatbuffer_generator.hh>

namespace fys::inv {

std::pair<void *, uint>
flatbuffer_generator::generate_initiate_exchange_response(const exchange_room &room) {
  return std::pair<void *, uint>();
}

std::pair<void *, uint>
flatbuffer_generator::generate_add_item_to_room_response(const exchange_room &room) {
  return std::pair<void *, uint>();
}

std::pair<void *, uint>
flatbuffer_generator::generate_remove_item_from_room_response(const exchange_room &room) {
  return std::pair<void *, uint>();
}

std::pair<void *, uint>
flatbuffer_generator::generate_lock_room_transaction_response(const exchange_room &room) {
  return std::pair<void *, uint>();
}

std::pair<void *, uint>
flatbuffer_generator::generate_terminate_transaction_response(const exchange_room &room) {
  return std::pair<void *, uint>();
}

}// namespace fys::inv