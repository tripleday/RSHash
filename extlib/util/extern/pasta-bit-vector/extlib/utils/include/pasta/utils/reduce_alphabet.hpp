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

#include "pasta/utils/histogram.hpp"

#include <iterator>
#include <utility>

namespace pasta {

//! Simpler Type name for internal representation of Histogram.
template <std::forward_iterator InputIterator>
using PlainHist =
    decltype(std::declval<Histogram<InputIterator>>().plain_histogram());

//! \addtogroup pasta_common
//! \{

/*!
 * \brief Transform text to use effective alphabet (preserving lexicographical
 * order)
 *
 * Compute the effective alphabet and map all characters in the text
 * accordingly. Since the ranks of the characters remains the same (in the
 * original alphabet and the new effective alphabet), the lexicographical
 * order also remains the same.
 *
 * \tparam Iterator Iterator type used for to iterate over the text. Must
 * iterate over a \c SmallAlphabet.
 * \param begin Iterator pointing to the first character.
 * \param end Iterator pointing after the last character.
 * \param alphabet_mapping_out Optional pointer to an array which is filled
 * with the mapping to (potentially) reverse the alphabet reduction.
 * \param smallest_symbol The smallest symbol used in the effective alphabet
 * (this allows to reserve some characters as sentinels).
 * \return Size of the alphabet. If there is a smallest symbol, this is
 * smallest symbol plus alphabet size.
 *
 * Reduces the alphabet of the text in the interval [begin, end). The
 * lexicographical order of all symbols remains unchanged. However, the new
 * text is over the effective alphabet.
 */
template <std::forward_iterator Iterator>
requires SmallAlphabet<std::iter_value_t<Iterator>>
    size_t reduce_alphabet(Iterator begin,
                           Iterator end,
                           PlainHist<Iterator>* alphabet_mapping_out = nullptr,
                           size_t smallest_symbol = 0) {
  Histogram hist(begin, end);
  return reduce_alphabet(begin,
                         end,
                         hist,
                         alphabet_mapping_out,
                         smallest_symbol);
}

/*!
 * \brief Transform text to use effective alphabet (preserving lexicographical
 * order) based on a histogram for the text.
 *
 * Compute the effective alphabet and map all characters in the text
 * accordingly. Since the ranks of the characters remains the same (in the
 * original alphabet and the new effective alphabet), the lexicographical
 * order also remains the same.
 *
 * \tparam Iterator Iterator type used for to iterate over the text. Must
 * iterate over a \c SmallAlphabet.
 * \param begin Iterator pointing to the first character.
 * \param end Iterator pointing after the last character.
 * \param histogram Histogram of the text.
 * \param alphabet_mapping_out Optional pointer to an array which is filled
 * with the mapping to (potentially) reverse the alphabet reduction.
 * \param smallest_symbol The smallest symbol used in the effective alphabet
 * (this allows to reserve some characters as sentinels).
 * \return Size of the alphabet. If there is a smallest symbol, this is
 * smallest symbol plus alphabet size.
 *
 * Reduces the alphabet of the text in the interval [begin, end). The
 * lexicographical order of all symbols remains unchanged. However, the new
 * text is over the effective alphabet.
 */
template <std::forward_iterator Iterator>
requires SmallAlphabet<std::iter_value_t<Iterator>>
    size_t reduce_alphabet(Iterator begin,
                           Iterator end,
                           Histogram<Iterator>& histogram,
                           PlainHist<Iterator>* alphabet_mapping_out = nullptr,
                           size_t smallest_symbol = 0) {
  for (auto& occ : histogram) {
    if (occ > 0) {
      occ = smallest_symbol++;
    }
  }
  for (; begin != end; ++begin) {
    *begin = histogram[*begin];
  }
  if (alphabet_mapping_out != nullptr) {
    *alphabet_mapping_out = histogram.plain_histogram();
  }
  return smallest_symbol;
}

//! \}

} // namespace pasta

/******************************************************************************/
