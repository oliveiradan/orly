/* <util/error.cc>

   Implements <util/error.h>.

   Copyright 2015 Theoretical Chaos.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <util/error.h>

#include <cassert>
#include <cstring>

#include <iostream>

namespace Util {

const char *Strerror(int errno_value, char *buf, size_t buf_size) {
  assert(buf);
  assert(buf_size);

/* The man page for strerror_r explains all of this ugliness. */
#pragma clang diagnostic ignored "-Wundef"
#if((_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && !_GNU_SOURCE) || __APPLE__
  /* This is the XSI-compliant version of strerror_r(). */
  int err = strerror_r(errno_value, buf, buf_size);

  if(err) {
    /* In the unlikely event that something went wrong, make the buffer
       contain the empty string, in case it would otherwise be left with
       arbitrary junk. */
    buf[0] = '\0';
  }

  return buf;
#else
  /* This is the GNU-specific version of strerror_r().  Its return type is
     'char *'. */
  return strerror_r(errno_value, buf, buf_size);
#endif
}

void Abort(const Base::TCodeLocation &code_location) {
  std::cerr << "aborting at " << code_location << std::endl;
  abort();
}
}  // Util
