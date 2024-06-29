/*******************************************************************************
 * This file is part of pasta::utils.
 *
 * Copyright (C) 2022 Florian Kurpicz <florian@kurpicz.org>
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

#include <type_traits>

namespace pasta {

/*!
 * \brief Deleted utility function to drop reference (end its lifetime). This
 * only works with rvalue references. \tparam T Type of element to be dropped.
 */
template <typename T>
void drop(T&) = delete;

/*!
 * \brief Deleted utility function to drop reference (end its lifetime). This
 * only works with rvalue references. \tparam T Type of element to be dropped.
 */
template <typename T>
void drop(T const&) = delete;

/*!
 * \brief Deleted utility function to drop reference (end its lifetime).
 * \tparam T Type of element to be dropped.
 * \param to_drop Element to be dropped.
 */
template <typename T>
void drop(T&& to_drop) {
  std::remove_reference_t<T>(std::move(to_drop));
}

/*!
 * \brief Deleted utility function to drop reference (end its lifetime). This
 * only works with rvalue references. \tparam T Type of element to be dropped.
 */
template <typename T>
void drop(T const&&) = delete;

} // namespace pasta
