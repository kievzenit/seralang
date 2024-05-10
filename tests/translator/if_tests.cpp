#include <gtest/gtest.h>

#include "../utils/stringify.h"
#include "../utils/test_utils.h"

TEST(IfTests, IfCanBeUsedWithoutElseOrElseIf) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;
                if (a == 0) {
                    a = 78;
                }
                return a;
            }
            );

    const int expected = 78;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(IfTests, IfCanBeUsedWithElseAndWithoutElseIf) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;
                if (a != 0) {
                    a = 9;
                } else {
                    a = 5;
                }
                return a;
            }
            );

    const int expected = 5;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(IfTests, IfCanBeUsedWithElseIfWithoutElse) {
    auto source = STRINGIFY(
            package test;
            func main() int {
            let a = 0;
            if (a != 0) {
                a = 9;
            } else if (a >= 0) {
                a = 1;
            }
            return a;
    }
    );

    const int expected = 1;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(IfTests, IfCanBeUsedWithElseIfAndElse) {
    auto source = STRINGIFY(
            package test;
            func main() int {
            let a = 0;
            if (a != 0) {
                a = 9;
            } else if (a > 0) {
                a = 1;
            } else {
                a = 2;
            }
            return a;
    }
    );

    const int expected = 2;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(IfTests, IfCanUseComplexExpressions) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 67;
                if (a < 0 || a > 1 && a < 100 && a / 6 > 5 || a != 67) {
                    a -= 1;
                }
                return a;
            }
            );

    const int expected = 66;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(IfTests, ElseIfCanUseComplexExpressions) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 67;
                if (a != 67) {
                    a -= 1;
                } else if (a < 0 || a >= 7 && a < 100 && a / 2 > 32) {
                    a += 2;
                }
                return a;
        }
        );

    const int expected = 69;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(IfTests, IfCanUseLoop) {
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

TEST(IfTests, IfCanUseWhileLoop) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;
                if (a == 0 && a != 0) {
                    a = 9;
                } else if (a < 1 || a == 9) {
                    while (a <= 9) {
                        a += 2;
                    }
                } else {
                    a = 8;
                }
                return a;
        }
        );

    const int expected = 10;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(IfTests, IfCanUseDoWhileLoop) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;
                if (a == 0 && a != 0) {
                    a = 9;
                } else if (a < 1 || a == 9) {
                    do {
                        a += 2;
                        if (a > 10) {
                            a += 1;
                        }
                    } while (a % 2 == 0);
                } else {
                    a = 8;
                }
                return a;
            }
            );

    const int expected = 13;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(IfTests, IfCanUseForLoop) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;
                if (a == 0 && a != 0) {
                    a = 9;
                } else if (a < 1 || a == 9) {
                    for (let i = 20; a < i; i -=1) {
                        a += 2;
                    }
                } else {
                    a = 8;
                }
                return a;
            }
            );

    const int expected = 14;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}

TEST(IfTests, IfBeUsedInComplexSituations) {
    auto source = STRINGIFY(
            package test;
            func main() int {
                let a = 0;
                if (a == 0 && a != 0) {
                    a = 9;
                } else if (a < 1 || a == 9) {
                    for (let i = 20; a < i; i -=1) {
                        a += 2;
                    }
                } else {
                    a = 8;
                }

                loop {
                    {
                        let b = 14;
                        if (a == 14 && a > 10) {
                            loop {
                                while (a == b) {
                                    a += 7;
                                    if (a == 21) {
                                        do {
                                            a += 1;
                                        } while (a % 2 == 0);

                                        break;
                                    }
                                }

                                loop {
                                    loop {
                                        while (true) {
                                            for (let b = 0; b < 100; b += 1) {
                                                {
                                                    a -= 1;

                                                    {
                                                        if (a == 22) {
                                                            {
                                                                {
                                                                    {
                                                                        break 3:uint;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    if (b < a) {
                                        breakall;
                                    }
                                }
                            }
                        }
                    }
                }
                return a;
            }
            );

    const int expected = 22;
    auto actual = test_source(source);

    EXPECT_EQ(actual, expected);
}