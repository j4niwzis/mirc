#include <gtest/gtest.h>
import std;
import mirc;

using namespace mirc;
using namespace std::string_view_literals;

TEST(IrcEvent, PingEvent) {
  auto ev = parse_event("PING :server");
  ASSERT_NE(ev, std::nullopt);
  auto* ping = std::get_if<event::ping>(&*ev);
  ASSERT_NE(ping, nullptr);
  EXPECT_EQ(ping->server, "server");
}

TEST(IrcEvent, PongEvent) {
  auto ev = parse_event("PONG :server");
  ASSERT_NE(ev, std::nullopt);
  auto* pong = std::get_if<event::pong>(&*ev);
  ASSERT_NE(pong, nullptr);
  EXPECT_EQ(pong->server, "server");
}

TEST(IrcEvent, PrivMsgEvent) {
  auto ev = parse_event(":nick!user@host PRIVMSG #channel :Hello");
  ASSERT_NE(ev, std::nullopt);
  auto* pm = std::get_if<event::priv_msg>(&*ev);
  ASSERT_NE(pm, nullptr);
  EXPECT_EQ(pm->prefix.nick, "nick");
  EXPECT_EQ(pm->target, "#channel");
  EXPECT_EQ(pm->text, "Hello");
}

TEST(IrcEvent, JoinEvent) {
  auto ev = parse_event(":nick JOIN #channel");
  ASSERT_NE(ev, std::nullopt);
  auto* join = std::get_if<event::join>(&*ev);
  ASSERT_NE(join, nullptr);
  EXPECT_EQ(join->prefix.nick, "nick");
  EXPECT_EQ(join->channel, "#channel");
}

TEST(IrcEvent, PartEvent) {
  auto ev = parse_event(":nick PART #channel :Goodbye");
  ASSERT_NE(ev, std::nullopt);
  auto* part = std::get_if<event::part>(&*ev);
  ASSERT_NE(part, nullptr);
  EXPECT_EQ(part->prefix.nick, "nick");
  EXPECT_EQ(part->channel, "#channel");
  EXPECT_EQ(part->reason, "Goodbye");
}

TEST(IrcEvent, QuitEvent) {
  auto ev = parse_event(":nick QUIT :Bye");
  ASSERT_NE(ev, std::nullopt);
  auto* quit = std::get_if<event::quit>(&*ev);
  ASSERT_NE(quit, nullptr);
  EXPECT_EQ(quit->prefix.nick, "nick");
  EXPECT_EQ(quit->reason, "Bye");
}

TEST(IrcEvent, NickEvent) {
  auto ev = parse_event(":oldnick NICK newnick");
  ASSERT_NE(ev, std::nullopt);
  auto* nick = std::get_if<event::nick>(&*ev);
  ASSERT_NE(nick, nullptr);
  EXPECT_EQ(nick->prefix.nick, "oldnick");
  EXPECT_EQ(nick->new_nick, "newnick");
}

TEST(IrcEvent, ModeEvent) {
  auto ev = parse_event(":nick MODE #channel +o user");
  ASSERT_NE(ev, std::nullopt);
  auto* mode = std::get_if<event::mode>(&*ev);
  ASSERT_NE(mode, nullptr);
  EXPECT_EQ(mode->prefix.nick, "nick");
  EXPECT_EQ(mode->target, "#channel");
  EXPECT_EQ(mode->modes, "+o");
  ASSERT_EQ(mode->args.size(), 1);
  EXPECT_EQ(mode->args[0], "user");
}

TEST(IrcEvent, KickEvent) {
  auto ev = parse_event(":nick KICK #channel user :reason");
  ASSERT_NE(ev, std::nullopt);
  auto* kick = std::get_if<event::kick>(&*ev);
  ASSERT_NE(kick, nullptr);
  EXPECT_EQ(kick->prefix.nick, "nick");
  EXPECT_EQ(kick->channel, "#channel");
  EXPECT_EQ(kick->user, "user");
  EXPECT_EQ(kick->reason, "reason");
}

