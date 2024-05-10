#ifndef SERALANG_TEST_UTILS_H
#define SERALANG_TEST_UTILS_H

#include <sstream>

#include "../../src/serac/lexer/lexer.h"
#include "../../src/serac/parser/parser.h"
#include "../../src/serac/translator/translator.h"
#include "../../src/serac/emitter/emitter.h"

#include <llvm/ExecutionEngine/ExecutionEngine.h>

int test_source(const std::string& source);

#endif //SERALANG_TEST_UTILS_H
