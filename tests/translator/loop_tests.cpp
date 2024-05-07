#include <gtest/gtest.h>

#include <llvm/ExecutionEngine/ExecutionEngine.h>

#include "../../src/snakelangc/lexer/lexer.h"
#include "../../src/snakelangc/parser/parser.h"
#include "../../src/snakelangc/translator/translator.h"
#include "../../src/snakelangc/emitter/emitter.h"

#include <cstdio>

#define STRINGIFY(...) #__VA_ARGS__

int test_source(const std::string& source) {
    std::istringstream string_stream(source);
    lexer::lexer lexer(string_stream.rdbuf());
    parser::parser parser(lexer);

    auto translation_ast = parser.parse();
    std::vector<std::unique_ptr<parser::ast::translation_ast>> translation_asts;
    translation_asts.push_back(std::move(translation_ast));

    emitter::emitter emitter(std::move(translation_asts));
    auto package_ir = emitter.emit();

    translator::translator translator(std::move(package_ir));
    auto module = translator.translate();

    auto main_func = module->getFunction("main");

    llvm::EngineBuilder engine_builder(std::move(module));
    llvm::ExecutionEngine* execution_engine = engine_builder.create();

    return execution_engine->runFunctionAsMain(main_func, {}, {});
}

TEST(LoopTests, LoopCanUseBreak) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;

                loop {
                    a += 1;
                    break;
                }

                return a;
            }
            );

    const int expected = 1;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(LoopTests, LoopCanUseContinue) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;
                loop {
                    if (a < 5) {
                        a += 1;
                        continue;
                    } else if (a > 6) {
                        a += 2;
                    } else {
                        a += 1;
                    }

                    if (a == 9) {
                        break;
                    }
                }
                return a;
            }
            );

    const int expected = 9;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(LoopTests, LoopCanUseBreakall) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;
                while (true) {
                    loop {
                        a += 1;
                        if (a == 10) {
                            breakall;
                        }
                    }
                }
                return a;
            }
            );

    const int expected = 10;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(LoopTests, LoopCanUseIf) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;
                loop {
                    let boolean = a > 5 && 7 & 5 & 3 == 1;

                    if (a > 10 && a % 2 == 0) {
                        break;
                    } else if (boolean) {
                        a += 1;
                    }

                    a += 5 & 3;
                }
                return a;
            }
            );

    const int expected = 12;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(LoopTests, IfCanUseLoop) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;
                if (a == 0 && a != 0) {
                    a = 9;
                } else if (a < 1 || a == 9) {
                    loop {
                        a += 2;
                        if (a == 6) {
                            break;
                        }
                    }
                } else {
                    a = 8;
                }
                return a;
            }
            );

    const int expected = 6;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(LoopTests, LoopCanUseBreakInt) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;
                do {
                    while (true) {
                        a += 1;

                        if (a == 10) {
                            loop {
                                a += 1;

                                if (a % 2 == 0) {
                                    break 2:uint;
                                }
                            }
                        }
                    }

                    loop {
                        loop {
                            loop {
                                loop {
                                    loop {
                                        loop {
                                            a += 1;

                                            if (a > 30) {
                                                break 6:uint;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    a += 5;
                } while (a < 30);
                return a;
            }
            );

    const int expected = 36;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}
