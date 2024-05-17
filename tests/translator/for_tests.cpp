#include <gtest/gtest.h>

#include "../utils/stringify.h"
#include "../utils/test_utils.h"

TEST(ForTests, ForCanBeUsedAsNormal) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;
                for (let i = 0; i < 24; i++) {
                    a += 2;
                }
                return a;
            }
            );

    const int expected = 48;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(ForTests, ForCanUseMultipleAssignments) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;
                for (let i = 0, let j = 1; i < 24; i++, j *= 2) {
                    a += i * j;
                }
                return a;
            }
            );

    const int expected = 369098754;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(ForTests, ForCanUseOnlyRunOnce) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;
                for (let i = 1; a < 24;) {
                    a += i;
                    a += 1;
                }
                return a;
            }
            );

    const int expected = 24;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(ForTests, ForCanUseOnyAfterEach) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;
                let b = 1;
                for (;a < 34; a++) {
                    b = a;
                }
                return b * a;
            }
            );

    const int expected = 1122;
    auto actual = test_source(source);

    EXPECT_EQ(expected, actual);
}

TEST(ForTest, ForCanBeUsedOnlyWithCondition) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 1;
                let b = 0;
                for (;b < 12;) {
                    b += 1;
                    a *= b;
                }
                return a;
            }
            );

    const int expected = 479001600;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(ForTests, ForLoopCanUseBreak) {
    auto source = STRINGIFY(
            package test;
            func main() int{
            let a = 0;

            for (;true;) {
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

TEST(ForTests, ForLoopCanUseContinue) {
    auto source = STRINGIFY(
            package test;
            func main() int{
            let a = 0;
            for (;true;) {
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

TEST(ForTests, ForLoopCanUseBreakall) {
    auto source = STRINGIFY(
            package test;
            func main() int{
            let a = 0;
            while (true) {
                for (;true;) {
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

TEST(ForTests, ForLoopCanUseBreakInt) {
    auto source = STRINGIFY(
            package test;
            func main() int{
            let a = 0;
            do {
                while (true) {
                    a += 1;

                    if (a == 10) {
                        for (;true;) {
                            a += 1;

                            if (a % 2 == 0) {
                                break
                                2:uint;
                            }
                        }
                    }
                }

                for (;true;) {
                    for (;true;) {
                        for (;true;) {
                            for (;true;) {
                                for (;true;) {
                                    for (;true;) {
                                        a += 1;

                                        if (a > 30) {
                                            break
                                            6:uint;
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

TEST(ForTests, ForLoopCanUseReturn) {
    auto source = STRINGIFY(
            package test;
            func main() int{
            let a = 1;
            for (;true;) {
                return a * 8;
            }
            return a;
    }
    );

    const int expected = 8;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(ForTests, ForLoopCanUseIf) {
    auto source = STRINGIFY(
            package test;
            func main() int{
            let a = 0;
            for (;true;) {
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

TEST(ForTests, ForLoopCanUseWhileLoop) {
    auto source = STRINGIFY(
            package test;
            func main() int{
            let a = 0;
            for (;true;) {
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

TEST(ForTests, ForLoopCanUseDoWhileLoop) {
    auto source = STRINGIFY(
            package test;
            func main() int{
            let a = 0;
            for (;true;) {
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

TEST(ForTests, ForLoopCanUseLoop) {
    auto source = STRINGIFY(
            package test;
            func main() int{
            let a = 0;
            for (;true;) {
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

TEST(ForTests, ForLoopCanUseNestedForLoop) {
    auto source = STRINGIFY(
            package test;
            func main() int{
            let a = 0;
            for (;true;) {
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

TEST(ForTests, ForLoopCanBeUsedInComplexSituations) {
    auto source = STRINGIFY(
            package test;
            func main() int{
            let a = 0;

            if (a >= 0) {
                for (;true;) {
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
                        for (;true;) {
                            while (true) {
                                do {
                                    for (;true;) {
                                        for (let i = 0; i < 10989; i *= 3) {
                                            a += 30;
                                            break
                                            6:uint;
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
