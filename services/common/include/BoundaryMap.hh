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

#include <map>

namespace fys {
template<typename T = int>
class BoundaryMap {
public:

	[[nodiscard]] auto
	get(int index) const
	{
		return _map.lower_bound(index);
	}

	void insert(int index, T&& element)
	{
		auto it = get(index);
		if (it == _map.end()) {
			_map[index] = std::forward<T>(element);
		}
		else if (element != it->second) {
			_map[it->first + 1] = std::forward<T>(element);
		}
		else {
			_map.erase(it);
			_map[it->first] = std::forward<T>(element);
		}
	}

	void insert(int index, const T& element)
	{
		T elem = element;
		insert(index, std::move(elem));
	}

	[[nodiscard]] auto
	end() const
	{
		return _map.end();
	}

private:
	std::map<int, T> _map;
};

using BoundaryMapInt = BoundaryMap<int>;

} // namespace fys
