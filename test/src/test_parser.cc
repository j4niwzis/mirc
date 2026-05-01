#include <gtest/gtest.h>
import std;
import mirc;

using namespace mirc;
using namespace std::string_view_literals;

TEST(IrcParser, EmptyLine) {
  EXPECT_EQ(parse(""), std::nullopt);
}

TEST(IrcParser, OnlyPrefix) {
  EXPECT_EQ(parse(":server"), std::nullopt);
}

TEST(IrcParser, SimpleCommand) {
  auto msg = parse("PING");
  ASSERT_NE(msg, std::nullopt);
  EXPECT_EQ(msg->command, "PING");
  EXPECT_EQ(msg->prefix, std::nullopt);
  EXPECT_EQ(msg->params.size(), 0);
}

TEST(IrcParser, CommandWithTrailingCr) {
  auto msg = parse("PING\r");
  ASSERT_NE(msg, std::nullopt);
  EXPECT_EQ(msg->command, "PING");
}

TEST(IrcParser, PrefixOnlyNick) {
  auto msg = parse(":nick PING");
  ASSERT_NE(msg, std::nullopt);
  ASSERT_NE(msg->prefix, std::nullopt);
  EXPECT_EQ(msg->prefix->raw, "nick");
  EXPECT_EQ(msg->prefix->nick, "nick");
  EXPECT_EQ(msg->prefix->user, "");
  EXPECT_EQ(msg->prefix->host, "");
}

TEST(IrcParser, PrefixNickUserHost) {
  auto msg = parse(":nick!user@host PING");
  ASSERT_NE(msg, std::nullopt);
  ASSERT_NE(msg->prefix, std::nullopt);
  EXPECT_EQ(msg->prefix->raw, "nick!user@host");
  EXPECT_EQ(msg->prefix->nick, "nick");
  EXPECT_EQ(msg->prefix->user, "user");
  EXPECT_EQ(msg->prefix->host, "host");
}

TEST(IrcParser, PrefixNickHost) {
  auto msg = parse(":nick@host PING");
  ASSERT_NE(msg, std::nullopt);
  ASSERT_NE(msg->prefix, std::nullopt);
  EXPECT_EQ(msg->prefix->nick, "nick");
  EXPECT_EQ(msg->prefix->user, "");
  EXPECT_EQ(msg->prefix->host, "host");
}

TEST(IrcParser, ParamsMiddle) {
  auto msg = parse("JOIN #channel");
  ASSERT_NE(msg, std::nullopt);
  EXPECT_EQ(msg->command, "JOIN");
  ASSERT_EQ(msg->params.size(), 1);
  EXPECT_EQ(msg->params[0], "#channel");
}

TEST(IrcParser, ParamsTrailing) {
  auto msg = parse("PING :server.name");
  ASSERT_NE(msg, std::nullopt);
  EXPECT_EQ(msg->command, "PING");
  ASSERT_EQ(msg->params.size(), 1);
  EXPECT_EQ(msg->params[0], "server.name");
}

TEST(IrcParser, ParamsMiddleAndTrailing) {
  auto msg = parse("PRIVMSG #channel :Hello world");
  ASSERT_NE(msg, std::nullopt);
  EXPECT_EQ(msg->command, "PRIVMSG");
  ASSERT_EQ(msg->params.size(), 2);
  EXPECT_EQ(msg->params[0], "#channel");
  EXPECT_EQ(msg->params[1], "Hello world");
}

TEST(IrcParser, MultipleMiddleParams) {
  auto msg = parse("MODE #channel +o user");
  ASSERT_NE(msg, std::nullopt);
  EXPECT_EQ(msg->command, "MODE");
  ASSERT_EQ(msg->params.size(), 3);
  EXPECT_EQ(msg->params[0], "#channel");
  EXPECT_EQ(msg->params[1], "+o");
  EXPECT_EQ(msg->params[2], "user");
}

TEST(IrcParser, FullMessage) {
  auto msg = parse(":nick!user@host PRIVMSG #channel :Hello world");
  ASSERT_NE(msg, std::nullopt);
  ASSERT_NE(msg->prefix, std::nullopt);
  EXPECT_EQ(msg->prefix->nick, "nick");
  EXPECT_EQ(msg->command, "PRIVMSG");
  ASSERT_EQ(msg->params.size(), 2);
  EXPECT_EQ(msg->params[0], "#channel");
  EXPECT_EQ(msg->params[1], "Hello world");
}

TEST(IrcParser, EmptyTrailing) {
  auto msg = parse("PING :");
  ASSERT_NE(msg, std::nullopt);
  ASSERT_EQ(msg->params.size(), 1);
  EXPECT_EQ(msg->params[0], "");
}

