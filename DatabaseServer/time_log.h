#ifndef TIME_LOG_H
#define TIME_LOG_H

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

inline std::string TimeNow() {
  const auto now = std::chrono::system_clock::now();
  const auto now_as_time_t = std::chrono::system_clock::to_time_t(now);
  const auto now_us = std::chrono::duration_cast<std::chrono::milliseconds>(
                          now.time_since_epoch()) %
                      1000000;
  std::stringstream now_ss;
  now_ss << std::put_time(std::localtime(&now_as_time_t), "%a %b %d %Y %T")
         << '.' << std::setfill('0') << std::setw(3) << now_us.count();
  return now_ss.str();
}

#define TIME_LOG (std::cout << TimeNow() << " ")

#endif