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

#include <cstdint>
#include <tlx/define.hpp>

namespace pasta {

/*!
 * \brief Collection of static functions used for callbacks in malloc/free.
 *
 * The callbacks are used in our implementations of malloc/free to keep track
 * of the allocated/freed memory in the \c MemoryMonitor.
 */
struct MemoryMonitorCallbacks {
  static void (*malloc_callback)(size_t);
  static void (*free_callback)(size_t);
  static void (*uncounted_callback)(void);
} TLX_ATTRIBUTE_PACKED; // struct MemoryMonitorCallbacks

} // namespace pasta

#ifndef _GNU_SOURCE
#  define _GNU_SOURCE
#endif

extern "C" void* __libc_malloc(size_t);
extern "C" void __libc_free(void*);
extern "C" void* __libc_realloc(void*, size_t);

/******************************************************************************/
