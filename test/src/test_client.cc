#include <gtest/gtest.h>
import std;
import mirc;

using namespace mirc;
using namespace std::string_view_literals;

TEST(IrcClientCommand, NickFormat) {
  EXPECT_EQ(std::format("{}", client::nick{.nickname = "test"}), "NICK test");
}

TEST(IrcClientCommand, UserFormat) {
  EXPECT_EQ(std::format("{}", client::user{.username = "user", .realname = "Real Name"}), "USER user 0 * :Real Name");
}

TEST(IrcClientCommand, PassFormat) {
  EXPECT_EQ(std::format("{}", client::pass{.password = "secret"}), "PASS secret");
}

TEST(IrcClientCommand, JoinFormat) {
  EXPECT_EQ(std::format("{}", client::join{.channel = "#test"}), "JOIN #test");
}

TEST(IrcClientCommand, JoinWithKeyFormat) {
  EXPECT_EQ(std::format("{}", client::join{.channel = "#test", .key = "key"}), "JOIN #test key");
}

TEST(IrcClientCommand, PartFormat) {
  EXPECT_EQ(std::format("{}", client::part{.channel = "#test"}), "PART #test");
}

TEST(IrcClientCommand, PartWithReasonFormat) {
  EXPECT_EQ(std::format("{}", client::part{.channel = "#test", .reason = "bye"}), "PART #test :bye");
}

TEST(IrcClientCommand, PrivMsgFormat) {
  EXPECT_EQ(std::format("{}", client::priv_msg{.target = "#test", .text = "hello"}), "PRIVMSG #test :hello");
}

TEST(IrcClientCommand, QuitFormat) {
  EXPECT_EQ(std::format("{}", client::quit{}), "QUIT");
}

TEST(IrcClientCommand, QuitWithReasonFormat) {
  EXPECT_EQ(std::format("{}", client::quit{.reason = "bye"}), "QUIT :bye");
}

TEST(IrcClientCommand, PongFormat) {
  EXPECT_EQ(std::format("{}", client::pong{.server = "server"}), "PONG server");
}

TEST(IrcClientCommand, PingFormat) {
  EXPECT_EQ(std::format("{}", client::ping{.server = "server"}), "PING server");
}

TEST(IrcClientCommand, ModeFormat) {
  EXPECT_EQ(std::format("{}", client::mode{.target = "#test", .modes = "+o"}), "MODE #test +o");
}

TEST(IrcClientCommand, ModeWithArgsFormat) {
  auto cmd = client::mode{.target = "#test", .modes = "+oo"};
  cmd.args.values[0] = "user1";
  cmd.args.values[1] = "user2";
  cmd.args.count = 2;
  EXPECT_EQ(std::format("{}", cmd), "MODE #test +oo user1 user2");
}

TEST(IrcClientCommand, KickFormat) {
  EXPECT_EQ(std::format("{}", client::kick{.channel = "#test", .user = "user"}), "KICK #test user");
}

TEST(IrcClientCommand, KickWithReasonFormat) {
  EXPECT_EQ(std::format("{}", client::kick{.channel = "#test", .user = "user", .reason = "bye"}), "KICK #test user :bye");
}

TEST(IrcClientCommand, NickEquality) {
  client::nick a{.nickname = "a"};
  client::nick b{.nickname = "a"};
  client::nick c{.nickname = "b"};
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST(IrcClientCommand, UserEquality) {
  client::user a{.username = "u", .realname = "r"};
  client::user b{.username = "u", .realname = "r"};
  client::user c{.username = "u", .realname = "x"};
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST(IrcClientCommand, PassEquality) {
  client::pass a{.password = "p"};
  client::pass b{.password = "p"};
  client::pass c{.password = "q"};
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST(IrcClientCommand, JoinEquality) {
  client::join a{.channel = "#c", .key = "k"};
  client::join b{.channel = "#c", .key = "k"};
  client::join c{.channel = "#c", .key = ""};
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST(IrcClientCommand, PartEquality) {
  client::part a{.channel = "#c", .reason = "r"};
  client::part b{.channel = "#c", .reason = "r"};
  client::part c{.channel = "#c", .reason = ""};
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST(IrcClientCommand, PrivMsgEquality) {
  client::priv_msg a{.target = "#t", .text = "hi"};
  client::priv_msg b{.target = "#t", .text = "hi"};
  client::priv_msg c{.target = "#t", .text = "bye"};
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST(IrcClientCommand, QuitEquality) {
  client::quit a{.reason = "r"};
  client::quit b{.reason = "r"};
  client::quit c{.reason = ""};
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST(IrcClientCommand, PongEquality) {
  client::pong a{.server = "s"};
  client::pong b{.server = "s"};
  client::pong c{.server = "t"};
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST(IrcClientCommand, PingEquality) {
  client::ping a{.server = "s"};
  client::ping b{.server = "s"};
  client::ping c{.server = "t"};
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST(IrcClientCommand, ModeEquality) {
  params args;
  args.values[0] = "a";
  args.count = 1;
  client::mode a{.target = "#t", .modes = "+o", .args = args};
  client::mode b{.target = "#t", .modes = "+o", .args = args};
  client::mode c{.target = "#t", .modes = "-o", .args = args};
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST(IrcClientCommand, KickEquality) {
  client::kick a{.channel = "#c", .user = "u", .reason = "r"};
  client::kick b{.channel = "#c", .user = "u", .reason = "r"};
  client::kick c{.channel = "#c", .user = "u", .reason = ""};
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST(IrcClientCommand, NickEmpty) {
  EXPECT_EQ(std::format("{}", client::nick{.nickname = ""}), "NICK ");
}

TEST(IrcClientCommand, PassEmpty) {
  EXPECT_EQ(std::format("{}", client::pass{.password = ""}), "PASS ");
}

TEST(IrcClientCommand, JoinEmptyKey) {
  EXPECT_EQ(std::format("{}", client::join{.channel = "#c", .key = ""}), "JOIN #c");
}

TEST(IrcClientCommand, ModeEmptyArgs) {
  EXPECT_EQ(std::format("{}", client::mode{.target = "#t", .modes = "+o"}), "MODE #t +o");
}

TEST(IrcClientCommand, PrivMsgEmptyText) {
  EXPECT_EQ(std::format("{}", client::priv_msg{.target = "#t", .text = ""}), "PRIVMSG #t :");
}

TEST(IrcClientCommand, UserEmptyRealname) {
  EXPECT_EQ(std::format("{}", client::user{.username = "u", .realname = ""}), "USER u 0 * :");
}
