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

/*
 * based on
 * https://github.com/google/benchmark/blob/main/include/benchmark/benchmark.h
 */

#pragma once

#define PASTA_DO_NOT_OPTIMIZE(value)                                           \
  asm volatile("" : : "r,m"(value) : "memory");

/******************************************************************************/
