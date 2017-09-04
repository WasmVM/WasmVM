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

#include <Store.h>

Store::~Store() {
  // Function
  for (std::vector<FuncInst*>::iterator funcIt = funcs.begin();
       funcIt != funcs.end(); ++funcIt) {
    delete *funcIt;
  }
  // Table
  for (std::vector<TableInst*>::iterator tableIt = tables.begin();
       tableIt != tables.end(); ++tableIt) {
    delete *tableIt;
  }
  // Memory
  for (std::vector<MemInst*>::iterator memIt = mems.begin();
       memIt != mems.end(); ++memIt) {
    delete *memIt;
  }
  // Global
  for (std::vector<GlobalInst*>::iterator globalIt = globals.begin();
       globalIt != globals.end(); ++globalIt) {
    delete *globalIt;
  }
}
