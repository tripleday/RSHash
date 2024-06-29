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

#include "pasta/utils/concepts/alphabet.hpp"

#include <array>
#include <iterator>
#include <limits>
#include <map>
#include <tlx/define.hpp>

namespace pasta {

//! \addtogroup pasta_utils
//! \{

/*!
 * \brief Histogram computation for texts.
 *
 * The histogram contains the number of occurrences of all characters in the
 * text. It can easily be accessed and has a low memory footprint. This
 * implementation is only used for texts that use a datatype that have *less*
 * than 2^16 different characters.
 *
 * \tparam InputIterator Iterator type used to iterate over the text the
 * \c Histogram is computed for.
 */
template <std::forward_iterator InputIterator>
requires SmallAlphabet<std::iter_value_t<InputIterator>>
class Histogram {
  //! Type of symbols in the text.
  using Symbol = std::iter_value_t<InputIterator>;

  //! \c std::array stores all histogram information for small alphabets.
  std::array<size_t, std::numeric_limits<Symbol>::max() + 1> hist_ = {0};

public:
  //! Constructor. Initializing empty histogram.
  Histogram() = default;

  /*!
   * \brief Constructor. Initializing and filling the histogram.
   * \param text Text for which the histogram is computed.
   */
  Histogram(InputIterator begin, InputIterator const end) {
    for (; begin != end; ++begin) {
      ++hist_[*begin];
    }
  }

  /*!
   * \brief Access operator to read/write to an entry of the histogram.
   * \param symbol Symbol for which the histogram entry is returned.
   * \return Number of occurrences of the symbol.
   */
  TLX_ATTRIBUTE_ALWAYS_INLINE size_t operator[](Symbol const symbol) const {
    return hist_[symbol];
  }

  /*!
   * \brief Access operator to read to an entry of the histogram.
   * \param symbol Symbol for which the histogram entry is returned.
   * \return Number of occurrences of the symbol.
   */
  TLX_ATTRIBUTE_ALWAYS_INLINE size_t& operator[](Symbol const symbol) {
    return hist_[symbol];
  }

  /*!
   * \brief Access to the underlying member storing the histogram data.
   * \return Array containing plain histogram data.
   */
  auto plain_histogram() const {
    return hist_;
  }

  /*!
   * \brief Iterator to iterate through all existing histogram entries.
   * \returns Iterator pointing to the first entry of the histogram.
   */
  auto begin() {
    return hist_.begin();
  }

  /*!
   * \brief Sentinel to iterate through all existing histogram entries.
   * \returns Iterator that can be used as sentinel when iterating through the
   * histogram.
   */
  auto end() {
    return hist_.end();
  }

}; // class Histogram (for small alphabets)

//! \}

} // namespace pasta

/******************************************************************************/
