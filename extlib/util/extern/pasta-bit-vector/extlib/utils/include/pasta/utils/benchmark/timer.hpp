/*******************************************************************************
 * This file is part of pasta::utils.
 *
 * Copyright (C) 2021 Florian Kurpicz <florian@kurpicz.org>
 *
 * pasta::utils is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * pasta::utils is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with pasta::utils.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#pragma once

#include <chrono>
#include <thread>

namespace pasta {

//! \addtogroup pasta_util
//! \{

/*!
 * \brief Utility wrapper for chrono timer.
 *
 * This small timer utility can be used to measure running times. It provided
 * more functionality than simply computing two \c time_points.
 */
class Timer {
  //! Time when the timer was created or reset.
  std::chrono::system_clock::time_point begin_;

public:
  //! Constructor. Creates and starts a timer.
  Timer() : begin_(std::chrono::system_clock::now()) {}

  //! Resets the timer, i.e., sets \c begin_ to the current time.
  void reset() {
    begin_ = std::chrono::system_clock::now();
  }

  /*!
   * \brief Computes duration of timer up to this point.
   * \return The duration the timer has been running in milliseconds.
   */
  [[nodiscard]] size_t get() const {
    auto const end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin_)
        .count();
  }

  /*!
   * \brief Computes duration of timer up to this point and calls \c reset.
   * \return The duration the timer has been running in milliseconds.
   */
  [[nodiscard]] size_t get_and_reset() {
    auto const time = get();
    reset();
    return time;
  }

  /*!
   * \brief Computes the duration of time up to this point, sleeps
   * for a specified time and calls \ref reset().
   * \param seconds_to_sleep Time in seconds the timer sleeps before
   * returning the result.
   * \return The duration the timer has been running in milliseconds.
   */
  [[nodiscard]] size_t get_and_sleep_and_reset(size_t seconds_to_sleep) {
    auto const time = get();
    std::this_thread::sleep_for(std::chrono::seconds(seconds_to_sleep));
    reset();
    return time;
  }

}; // class Timer

//! \)

} // namespace pasta

/******************************************************************************/
