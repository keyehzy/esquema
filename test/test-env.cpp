#include "esquema/expr.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(test_env, initialize) {
  {
    Env env;
    env.add(Atom("foo"_sv), Expr::nil());
    EXPECT_EQ(env.get(0)->entry.key, Atom("foo"_sv));
    EXPECT_EQ(env.get(0)->entry.value.kind(), Expr_kind::nil);
  }

  {
    Env env;
    env.add(Atom("foo"_sv), Expr::nil());
    env.add(Atom("bar"_sv), Expr::nil());
    EXPECT_EQ(env.get(0)->entry.key, Atom("foo"_sv));
    EXPECT_EQ(env.get(0)->entry.value.kind(), Expr_kind::nil);
    EXPECT_EQ(env.get(1)->entry.key, Atom("bar"_sv));
    EXPECT_EQ(env.get(1)->entry.value.kind(), Expr_kind::nil);
  }

  {
    Env env;
    env.add(Atom("foo"_sv), Expr::nil());
    env.add(Atom("bar"_sv), Expr::nil());
    env.add(Atom("baz"_sv), Expr::nil());

    EXPECT_EQ(env.get(0)->entry.key, Atom("foo"_sv));
    EXPECT_EQ(env.get(0)->entry.value.kind(), Expr_kind::nil);

    EXPECT_EQ(env.get(1)->entry.key, Atom("bar"_sv));
    EXPECT_EQ(env.get(1)->entry.value.kind(), Expr_kind::nil);

    EXPECT_EQ(env.get(2)->entry.key, Atom("baz"_sv));
    EXPECT_EQ(env.get(2)->entry.value.kind(), Expr_kind::nil);
  }
}

TEST(test_env, find) {
  {
    Env env;
    env.add(Atom("foo"_sv), Expr::nil());
    auto* node = env.find(Atom("foo"_sv));
    EXPECT_TRUE(node != nullptr);
  }

  {
    Env env;
    env.add(Atom("foo"_sv), Expr::nil());
    auto* node = env.find(Atom("foo"_sv));
    EXPECT_TRUE(node != nullptr);
    node->entry.value = Expr("bar"_sv);
    EXPECT_EQ(env.get(0)->entry.key, Atom("foo"_sv));
    EXPECT_EQ(env.get(0)->entry.value.atom(), Atom("bar"_sv));
  }

  {
    Env env;
    env.add(Atom("foo"_sv), Expr::nil());
    env.add(Atom("bar"_sv), Expr::nil());
    env.add(Atom("baz"_sv), Expr::nil());
    auto* node = env.find(Atom("bar"_sv));
    EXPECT_TRUE(node != nullptr);
    node->entry.value = Expr("hola"_sv);
    EXPECT_EQ(env.get(0)->entry.key, Atom("foo"_sv));
    EXPECT_EQ(env.get(1)->entry.key, Atom("bar"_sv));
    EXPECT_EQ(env.get(1)->entry.value.atom(), Atom("hola"_sv));
    EXPECT_EQ(env.get(2)->entry.key, Atom("baz"_sv));
  }
  {
    Env env1, env2, env3;
    std::vector<Env*> envs;
    envs.push_back(&env1);
    envs.back()->add(Atom("foo"_sv), Expr::nil());
    envs.push_back(&env2);
    envs.back()->add(Atom("bar"_sv), Expr::nil());
    envs.push_back(&env3);
    envs.back()->add(Atom("baz"_sv), Expr::nil());

    EXPECT_EQ(envs[0]->get(0)->entry.key, Atom("foo"_sv));
    EXPECT_EQ(envs[1]->get(0)->entry.key, Atom("bar"_sv));
    EXPECT_EQ(envs[2]->get(0)->entry.key, Atom("baz"_sv));
  }

  {
    Env env1, env2, env3;
    std::vector<Env*> envs;
    envs.push_back(&env1);
    envs.back()->add(Atom("foo"_sv), Expr::nil());
    envs.push_back(&env2);
    envs.back()->add(Atom("bar"_sv), Expr::nil());
    envs.push_back(&env3);
    envs.back()->add(Atom("baz"_sv), Expr::nil());

    EXPECT_EQ(envs[0]->get(0)->entry.key, Atom("foo"_sv));
    EXPECT_EQ(envs[1]->get(0)->entry.key, Atom("bar"_sv));
    EXPECT_EQ(envs[2]->get(0)->entry.key, Atom("baz"_sv));
  }
}
