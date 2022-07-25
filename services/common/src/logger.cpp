// MIT License
//
// Copyright (c) 2022 Quentin Balland
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

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "../include/logger.hh"

namespace fys {

std::shared_ptr<spdlog::logger> logger = nullptr;

void configure_logger(const std::string& name, const std::string& level, const std::string& file_path) {
  if (!logger) {
    std::vector<spdlog::sink_ptr> sinks;

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    console_sink->set_level(spdlog::level::from_str(level));
    console_sink->set_pattern(fmt::format("[%D %H:%M:%S][{}][%L]: %v", name));
    sinks.push_back(std::move(console_sink));
    if (!file_path.empty()) {
      auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(file_path, true);
      file_sink->set_level(spdlog::level::from_str(level));
      file_sink->set_pattern(fmt::format("[%D %H:%M:%S][{}][%L]: %v", name));
      sinks.push_back(std::move(file_sink));
    }

    logger = std::make_shared<spdlog::logger>(std::move(name), sinks.begin(), sinks.end());
    logger->set_level(spdlog::level::from_str(level));
    logger->set_pattern(fmt::format("[%D %H:%M:%S][{}][%L]: %v", name));

    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
  } else {
    log_error("trying to configure logger twice");
  }
}

void log_info(const std::string& msg) {
  logger->info(msg);
}

void log_warn(const std::string& msg) {
  logger->warn(msg);
}

void log_error(const std::string& msg) {
  logger->error(msg);
}

void log_debug(const std::string& msg) {
  logger->debug(msg);
}

}// namespace fys