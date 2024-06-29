/*******************************************************************************
 * This file is part of pasta::utils.
 *
 * Copyright (C) 2022 Florian Kurpicz <florian@kurpicz.org>
 *
 * pasta::util is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * pasta::util is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with pasta::util.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#pragma once

#include <fcntl.h>
#include <functional>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace pasta {

/*!
 * \brief Profile any function using perf. Therefore, this requires perf!
 *
 * Loosely based on:
 *   https://muehe.org/posts/profiling-only-parts-of-your-code-with-perf/
 *
 * \tparam ProfiledFunction Usually deducted template used to pass lambda
 * function. \param profile_name Name of the profile file. \param
 * function_to_profile Function that is profiled.
 */
template <typename ProfiledFunction>
static void profile(std::string const& profile_name,
                    ProfiledFunction function_to_profile) {
  std::string const file_name = profile_name.ends_with(".data") ?
                                    profile_name :
                                    (profile_name + std::string(".data"));

  int pid = getpid();
  int cpid = fork();
  if (cpid == 0) {
    std::string command1 = "perf stat -x \";\" -p %d -e "
                           "cache-misses,cache-references,branch-misses,"
                           "branches,instructions,cycles -o ";
    std::vector<char> buffer(command1.size() + file_name.size() +
                             std::to_string(pid).size());
    sprintf(buffer.data(), std::string(command1 + file_name).c_str(), pid);

    execl("/bin/sh", "sh", "-c", buffer.data(), nullptr);
  } else {
    setpgid(cpid, 0);
    sleep(1);
    function_to_profile();

    kill(cpid, SIGINT);
  }
}
} // namespace pasta

/******************************************************************************/
