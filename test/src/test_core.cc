#include <gtest/gtest.h>
import std;
import mirc;

using namespace mirc;
using namespace std::string_view_literals;

TEST(IrcCore, PrefixEquality) {
  prefix a{.raw = "nick!user@host", .nick = "nick", .user = "user", .host = "host"};
  prefix b{.raw = "nick!user@host", .nick = "nick", .user = "user", .host = "host"};
  prefix c{.raw = "other", .nick = "other", .user = "", .host = ""};
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST(IrcCore, PrefixSelfEquality) {
  prefix a{.raw = "nick", .nick = "nick", .user = "", .host = ""};
  EXPECT_EQ(a, a);
}

TEST(IrcCore, ParamsDefault) {
  params p;
  EXPECT_EQ(p.size(), 0);
  EXPECT_EQ(p.begin(), p.end());
}

TEST(IrcCore, ParamsIndexing) {
  params p;
  p.values[0] = "a";
  p.values[1] = "b";
  p.count = 2;
  EXPECT_EQ(p[0], "a");
  EXPECT_EQ(p[1], "b");
  EXPECT_EQ(p.size(), 2);
}

TEST(IrcCore, ParamsIteration) {
  params p;
  p.values[0] = "x";
  p.values[1] = "y";
  p.values[2] = "z";
  p.count = 3;
  std::vector<std::string_view> vec(p.begin(), p.end());
  EXPECT_EQ(vec.size(), 3);
  EXPECT_EQ(vec[0], "x");
  EXPECT_EQ(vec[1], "y");
  EXPECT_EQ(vec[2], "z");
}

TEST(IrcCore, MessageEquality) {
  message a{.prefix = std::nullopt, .command = "PING", .params = params{}};
  message b{.prefix = std::nullopt, .command = "PING", .params = params{}};
  message c{.prefix = prefix{.raw = "srv", .nick = "srv", .user = "", .host = ""}, .command = "PONG", .params = params{}};
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST(IrcCore, MessageEqualityWithPrefix) {
  message a{.prefix = prefix{.raw = "n", .nick = "n", .user = "", .host = ""}, .command = "PRIVMSG", .params = params{}};
  message b{.prefix = prefix{.raw = "n", .nick = "n", .user = "", .host = ""}, .command = "PRIVMSG", .params = params{}};
  EXPECT_EQ(a, b);
}

TEST(IrcCore, PrefixFormatter) {
  prefix p{.raw = "nick!user@host", .nick = "nick", .user = "user", .host = "host"};
  EXPECT_EQ(std::format("{}", p), ":nick!user@host");
}

TEST(IrcCore, PrefixFormatterNickOnly) {
  prefix p{.raw = "nick", .nick = "nick", .user = "", .host = ""};
  EXPECT_EQ(std::format("{}", p), ":nick");
}

TEST(IrcCore, MessageFormatterNoPrefix) {
  message msg{.prefix = std::nullopt, .command = "PING", .params = params{}};
  EXPECT_EQ(std::format("{}", msg), "PING");
}

TEST(IrcCore, MessageFormatterWithPrefix) {
  message msg{.prefix = prefix{.raw = "server", .nick = "server", .user = "", .host = ""}, .command = "PONG", .params = params{}};
  EXPECT_EQ(std::format("{}", msg), ":server PONG");
}

TEST(IrcCore, MessageFormatterParams) {
  message msg{.prefix = std::nullopt, .command = "MODE", .params = params{}};
  msg.params.values[0] = "#channel";
  msg.params.values[1] = "+o";
  msg.params.count = 2;
  EXPECT_EQ(std::format("{}", msg), "MODE #channel :+o");
}

TEST(IrcCore, MessageFormatterMultipleParams) {
  message msg{.prefix = std::nullopt, .command = "MODE", .params = params{}};
  msg.params.values[0] = "#channel";
  msg.params.values[1] = "+o";
  msg.params.values[2] = "user";
  msg.params.count = 3;
  EXPECT_EQ(std::format("{}", msg), "MODE #channel +o :user");
}

TEST(IrcCore, MessageFormatterFull) {
  message msg{.prefix = prefix{.raw = "nick!user@host", .nick = "nick", .user = "user", .host = "host"}, .command = "PRIVMSG", .params = params{}};
  msg.params.values[0] = "#channel";
  msg.params.values[1] = "Hello world";
  msg.params.count = 2;
  EXPECT_EQ(std::format("{}", msg), ":nick!user@host PRIVMSG #channel :Hello world");
}

TEST(IrcCore, ComparableThroughDerived) {
  prefix a{.raw = "n", .nick = "n", .user = "", .host = ""};
  prefix b{.raw = "n", .nick = "n", .user = "", .host = ""};
  EXPECT_EQ(a, b);
}

TEST(IrcCore, EmptyParamsFormatter) {
  message msg{.prefix = std::nullopt, .command = "QUIT", .params = params{}};
  EXPECT_EQ(std::format("{}", msg), "QUIT");
}

TEST(IrcCore, ParamsSingleTrailing) {
  message msg{.prefix = std::nullopt, .command = "PING", .params = params{}};
  msg.params.values[0] = "server";
  msg.params.count = 1;
  EXPECT_EQ(std::format("{}", msg), "PING :server");
}
