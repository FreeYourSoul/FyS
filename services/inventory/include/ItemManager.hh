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


#ifndef FYS_ONLINE_ITEMMANAGER_HH
#define FYS_ONLINE_ITEMMANAGER_HH

#include <string>
#include <vector>

// forward declarations
namespace fys::inv {
class InventoryServerContext;
}
// end forward declarations

namespace fys::inv {

struct Item {
	std::string itemCode;
	uint quantity;
};

/**
 * Handle items of character:
 * - Verify if an item is owned by a character
 * - Add / Remove item of a character
 */
class ItemManager {

public:
	// shall take in argument required value for connection
	explicit ItemManager() = default;

	[[nodiscard]] bool
	areItemsOwnedByUser(const std::string& userName, const std::vector<Item>& item);


private:
	// DB connector ?

};

}
#endif //FYS_ONLINE_ITEMMANAGER_HH
