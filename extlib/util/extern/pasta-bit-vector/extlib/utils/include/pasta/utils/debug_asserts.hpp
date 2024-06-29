/*******************************************************************************
 * This file is part of pasta::utils.
 *
 * Copyright (C) 2021 Florian Kurpicz <florian@kurpicz.org>
 *
 * PaStA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PaStA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PaStA.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#pragma once

#include <iostream>
#include <string_view>

#ifdef __has_include
#  if __has_include(<source_location>)
#    include <source_location>
#    define has_source_location 1
#  endif
#endif

namespace pasta {

//! Macro used to define our assertions in debug builds.
#ifdef DEBUG
#  define PASTA_ASSERT(condition, message)                                     \
    pasta::pasta_assert_impl(condition, message);
#else // NDEBUG
#  define PASTA_ASSERT(condition, message)
#endif

/*!
 * \brief Custom assert function, which we can add more functionality to,
 * compared with the default cassert.
 * \param condition Condition that is checked w.r.t. the assertion.
 * \param message Message that is printed if assertion fails.
 * \param location Source location of the assertion, default parameter.
 */
#ifdef has_source_location
void pasta_assert_impl(
    bool const condition,
    std::string_view const message,
    std::source_location const location = std::source_location::current()) {
#else
void pasta_assert_impl(bool const condition, std::string_view const message) {
#endif
  if (!condition) {
#ifdef has_source_location
    std::cerr << "Assertion failed in file: " << location.file_name() << "("
              << location.line() << ":" << location.column() << ") "
              << location.function_name() << "'";
#else
    std::cerr << "Assertion failed in file: " << __FILE__ << "(line "
              << __LINE__ << ") " << __FUNCTION__ << "'";
#endif
    if (message.size() > 0) {
      std::cerr << " with message: " << message;
    }
    std::cerr << std::endl;
    std::abort();
  }
}

} // namespace pasta

/******************************************************************************/