TEST(IrcEvent, NumericEvent) {
  auto ev = parse_event(":server 001 nick :Welcome");
  ASSERT_NE(ev, std::nullopt);
  auto* num = std::get_if<event::numeric_t>(&*ev);
  ASSERT_NE(num, nullptr);
  ASSERT_NE(num->prefix, std::nullopt);
  EXPECT_EQ(num->prefix->raw, "server");
  EXPECT_EQ(num->code(), 1);
  EXPECT_EQ(num->target(), "nick");
  EXPECT_EQ(num->text(), "Welcome");
  const auto& var = static_cast<const event::numeric::variant&>(*num);
  auto* welcome = std::get_if<event::numeric::welcome>(&var);
  ASSERT_NE(welcome, nullptr);
  EXPECT_EQ(welcome->target, "nick");
  EXPECT_EQ(welcome->text, "Welcome");
}

TEST(IrcEvent, UnknownEvent) {
  auto ev = parse_event("UNKNOWN foo");
  ASSERT_NE(ev, std::nullopt);
  auto* unk = std::get_if<event::unknown>(&*ev);
  ASSERT_NE(unk, nullptr);
  EXPECT_EQ(unk->message.command, "UNKNOWN");
  ASSERT_EQ(unk->message.params.size(), 1);
  EXPECT_EQ(unk->message.params[0], "foo");
}

TEST(IrcEvent, PartWithoutReason) {
  auto ev = parse_event(":nick PART #channel");
  ASSERT_NE(ev, std::nullopt);
  auto* part = std::get_if<event::part>(&*ev);
  ASSERT_NE(part, nullptr);
  EXPECT_EQ(part->reason, "");
}

TEST(IrcEvent, QuitWithoutReason) {
  auto ev = parse_event(":nick QUIT");
  ASSERT_NE(ev, std::nullopt);
  auto* quit = std::get_if<event::quit>(&*ev);
  ASSERT_NE(quit, nullptr);
  EXPECT_EQ(quit->reason, "");
}

TEST(IrcEvent, KickWithoutReason) {
  auto ev = parse_event(":nick KICK #channel user");
  ASSERT_NE(ev, std::nullopt);
  auto* kick = std::get_if<event::kick>(&*ev);
  ASSERT_NE(kick, nullptr);
  EXPECT_EQ(kick->reason, "");
}

TEST(IrcEvent, EventTypeAlias) {
  auto ev = parse_event("PING :server");
  ASSERT_NE(ev, std::nullopt);
  static_assert(std::is_same_v<decltype(ev)::value_type, event_t>);
}

TEST(IrcEvent, PrivMsgWithoutPrefix) {
  auto ev = parse_event("PRIVMSG #channel :Hello");
  ASSERT_NE(ev, std::nullopt);
  auto* unk = std::get_if<event::unknown>(&*ev);
  ASSERT_NE(unk, nullptr);
}

TEST(IrcEvent, JoinWithoutPrefix) {
  auto ev = parse_event("JOIN #channel");
  ASSERT_NE(ev, std::nullopt);
  auto* unk = std::get_if<event::unknown>(&*ev);
  ASSERT_NE(unk, nullptr);
}

TEST(IrcEvent, PartWithoutPrefix) {
  auto ev = parse_event("PART #channel");
  ASSERT_NE(ev, std::nullopt);
  auto* unk = std::get_if<event::unknown>(&*ev);
  ASSERT_NE(unk, nullptr);
}

TEST(IrcEvent, QuitWithoutPrefix) {
  auto ev = parse_event("QUIT :Bye");
  ASSERT_NE(ev, std::nullopt);
  auto* unk = std::get_if<event::unknown>(&*ev);
  ASSERT_NE(unk, nullptr);
}

TEST(IrcEvent, NickWithoutPrefix) {
  auto ev = parse_event("NICK newnick");
  ASSERT_NE(ev, std::nullopt);
  auto* unk = std::get_if<event::unknown>(&*ev);
  ASSERT_NE(unk, nullptr);
}

