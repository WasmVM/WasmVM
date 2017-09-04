#ifndef DECODER_DEF
#define DECODER_DEF

#include <iostream>
#include <cstdio>
#include <Instance.h>
#include <Store.h>
#include <Stack.h>
#include <Instruction.h>
#include <Exception.h>
#include <Bincode.h>
#include <Util.h>

class Decoder {
 public:
  static std::vector<char>* funcBody;
  static std::uint32_t* curFuncIdx;
  static void decode(Store& store, Stack& coreStack);
};

#endif