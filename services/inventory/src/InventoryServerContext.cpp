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


#include "InventoryServerContext.hh"

namespace fys::inv {

InventoryServerContext::InventoryServerContext(int argc, char** argv)
		:
		common::ServiceContextBase(argc, argv)
{

}
std::string
InventoryServerContext::toString() const
{
	std::string str;
	str = "dump context\n*************************\n";
	str += "[INFO] Service " + _name + " context VERSION: " + _version + "\n";
	str += "[INFO] Config file used: " + _configFile + "\n";
	str += "\n[INFO] Dispatcher connected port: " + std::to_string(_dispatcherData.port) + "\n";
	str += "[INFO] Dispatcher connected host: " + _dispatcherData.address + "\n";
	str += "[INFO] Dispatcher connection string: " + getDispatcherConnectionString() + "\n";
	str += "\n*************************\n";
	return str;
}

}