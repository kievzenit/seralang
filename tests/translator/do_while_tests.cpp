#include <gtest/gtest.h>

#include "../utils/stringify.h"
#include "../utils/test_utils.h"

TEST(DoWhileTests, DoWhileLoopCanUseBreak) {
    auto source = STRINGIFY(
            package test;
            func main() int {
            let a = 0;

            do {
                a += 1;
                break;
            } while (true);

            return a;
    }
    );

    const int expected = 1;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(DoWhileTests, DoWhileLoopCanUseContinue) {
    auto source = STRINGIFY(
            package test;
            func main() int {
            let a = 0;
            do {
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
            } while (true);
            return a;
    }
    );

    const int expected = 9;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(DoWhileTests, DoWhileLoopCanUseBreakall) {
    auto source = STRINGIFY(
            package test;
            func main() int {
            let a = 0;
            do {
                do {
                    a += 1;
                    if (a == 10) {
                        breakall;
                    }
                } while (true);
            } while (true);
            return a;
    }
    );

    const int expected = 10;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(DoWhileTests, DoWhileLoopCanUseBreakInt) {
    auto source = STRINGIFY(
            package test;
            func main() int {
            let a = 0;
            do {
                a += 1;
                if (a == 10) {
                    do {
                        a += 1;
                        if (a % 2 == 0) {
                            break 2:uint;
                        }
                    } while (true);
                }
                do {
                    do {
                        do {
                            do {
                                do {
                                    a += 1;
                                    if (a > 30) {
                                        break 5:uint;
                                    }
                                } while (true);
                            } while (true);
                        } while (true);
                    } while (true);
                } while (true);
                a += 5;
            } while (a < 30);
            return a;
    }
    );

    const int expected = 36;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(DoWhileTests, DoWhileLoopCanUseReturn) {
    auto source = STRINGIFY(
            package test;
            func main() int {
            let a = 1;
            do {
                return a * 8;
            } while (true);
            return a;
    }
    );

    const int expected = 8;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(DoWhileTests, DoWhileLoopCanUseIf) {
    auto source = STRINGIFY(
            package test;
            func main() int {
            let a = 0;
            do {
                let boolean = a > 5 && 7 & 5 & 3 == 1;

                if (a > 10 && a % 2 == 0) {
                    break;
                } else if (boolean) {
                    a += 1;
                }

                a += 5 & 3;
            } while (true);
            return a;
    }
    );

    const int expected = 12;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(DoWhileTests, DoWhileLoopCanUseWhileLoop) {
    auto source = STRINGIFY(
            package test;
            func main() int {
            let a = 0;
            do {
                while (a < 10) {
                    a += 1;
                }
                break;
            } while (true);
            return a;
    }
    );

    const int expected = 10;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(DoWhileTests, DoWhileLoopCanUseAnotherLoop) {
    auto source = STRINGIFY(
            package test;
            func main() int {
            let a = 0;
            do {
                do {
                    a += 2;
                    if (a == 20) {
                        breakall;
                    }
                } while (true);
            } while (true);
            return a;
    }
    );

    const int expected = 20;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(DoWhileTests, DoWhileLoopCanUseForLoop) {
    auto source = STRINGIFY(
            package test;
            func main() int {
            let a = 0;
            do {
                for (let i = 0; i < 24; i += 1) {
                    a += 1;
                }
                break;
            } while (true);
            return a;
    }
    );

    const int expected = 24;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(DoWhileTests, DoWhileLoopCanBeUsedInComplexSituations) {
    auto source = STRINGIFY(
            package test;
            func main() int {
            let a = 0;

            if (a >= 0) {
                do {
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
                        do {
                            while (true) {
                                do {
                                    while (true) {
                                        for (let i = 0; i < 10989; i *= 3) {
                                            a += 30;
                                            break 6:uint;
                                        }
                                    }
                                } while (a > 9 && a < 1000);
                            }
                        } while (true);
                    }
                } while (true);
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