TEST(IrcEvent, ModeWithoutPrefix) {
  auto ev = parse_event("MODE #channel +o user");
  ASSERT_NE(ev, std::nullopt);
  auto* unk = std::get_if<event::unknown>(&*ev);
  ASSERT_NE(unk, nullptr);
}

TEST(IrcEvent, KickWithoutPrefix) {
  auto ev = parse_event("KICK #channel user");
  ASSERT_NE(ev, std::nullopt);
  auto* unk = std::get_if<event::unknown>(&*ev);
  ASSERT_NE(unk, nullptr);
}

TEST(IrcEvent, NumericWithoutPrefix) {
  auto ev = parse_event("001 nick :Welcome");
  ASSERT_NE(ev, std::nullopt);
  auto* num = std::get_if<event::numeric_t>(&*ev);
  ASSERT_NE(num, nullptr);
  EXPECT_EQ(num->prefix, std::nullopt);
  EXPECT_EQ(num->code(), 1);
  EXPECT_EQ(num->target(), "nick");
  EXPECT_EQ(num->text(), "Welcome");
}

TEST(IrcEvent, ModeWithMultipleArgs) {
  auto ev = parse_event(":nick MODE #channel +oo user1 user2");
  ASSERT_NE(ev, std::nullopt);
  auto* mode = std::get_if<event::mode>(&*ev);
  ASSERT_NE(mode, nullptr);
  EXPECT_EQ(mode->target, "#channel");
  EXPECT_EQ(mode->modes, "+oo");
  ASSERT_EQ(mode->args.size(), 2);
  EXPECT_EQ(mode->args[0], "user1");
  EXPECT_EQ(mode->args[1], "user2");
}

TEST(IrcEvent, PingWithoutServer) {
  auto ev = parse_event("PING");
  ASSERT_NE(ev, std::nullopt);
  auto* ping = std::get_if<event::ping>(&*ev);
  ASSERT_NE(ping, nullptr);
  EXPECT_EQ(ping->server, "");
}

TEST(IrcEvent, PongWithoutServer) {
  auto ev = parse_event("PONG");
  ASSERT_NE(ev, std::nullopt);
  auto* pong = std::get_if<event::pong>(&*ev);
  ASSERT_NE(pong, nullptr);
  EXPECT_EQ(pong->server, "");
}

TEST(IrcEvent, FullMessageWithCr) {
  auto ev = parse_event(":nick!user@host PRIVMSG #channel :Hello\r");
  ASSERT_NE(ev, std::nullopt);
  auto* pm = std::get_if<event::priv_msg>(&*ev);
  ASSERT_NE(pm, nullptr);
  EXPECT_EQ(pm->text, "Hello");
}

TEST(IrcEvent, Numeric999) {
  auto ev = parse_event("999 target :text");
  ASSERT_NE(ev, std::nullopt);
  auto* num = std::get_if<event::numeric_t>(&*ev);
  ASSERT_NE(num, nullptr);
  EXPECT_EQ(num->code(), 999);
  const auto& var = static_cast<const event::numeric::variant&>(*num);
  auto* unk = std::get_if<event::numeric::unknown>(&var);
  ASSERT_NE(unk, nullptr);
  EXPECT_EQ(unk->code, 999);
}

TEST(IrcEvent, NumericWithTargetOnly) {
  auto ev = parse_event("001 target");
  ASSERT_NE(ev, std::nullopt);
  auto* num = std::get_if<event::numeric_t>(&*ev);
  ASSERT_NE(num, nullptr);
  EXPECT_EQ(num->target(), "target");
  EXPECT_EQ(num->text(), "");
  const auto& var = static_cast<const event::numeric::variant&>(*num);
  auto* welcome = std::get_if<event::numeric::welcome>(&var);
  ASSERT_NE(welcome, nullptr);
  EXPECT_EQ(welcome->target, "target");
  EXPECT_EQ(welcome->text, "");
}

