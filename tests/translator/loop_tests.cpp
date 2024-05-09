#include <gtest/gtest.h>

#include "../utils/stringify.h"
#include "../utils/test_utils.h"

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

TEST(LoopTests, LoopCanUseReturn) {
    auto source = STRINGIFY(
            package test;
            func main() int {
            let a = 1;
            loop {
                return a * 8;
            }
            return a;
    }
    );

    const int expected = 8;
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

TEST(LoopTests, LoopCanUseWhileLoop) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;
                loop {
                    while (a < 10) {
                        a += 1;
                    }
                    break;
                }
                return a;
            }
            );

    const int expected = 10;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(LoopTests, LoopCanUseDoWhileLoop) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;
                loop {
                    a += 1;
                    do {
                        a += 2;
                    } while (a % 1 != 0);
                    break;
                }
                return a;
            }
            );

    const int expected = 3;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(LoopTests, LoopCanUseAnotherLoop) {
    auto source = STRINGIFY(
            package test;
            func main() int {
            let a = 0;
            loop {
                loop {
                    a += 2;

                    if (a == 20) {
                        breakall;
                    }
                }
            }
            return a;
    }
    );

    const int expected = 20;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}


TEST(LoopTests, LoopCanUseForLoop) {
    auto source = STRINGIFY(
            package test;
            func main() int {
            let a = 0;
            loop {
                for (let i = 0; i < 24; i += 1) {
                    a += 1;
                }
                break;
            }
            return a;
    }
    );

    const int expected = 24;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(LoopTests, LoopCanBeUsedInComplexSituations) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;

                if (a >= 0) {
                    loop {
                        while (true) {
                            for (let i = 30; a < 14; i -= 1) {
                                a += 1;

                                do {
                                    a += 1;
                                } while (a % 2 != 1);
                            }
                            break;
                        }

                        for (let i = 0; i < 3; i += 1) {
                            a -= 1;
                        }

                        if (a < 100) {
                            loop {
                                while (true) {
                                    do {
                                        loop {
                                            for (let i = 0; i < 10989; i *= 3) {
                                                a += 30;
                                                break 6:uint;
                                            }
                                        }
                                    } while (a > 9 && a < 1000);
                                }
                            }
                        }
                    }
                } else if (a > 1) {
                    return 1;
                }

                if (a > 0) {
                    a -= 15;
                }

                return a;
            }
            );

    const int expected = 27;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}
