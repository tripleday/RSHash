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

#include "pasta/utils/benchmark/malloc.hpp"

#include <cstddef>
#include <iostream>
#include <ostream>

namespace pasta {

/*!
 * \brief Helper-struct for \c MemoryMonitor.
 *
 * Simple struct that contains all changes in memory usage that are measure by
 * the \c MemoryMonitor.
 */
struct MemoryMonitorStats {
  //! The current memory peak.
  size_t cur_peak;
  //! The sum of all memory that has been allocated.
  size_t total_size_malloc;
  //! The sum of all memory that has been freed.
  size_t total_size_free;
  //! The amount of malloc-calls.
  size_t number_malloc;
  //! The amount of free-calls.
  size_t number_free;
  //! The number of malloc/free-call not tracked by the \c MemoryMonitor.
  size_t uncounted;

  /*!
   * \brief Constructor. Setting all members.
   *
   * \param _cur_peak The current memory peak.
   * \param _total_size_malloc The sum of all memory that has been allocated.
   * \param _total_size_free The sum of all memory that has been freed.
   * \param _number_malloc The amount of malloc-calls.
   * \param _number_free The amount of free-calls.
   * \param _uncounted. The number of malloc/free-call not tracked by the
   * \c MemoryMonitor.
   */
  MemoryMonitorStats(size_t const _cur_peak,
                     size_t const _total_size_malloc,
                     size_t const _total_size_free,
                     size_t const _number_malloc,
                     size_t const _number_free,
                     size_t const _uncounted)
      : cur_peak(_cur_peak),
        total_size_malloc(_total_size_malloc),
        total_size_free(_total_size_free),
        number_malloc(_number_malloc),
        number_free(_number_free),
        uncounted(_uncounted) {}
}; // struct MemoryMonitorStats

//! Simple output of \c MemoryMonitorStats.
std::ostream& operator<<(std::ostream& os, const MemoryMonitorStats& mms) {
  os << "cur_peak=" << mms.cur_peak << " "
     << "total_size_malloc=" << mms.total_size_malloc << " "
     << "total_size_free=" << mms.total_size_free << " "
     << "number_malloc=" << mms.number_malloc << " "
     << "number_free=" << mms.number_free << " "
     << "uncounted=" << mms.uncounted;
  return os;
}

//! \addtogroup pasta_util
//! \{

/*!
 * \brief Utility to monitor memory usage.
 *
 * Singleton that keeps track of memory usage. Can be reseted to keep track of
 * the memory peak during different phases of the program that is monitored.
 */
class MemoryMonitor {
  //! The current state of the tracked memory.
  MemoryMonitorStats memory_stats_;

  //! Private constructor to enable singleton pattern.
  MemoryMonitor() : memory_stats_(0, 0, 0, 0, 0, 0) {
    MemoryMonitorCallbacks::malloc_callback = MemoryMonitor::malloc_callback;
    MemoryMonitorCallbacks::free_callback = MemoryMonitor::free_callback;
    MemoryMonitorCallbacks::uncounted_callback =
        MemoryMonitor::uncounted_callback;
  }

  //! Callback for malloc (used in malloc.cpp).
  static void malloc_callback(size_t const size) {
    instance().memory_stats_.cur_peak += size;
    instance().memory_stats_.total_size_malloc += size;
    ++instance().memory_stats_.number_malloc;
  }

  //! Callback for free (used in malloc.cpp)
  static void free_callback(size_t const size) {
    MemoryMonitorStats& cur_stats = instance().memory_stats_;
    cur_stats.cur_peak -= size;
    cur_stats.total_size_free += size;
    ++cur_stats.number_free;
  }

  //! Callback for malloc/free that are not tracked (which should not occur).
  static void uncounted_callback() {
    ++instance().memory_stats_.uncounted;
  }

public:
  /*!
   * \brief Get the \c MemoryMonitor singleton instance.
   * \return Singleton instance of \c MemoryMonitor.
   */
  static MemoryMonitor& instance() {
    static MemoryMonitor instance;
    return instance;
  }

  //! Reset the current measurement. All measurements are set to zero.
  void reset() {
    instance().memory_stats_ = {0, 0, 0, 0, 0, 0};
  }

  /*!
   * \brief Get the current stats of the monitored memory.
   * \return Current stats of the monitored memory.
   */
  [[nodiscard]] MemoryMonitorStats get() {
    return memory_stats_;
  }

  /*!
   * \brief Get the current stats of the monitored memory. Then reset stats.
   * \return Current stats of the monitored memory.
   */
  [[nodiscard]] MemoryMonitorStats get_and_reset() {
    auto tmp = memory_stats_;
    reset();
    return tmp;
  }
}; // class MemoryMonitor

//! \}

} // namespace pasta

/******************************************************************************/