TEST(IrcEvent, NotNumericTwoDigits) {
  auto ev = parse_event("99 target");
  ASSERT_NE(ev, std::nullopt);
  auto* unk = std::get_if<event::unknown>(&*ev);
  ASSERT_NE(unk, nullptr);
}

TEST(IrcEvent, NotNumericFourDigits) {
  auto ev = parse_event("1234 target");
  ASSERT_NE(ev, std::nullopt);
  auto* unk = std::get_if<event::unknown>(&*ev);
  ASSERT_NE(unk, nullptr);
}

TEST(IrcEvent, NotNumericLetters) {
  auto ev = parse_event("ABC target");
  ASSERT_NE(ev, std::nullopt);
  auto* unk = std::get_if<event::unknown>(&*ev);
  ASSERT_NE(unk, nullptr);
}

TEST(IrcEvent, NumericWithEmptyText) {
  auto ev = parse_event("002 target :");
  ASSERT_NE(ev, std::nullopt);
  auto* num = std::get_if<event::numeric_t>(&*ev);
  ASSERT_NE(num, nullptr);
  EXPECT_EQ(num->text(), "");
  const auto& var = static_cast<const event::numeric::variant&>(*num);
  auto* host = std::get_if<event::numeric::your_host>(&var);
  ASSERT_NE(host, nullptr);
  EXPECT_EQ(host->text, "");
}

TEST(IrcEvent, NumericZeroCode) {
  auto ev = parse_event("000 target :text");
  ASSERT_NE(ev, std::nullopt);
  auto* num = std::get_if<event::numeric_t>(&*ev);
  ASSERT_NE(num, nullptr);
  EXPECT_EQ(num->code(), 0);
  const auto& var = static_cast<const event::numeric::variant&>(*num);
  auto* unk = std::get_if<event::numeric::unknown>(&var);
  ASSERT_NE(unk, nullptr);
  EXPECT_EQ(unk->code, 0);
}

TEST(IrcEvent, UnknownWithPrefix) {
  auto ev = parse_event(":server UNKNOWN foo bar");
  ASSERT_NE(ev, std::nullopt);
  auto* unk = std::get_if<event::unknown>(&*ev);
  ASSERT_NE(unk, nullptr);
  EXPECT_EQ(unk->message.command, "UNKNOWN");
  ASSERT_EQ(unk->message.params.size(), 2);
}

TEST(IrcEvent, PrivMsgFormatter) {
  auto ev = parse_event(":nick!user@host PRIVMSG #channel :Hello");
  ASSERT_NE(ev, std::nullopt);
  auto* pm = std::get_if<event::priv_msg>(&*ev);
  ASSERT_NE(pm, nullptr);
  EXPECT_EQ(std::format("{}", *pm), ":nick!user@host PRIVMSG #channel :Hello");
}

TEST(IrcEvent, JoinFormatter) {
  auto ev = parse_event(":nick JOIN #channel");
  ASSERT_NE(ev, std::nullopt);
  auto* join = std::get_if<event::join>(&*ev);
  ASSERT_NE(join, nullptr);
  EXPECT_EQ(std::format("{}", *join), ":nick JOIN #channel");
}

TEST(IrcEvent, PartFormatter) {
  auto ev = parse_event(":nick PART #channel :Bye");
  ASSERT_NE(ev, std::nullopt);
  auto* part = std::get_if<event::part>(&*ev);
  ASSERT_NE(part, nullptr);
  EXPECT_EQ(std::format("{}", *part), ":nick PART #channel :Bye");
}

TEST(IrcEvent, PartFormatterWithoutReason) {
  event::part part{.prefix = parse_prefix("nick"), .channel = "#channel", .reason = ""};
  EXPECT_EQ(std::format("{}", part), ":nick PART #channel");
}

TEST(IrcEvent, QuitFormatter) {
  auto ev = parse_event(":nick QUIT :Bye");
  ASSERT_NE(ev, std::nullopt);
  auto* quit = std::get_if<event::quit>(&*ev);
  ASSERT_NE(quit, nullptr);
  EXPECT_EQ(std::format("{}", *quit), ":nick QUIT :Bye");
}

