// MIT License
//
// Copyright (c) 2021 Quentin Balland
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

#include <fmt/format.h>

#include <fil/cli/command_line_interface.hh>

#include <collision_map_converter.hh>

int main(int ac, char** av) {
  fil::command_line_interface cli([] {}, "FyS::CollisionMapConverter");
  std::string config_path = "";
  bool human_readable = false;
  std::string file_path = "NONE";
  std::string output_file_path = "./output.collision.json";

  cli.add_option(fil::option(
      "-f", [&v = config_path](std::string str) { v = std::move(str); }, "Path to all tmx folder to convert [not implemented yet]"));
  cli.add_option(fil::option(
      "-c", [&v = file_path](std::string str) { v = std::move(str); }, "Path of the file to convert"));
  cli.add_option(fil::option(
      "-o", [&v = output_file_path](std::string value) { v = std::move(value); }, "Collision map : Output file"));
  cli.add_option(fil::option("-h", [&v = human_readable]() { v = true; }, "Human readable JSON output"));

  cli.parse_command_line(ac, av);
  if (!config_path.empty())
    throw std::logic_error("Not Implemented Yet");

  fmt::print("Start collision map converter for file {} with output file set to {}\n", file_path, output_file_path);
  try {
    fys::map_converter::convert_map_from_tmx_file(file_path, output_file_path, human_readable);
  } catch (const std::exception& e) {
    fmt::print("An error occurred : {}\n", e.what());
    return 1;
  }
  return 0;
}