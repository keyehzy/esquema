#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::ElementsAre;
using testing::IsEmpty;

TEST(test_ast, simple) { EXPECT_TRUE(1); }