TEST(IrcEvent, QuitFormatterWithoutReason) {
  event::quit quit{.prefix = parse_prefix("nick"), .reason = ""};
  EXPECT_EQ(std::format("{}", quit), ":nick QUIT");
}

TEST(IrcEvent, NickFormatter) {
  auto ev = parse_event(":oldnick NICK newnick");
  ASSERT_NE(ev, std::nullopt);
  auto* nick = std::get_if<event::nick>(&*ev);
  ASSERT_NE(nick, nullptr);
  EXPECT_EQ(std::format("{}", *nick), ":oldnick NICK newnick");
}

TEST(IrcEvent, ModeFormatter) {
  auto ev = parse_event(":nick MODE #channel +o user");
  ASSERT_NE(ev, std::nullopt);
  auto* mode = std::get_if<event::mode>(&*ev);
  ASSERT_NE(mode, nullptr);
  EXPECT_EQ(std::format("{}", *mode), ":nick MODE #channel +o user");
}

TEST(IrcEvent, KickFormatter) {
  auto ev = parse_event(":nick KICK #channel user :reason");
  ASSERT_NE(ev, std::nullopt);
  auto* kick = std::get_if<event::kick>(&*ev);
  ASSERT_NE(kick, nullptr);
  EXPECT_EQ(std::format("{}", *kick), ":nick KICK #channel user :reason");
}

TEST(IrcEvent, KickFormatterWithoutReason) {
  event::kick kick{.prefix = parse_prefix("nick"), .channel = "#channel", .user = "user", .reason = ""};
  EXPECT_EQ(std::format("{}", kick), ":nick KICK #channel user");
}

TEST(IrcEvent, PingFormatter) {
  event::ping ping{.server = "server"};
  EXPECT_EQ(std::format("{}", ping), "PING :server");
}

TEST(IrcEvent, PingFormatterEmptyServer) {
  event::ping ping{.server = ""};
  EXPECT_EQ(std::format("{}", ping), "PING");
}

TEST(IrcEvent, PongFormatter) {
  event::pong pong{.server = "server"};
  EXPECT_EQ(std::format("{}", pong), "PONG :server");
}

TEST(IrcEvent, NumericFormatter) {
  event::numeric_t num{prefix{.raw = "server", .nick = "server", .user = "", .host = ""}, event::numeric::welcome{.target = "nick", .text = "Welcome"}};
  EXPECT_EQ(std::format("{}", num), ":server 001 nick :Welcome");
}

TEST(IrcEvent, NumericFormatterWithoutPrefix) {
  event::numeric_t num{std::nullopt, event::numeric::welcome{.target = "nick", .text = "Welcome"}};
  EXPECT_EQ(std::format("{}", num), "001 nick :Welcome");
}

TEST(IrcEvent, UnknownFormatter) {
  auto ev = parse_event("UNKNOWN foo");
  ASSERT_NE(ev, std::nullopt);
  auto* unk = std::get_if<event::unknown>(&*ev);
  ASSERT_NE(unk, nullptr);
  EXPECT_EQ(std::format("{}", *unk), "UNKNOWN :foo");
}

