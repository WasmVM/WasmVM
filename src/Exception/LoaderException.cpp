/*
Copyright (c) 2017 Luis Hsu

Licensed under the Apache License, Version 2.0 (the "License");you may not use
this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and limitations
under the License.
*/

#include <LoaderException.h>

LoaderException::LoaderException(std::string desc,
                                 bool showAddress,
                                 std::uint64_t addr)
    : desc(desc), address(addr), showAddress(showAddress) {}

std::ostream& operator<<(std::ostream& os, LoaderException& e) {
  os << "Loader error";
  if (e.showAddress) {
    os << " at 0x" << std::setfill('0') << std::setw(8) << std::setbase(16)
       << e.address;
  }
  os << ": " << e.desc;
  return os;
}
