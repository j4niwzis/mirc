export module mirc.client;

import std;
export import mirc.core;

namespace mirc {

namespace client {

export struct nick : internal::comparable {
  std::string_view nickname;
};

export struct user : internal::comparable {
  std::string_view username;
  std::string_view realname;
};

export struct pass : internal::comparable {
  std::string_view password;
};

export struct join : internal::comparable {
  std::string_view channel;
  std::string_view key;
};

export struct part : internal::comparable {
  std::string_view channel;
  std::string_view reason;
};

export struct priv_msg : internal::comparable {
  std::string_view target;
  std::string_view text;
};

export struct quit : internal::comparable {
  std::string_view reason;
};

export struct pong : internal::comparable {
  std::string_view server;
};

export struct ping : internal::comparable {
  std::string_view server;
};

export struct mode : internal::comparable {
  std::string_view target;
  std::string_view modes;
  params args;
};

export struct kick : internal::comparable {
  std::string_view channel;
  std::string_view user;
  std::string_view reason;
};

export struct names : internal::comparable {
  std::string_view channel;
};

export struct action : internal::comparable {
  std::string_view target;
  std::string_view text;
};

export struct webirc : internal::comparable {
  std::string_view password;
  std::string_view login;
  std::string_view hostname;
  std::string_view ip_address;
};

}  // namespace client

export using command_t = std::variant<client::nick,
                                      client::user,
                                      client::pass,
                                      client::join,
                                      client::part,
                                      client::priv_msg,
                                      client::quit,
                                      client::pong,
                                      client::ping,
                                      client::mode,
                                      client::kick,
                                      client::names,
                                      client::action,
                                      client::webirc>;

}  // namespace mirc

template <>
struct std::formatter<mirc::client::nick> : formatter_base<"NICK {}"> {};

template <>
struct std::formatter<mirc::client::user> : formatter_base<"USER {} 0 * :{}"> {};

template <>
struct std::formatter<mirc::client::pass> : formatter_base<"PASS {}"> {};

template <>
struct std::formatter<mirc::client::join> : formatter_parse {
  constexpr auto format(const mirc::client::join& cmd, std::format_context& ctx) const {
    return cmd.key.empty() ? std::format_to(ctx.out(), "JOIN {}", cmd.channel)
                           : std::format_to(ctx.out(), "JOIN {} {}", cmd.channel, cmd.key);
  }
};

template <>
struct std::formatter<mirc::client::part> : formatter_parse {
  constexpr auto format(const mirc::client::part& cmd, std::format_context& ctx) const {
    return cmd.reason.empty() ? std::format_to(ctx.out(), "PART {}", cmd.channel)
                              : std::format_to(ctx.out(), "PART {} :{}", cmd.channel, cmd.reason);
  }
};

template <>
struct std::formatter<mirc::client::priv_msg> : formatter_base<"PRIVMSG {} :{}"> {};

template <>
struct std::formatter<mirc::client::quit> : formatter_parse {
  constexpr auto format(const mirc::client::quit& cmd, std::format_context& ctx) const {
    return cmd.reason.empty() ? std::format_to(ctx.out(), "QUIT") : std::format_to(ctx.out(), "QUIT :{}", cmd.reason);
  }
};

template <>
struct std::formatter<mirc::client::pong> : formatter_base<"PONG {}"> {};

template <>
struct std::formatter<mirc::client::ping> : formatter_base<"PING {}"> {};

template <>
struct std::formatter<mirc::client::mode> : formatter_parse {
  constexpr auto format(const mirc::client::mode& cmd, std::format_context& ctx) const {
    auto out = std::format_to(ctx.out(), "MODE {} {}", cmd.target, cmd.modes);
    for(auto arg : cmd.args)
      out = std::format_to(out, " {}", arg);
    return out;
  }
};

template <>
struct std::formatter<mirc::client::kick> : formatter_parse {
  constexpr auto format(const mirc::client::kick& cmd, std::format_context& ctx) const {
    return cmd.reason.empty() ? std::format_to(ctx.out(), "KICK {} {}", cmd.channel, cmd.user)
                              : std::format_to(ctx.out(), "KICK {} {} :{}", cmd.channel, cmd.user, cmd.reason);
  }
};

template <>
struct std::formatter<mirc::client::names> : formatter_base<"NAMES {}"> {};

template <>
struct std::formatter<mirc::client::action> : formatter_base<"PRIVMSG {} :\001ACTION {}\001"> {};

template <>
struct std::formatter<mirc::client::webirc> : formatter_base<"WEBIRC {} {} {} {}"> {};