TEST(IrcEvent, EventEquality) {
  event::ping a{.server = "srv"};
  event::ping b{.server = "srv"};
  event::ping c{.server = "other"};
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST(IrcEvent, NumericEquality) {
  event::numeric_t a{std::nullopt, event::numeric::welcome{.target = "t", .text = "txt"}};
  event::numeric_t b{std::nullopt, event::numeric::welcome{.target = "t", .text = "txt"}};
  event::numeric_t c{prefix{.raw = "srv", .nick = "srv", .user = "", .host = ""}, event::numeric::welcome{.target = "t", .text = "txt"}};
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST(IrcEvent, NamReplyEvent) {
  auto ev = parse_event(":server 353 target = #channel :nick1 @nick2 +nick3");
  ASSERT_NE(ev, std::nullopt);
  auto* num = std::get_if<event::numeric_t>(&*ev);
  ASSERT_NE(num, nullptr);
  EXPECT_EQ(num->code(), 353);
  const auto& var = static_cast<const event::numeric::variant&>(*num);
  auto* nam = std::get_if<event::numeric::nam_reply>(&var);
  ASSERT_NE(nam, nullptr);
  EXPECT_EQ(nam->target, "target");
  EXPECT_EQ(nam->channel_type, "=");
  EXPECT_EQ(nam->channel, "#channel");
  EXPECT_EQ(nam->names, "nick1 @nick2 +nick3");
  std::vector<std::string_view> nicks;
  for (auto nick : nam->nick_views()) {
    nicks.push_back(nick);
  }
  ASSERT_EQ(nicks.size(), 3);
  EXPECT_EQ(nicks[0], "nick1");
  EXPECT_EQ(nicks[1], "@nick2");
  EXPECT_EQ(nicks[2], "+nick3");
}

TEST(IrcEvent, TopicEvent) {
  auto ev = parse_event(":server 332 target #channel :Hello topic");
  ASSERT_NE(ev, std::nullopt);
  auto* num = std::get_if<event::numeric_t>(&*ev);
  ASSERT_NE(num, nullptr);
  EXPECT_EQ(num->code(), 332);
  const auto& var = static_cast<const event::numeric::variant&>(*num);
  auto* topic = std::get_if<event::numeric::topic>(&var);
  ASSERT_NE(topic, nullptr);
  EXPECT_EQ(topic->channel, "#channel");
  EXPECT_EQ(topic->topic_text, "Hello topic");
}

TEST(IrcEvent, WhoisChannelsEvent) {
  auto ev = parse_event(":server 319 target nick :@#chan1 +#chan2 #chan3");
  ASSERT_NE(ev, std::nullopt);
  auto* num = std::get_if<event::numeric_t>(&*ev);
  ASSERT_NE(num, nullptr);
  EXPECT_EQ(num->code(), 319);
  const auto& var = static_cast<const event::numeric::variant&>(*num);
  auto* wc = std::get_if<event::numeric::whois_channels>(&var);
  ASSERT_NE(wc, nullptr);
  std::vector<std::string_view> chans;
  for (auto ch : wc->channel_views()) {
    chans.push_back(ch);
  }
  ASSERT_EQ(chans.size(), 3);
  EXPECT_EQ(chans[0], "@#chan1");
  EXPECT_EQ(chans[1], "+#chan2");
  EXPECT_EQ(chans[2], "#chan3");
}

TEST(IrcEvent, IsupportEvent) {
  auto ev = parse_event(":server 005 target TOKEN1 TOKEN2 :are supported by this server");
  ASSERT_NE(ev, std::nullopt);
  auto* num = std::get_if<event::numeric_t>(&*ev);
  ASSERT_NE(num, nullptr);
  EXPECT_EQ(num->code(), 5);
  const auto& var = static_cast<const event::numeric::variant&>(*num);
  auto* sup = std::get_if<event::numeric::isupport>(&var);
  ASSERT_NE(sup, nullptr);
  EXPECT_EQ(sup->target, "target");
  ASSERT_EQ(sup->tokens.size(), 3);
  EXPECT_EQ(sup->tokens[0], "TOKEN1");
  EXPECT_EQ(sup->tokens[1], "TOKEN2");
  EXPECT_EQ(sup->tokens[2], "are supported by this server");
}

TEST(IrcEvent, UnknownNumericEvent) {
  auto ev = parse_event("999 target :some text");
  ASSERT_NE(ev, std::nullopt);
  auto* num = std::get_if<event::numeric_t>(&*ev);
  ASSERT_NE(num, nullptr);
  EXPECT_EQ(num->code(), 999);
  EXPECT_EQ(num->target(), "target");
  EXPECT_EQ(num->text(), "some text");
  const auto& var = static_cast<const event::numeric::variant&>(*num);
  auto* unk = std::get_if<event::numeric::unknown>(&var);
  ASSERT_NE(unk, nullptr);
}