TEST(IrcParser, ParsePrefixNickOnly) {
  auto prefix = parse_prefix("nick");
  EXPECT_EQ(prefix.raw, "nick");
  EXPECT_EQ(prefix.nick, "nick");
  EXPECT_EQ(prefix.user, "");
  EXPECT_EQ(prefix.host, "");
}

TEST(IrcParser, ParsePrefixNickUserHost) {
  auto prefix = parse_prefix("nick!user@host");
  EXPECT_EQ(prefix.raw, "nick!user@host");
  EXPECT_EQ(prefix.nick, "nick");
  EXPECT_EQ(prefix.user, "user");
  EXPECT_EQ(prefix.host, "host");
}

TEST(IrcParser, ParsePrefixNickHost) {
  auto prefix = parse_prefix("nick@host");
  EXPECT_EQ(prefix.raw, "nick@host");
  EXPECT_EQ(prefix.nick, "nick");
  EXPECT_EQ(prefix.user, "");
  EXPECT_EQ(prefix.host, "host");
}

TEST(IrcParser, ParsePrefixNickUser) {
  auto prefix = parse_prefix("nick!user");
  EXPECT_EQ(prefix.raw, "nick!user");
  EXPECT_EQ(prefix.nick, "nick");
  EXPECT_EQ(prefix.user, "user");
  EXPECT_EQ(prefix.host, "");
}

TEST(IrcParser, ParamsIteration) {
  auto msg = parse("MODE #channel +o user");
  ASSERT_NE(msg, std::nullopt);
  auto params = msg->params;
  std::vector<std::string_view> vec(params.begin(), params.end());
  EXPECT_EQ(vec.size(), 3);
  EXPECT_EQ(vec[0], "#channel");
  EXPECT_EQ(vec[1], "+o");
  EXPECT_EQ(vec[2], "user");
}

TEST(IrcParser, CommandStartsWithColon) {
  EXPECT_EQ(parse(":prefix :command"), std::nullopt);
}

TEST(IrcParser, MultipleSpacesBetweenParams) {
  auto msg = parse("MODE  #channel  +o  user");
  ASSERT_NE(msg, std::nullopt);
  ASSERT_EQ(msg->params.size(), 3);
  EXPECT_EQ(msg->params[0], "#channel");
  EXPECT_EQ(msg->params[1], "+o");
  EXPECT_EQ(msg->params[2], "user");
}

TEST(IrcParser, OnlySpaces) {
  EXPECT_EQ(parse("   "), std::nullopt);
}

TEST(IrcParser, ColonOnly) {
  EXPECT_EQ(parse(":"), std::nullopt);
}

TEST(IrcParser, PrefixWithoutCommand) {
  EXPECT_EQ(parse(":prefix "), std::nullopt);
}

TEST(IrcParser, ParamsWithColonInMiddle) {
  auto msg = parse("COMMAND foo:bar");
  ASSERT_NE(msg, std::nullopt);
  ASSERT_EQ(msg->params.size(), 1);
  EXPECT_EQ(msg->params[0], "foo:bar");
}

TEST(IrcParser, TrailingWithSpaces) {
  auto msg = parse("PRIVMSG #channel :Hello  world");
  ASSERT_NE(msg, std::nullopt);
  ASSERT_EQ(msg->params.size(), 2);
  EXPECT_EQ(msg->params[1], "Hello  world");
}

TEST(IrcParser, ManyParams) {
  auto msg = parse("MODE #ch +o a b c d e f g h i j k l m");
  ASSERT_NE(msg, std::nullopt);
  EXPECT_EQ(msg->params.size(), 15);
}

TEST(IrcParser, EmptyPrefixFields) {
  auto prefix = parse_prefix("");
  EXPECT_EQ(prefix.raw, "");
  EXPECT_EQ(prefix.nick, "");
  EXPECT_EQ(prefix.user, "");
  EXPECT_EQ(prefix.host, "");
}

TEST(IrcParser, PrefixWithOnlyAt) {
  auto prefix = parse_prefix("@");
  EXPECT_EQ(prefix.nick, "");
  EXPECT_EQ(prefix.user, "");
  EXPECT_EQ(prefix.host, "");
}

TEST(IrcParser, PrefixWithOnlyExclamation) {
  auto prefix = parse_prefix("!");
  EXPECT_EQ(prefix.nick, "");
  EXPECT_EQ(prefix.user, "");
  EXPECT_EQ(prefix.host, "");
}

TEST(IrcParser, PrefixWithMultipleAt) {
  auto prefix = parse_prefix("nick!user@host@extra");
  EXPECT_EQ(prefix.nick, "nick");
  EXPECT_EQ(prefix.user, "user");
  EXPECT_EQ(prefix.host, "host@extra");
}
