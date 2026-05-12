export module mirc.event;

import std;
export import mirc.core;

namespace mirc {

export constexpr auto to_string_view = [](auto&& r) {
  auto first = std::ranges::begin(r);
  if(first == std::ranges::end(r))
    return std::string_view{};
  return std::string_view{std::addressof(*first), static_cast<std::size_t>(std::ranges::distance(r))};
};

namespace event {

export struct ping : internal::comparable {
  std::string_view server;
};

export struct pong : internal::comparable {
  std::string_view server;
};

export struct priv_msg : internal::comparable {
  prefix prefix;
  std::string_view target;
  std::string_view text;
};

export struct join : internal::comparable {
  prefix prefix;
  std::string_view channel;
};

export struct part : internal::comparable {
  prefix prefix;
  std::string_view channel;
  std::string_view reason;
};

export struct quit : internal::comparable {
  prefix prefix;
  std::string_view reason;
};

export struct nick : internal::comparable {
  prefix prefix;
  std::string_view new_nick;
};

export struct mode : internal::comparable {
  prefix prefix;
  std::string_view target;
  std::string_view modes;
  params args;
};

export struct kick : internal::comparable {
  prefix prefix;
  std::string_view channel;
  std::string_view user;
  std::string_view reason;
};

export struct action : internal::comparable {
  prefix prefix;
  std::string_view target;
  std::string_view text;
};

namespace numeric {

export struct welcome : internal::comparable {
  static constexpr int code = 1;
  std::string_view target;
  std::string_view text;
};

export struct your_host : internal::comparable {
  static constexpr int code = 2;
  std::string_view target;
  std::string_view text;
};

export struct created : internal::comparable {
  static constexpr int code = 3;
  std::string_view target;
  std::string_view text;
};

export struct my_info : internal::comparable {
  static constexpr int code = 4;
  std::string_view target;
  std::string_view server_name;
  std::string_view version;
  std::string_view user_modes;
  std::string_view channel_modes;
};

export struct isupport : internal::comparable {
  static constexpr int code = 5;
  std::string_view target;
  params tokens;

  auto token_views() const {
    return std::views::iota(std::size_t{0}, tokens.size()) | std::views::transform([this](std::size_t i) {
             return tokens[i];
           });
  }
};

export struct umode_is : internal::comparable {
  static constexpr int code = 221;
  std::string_view target;
  std::string_view modes;
};

export struct luser_client : internal::comparable {
  static constexpr int code = 251;
  std::string_view target;
  std::string_view text;
};

export struct luser_op : internal::comparable {
  static constexpr int code = 252;
  std::string_view target;
  std::string_view count;
  std::string_view text;
};

export struct luser_unknown : internal::comparable {
  static constexpr int code = 253;
  std::string_view target;
  std::string_view count;
  std::string_view text;
};

export struct luser_channels : internal::comparable {
  static constexpr int code = 254;
  std::string_view target;
  std::string_view count;
  std::string_view text;
};

export struct luser_me : internal::comparable {
  static constexpr int code = 255;
  std::string_view target;
  std::string_view text;
};

export struct whois_user : internal::comparable {
  static constexpr int code = 311;
  std::string_view target;
  std::string_view nick;
  std::string_view user;
  std::string_view host;
  std::string_view real_name;
};

export struct whois_server : internal::comparable {
  static constexpr int code = 312;
  std::string_view target;
  std::string_view nick;
  std::string_view server;
  std::string_view server_info;
};

export struct whois_operator : internal::comparable {
  static constexpr int code = 313;
  std::string_view target;
  std::string_view nick;
  std::string_view text;
};

export struct whois_idle : internal::comparable {
  static constexpr int code = 317;
  std::string_view target;
  std::string_view nick;
  std::string_view idle;
  std::string_view signon;
};

export struct end_of_whois : internal::comparable {
  static constexpr int code = 318;
  std::string_view target;
  std::string_view nick;
  std::string_view text;
};

export struct whois_channels : internal::comparable {
  static constexpr int code = 319;
  std::string_view target;
  std::string_view nick;
  std::string_view channels;

  auto channel_views() const {
    return channels | std::views::split(' ') | std::views::transform(to_string_view);
  }
};

export struct channel_mode_is : internal::comparable {
  static constexpr int code = 324;
  std::string_view target;
  std::string_view channel;
  std::string_view modes;
};

export struct no_topic : internal::comparable {
  static constexpr int code = 331;
  std::string_view target;
  std::string_view channel;
  std::string_view text;
};

export struct topic : internal::comparable {
  static constexpr int code = 332;
  std::string_view target;
  std::string_view channel;
  std::string_view topic_text;
};

export struct topic_who_time : internal::comparable {
  static constexpr int code = 333;
  std::string_view target;
  std::string_view channel;
  std::string_view who;
  std::string_view set_at;
};

export struct inviting : internal::comparable {
  static constexpr int code = 341;
  std::string_view target;
  std::string_view nick;
  std::string_view channel;
};

export struct nam_reply : internal::comparable {
  static constexpr int code = 353;
  std::string_view target;
  std::string_view channel_type;
  std::string_view channel;
  std::string_view names;

  auto nick_views() const {
    return names | std::views::split(' ') | std::views::transform(to_string_view);
  }
};

export struct end_of_names : internal::comparable {
  static constexpr int code = 366;
  std::string_view target;
  std::string_view channel;
  std::string_view text;
};

export struct motd : internal::comparable {
  static constexpr int code = 372;
  std::string_view target;
  std::string_view text;
};

export struct motd_start : internal::comparable {
  static constexpr int code = 375;
  std::string_view target;
  std::string_view text;
};

export struct end_of_motd : internal::comparable {
  static constexpr int code = 376;
  std::string_view target;
  std::string_view text;
};

export struct no_such_nick : internal::comparable {
  static constexpr int code = 401;
  std::string_view target;
  std::string_view nick;
  std::string_view text;
};

export struct no_such_channel : internal::comparable {
  static constexpr int code = 403;
  std::string_view target;
  std::string_view channel;
  std::string_view text;
};

export struct nick_name_in_use : internal::comparable {
  static constexpr int code = 433;
  std::string_view target;
  std::string_view nick;
  std::string_view text;
};

export struct unknown : internal::comparable {
  int code;
  std::string_view target;
  std::string_view text;
};

export using variant = std::variant<welcome,
                                    your_host,
                                    created,
                                    my_info,
                                    isupport,
                                    umode_is,
                                    luser_client,
                                    luser_op,
                                    luser_unknown,
                                    luser_channels,
                                    luser_me,
                                    whois_user,
                                    whois_server,
                                    whois_operator,
                                    whois_idle,
                                    end_of_whois,
                                    whois_channels,
                                    channel_mode_is,
                                    no_topic,
                                    topic,
                                    topic_who_time,
                                    inviting,
                                    nam_reply,
                                    end_of_names,
                                    motd,
                                    motd_start,
                                    end_of_motd,
                                    no_such_nick,
                                    no_such_channel,
                                    nick_name_in_use,
                                    unknown>;

}  // namespace numeric

export class numeric_t : public numeric::variant {
 public:
  std::optional<::mirc::prefix> prefix;

  using numeric::variant::variant;

  constexpr numeric_t(std::optional<::mirc::prefix> p, numeric::variant v) : numeric::variant(std::move(v)), prefix(std::move(p)) {
  }

  constexpr int code() const {
    return std::visit(
        [](auto&& v) {
          using T = std::remove_cvref_t<decltype(v)>;
          if constexpr(!std::is_member_pointer_v<decltype(&T::code)>) {
            return T::code;
          } else {
            return v.code;
          }
        },
        static_cast<const numeric::variant&>(*this));
  }

  constexpr std::string_view target() const {
    return std::visit(
        [](auto&& v) {
          if constexpr(requires { v.target; }) {
            return v.target;
          } else {
            return std::string_view{};
          }
        },
        static_cast<const numeric::variant&>(*this));
  }

  constexpr std::string_view text() const {
    return std::visit(
        [](auto&& v) {
          if constexpr(requires { v.text; }) {
            return v.text;
          } else {
            return std::string_view{};
          }
        },
        static_cast<const numeric::variant&>(*this));
  }
};

export constexpr bool operator==(const numeric_t& lhs, const numeric_t& rhs) {
  return lhs.prefix == rhs.prefix && static_cast<const numeric::variant&>(lhs) == static_cast<const numeric::variant&>(rhs);
}

export struct unknown : internal::comparable {
  message message;
};

}  // namespace event

export using event_t = std::variant<event::ping,
                                    event::pong,
                                    event::priv_msg,
                                    event::join,
                                    event::part,
                                    event::quit,
                                    event::nick,
                                    event::mode,
                                    event::kick,
                                    event::action,
                                    event::numeric_t,
                                    event::unknown>;

}  // namespace mirc

template <>
struct std::formatter<mirc::event::ping> : formatter_parse {
  constexpr auto format(const mirc::event::ping& ev, std::format_context& ctx) const {
    return ev.server.empty() ? std::format_to(ctx.out(), "PING") : std::format_to(ctx.out(), "PING :{}", ev.server);
  }
};

template <>
struct std::formatter<mirc::event::pong> : formatter_parse {
  constexpr auto format(const mirc::event::pong& ev, std::format_context& ctx) const {
    return ev.server.empty() ? std::format_to(ctx.out(), "PONG") : std::format_to(ctx.out(), "PONG :{}", ev.server);
  }
};

template <>
struct std::formatter<mirc::event::priv_msg> : formatter_base<"{} PRIVMSG {} :{}"> {};

template <>
struct std::formatter<mirc::event::join> : formatter_base<"{} JOIN {}"> {};

template <>
struct std::formatter<mirc::event::part> : formatter_parse {
  constexpr auto format(const mirc::event::part& ev, std::format_context& ctx) const {
    return ev.reason.empty() ? std::format_to(ctx.out(), "{} PART {}", ev.prefix, ev.channel)
                             : std::format_to(ctx.out(), "{} PART {} :{}", ev.prefix, ev.channel, ev.reason);
  }
};

template <>
struct std::formatter<mirc::event::quit> : formatter_parse {
  constexpr auto format(const mirc::event::quit& ev, std::format_context& ctx) const {
    return ev.reason.empty() ? std::format_to(ctx.out(), "{} QUIT", ev.prefix)
                             : std::format_to(ctx.out(), "{} QUIT :{}", ev.prefix, ev.reason);
  }
};

template <>
struct std::formatter<mirc::event::nick> : formatter_base<"{} NICK {}"> {};

template <>
struct std::formatter<mirc::event::mode> : formatter_parse {
  constexpr auto format(const mirc::event::mode& ev, std::format_context& ctx) const {
    auto out = std::format_to(ctx.out(), "{} MODE {} {}", ev.prefix, ev.target, ev.modes);
    for(auto arg : ev.args)
      out = std::format_to(out, " {}", arg);
    return out;
  }
};

template <>
struct std::formatter<mirc::event::kick> : formatter_parse {
  constexpr auto format(const mirc::event::kick& ev, std::format_context& ctx) const {
    return ev.reason.empty() ? std::format_to(ctx.out(), "{} KICK {} {}", ev.prefix, ev.channel, ev.user)
                             : std::format_to(ctx.out(), "{} KICK {} {} :{}", ev.prefix, ev.channel, ev.user, ev.reason);
  }
};

template <>
struct std::formatter<mirc::event::action> : formatter_base<"{} PRIVMSG {} :\001ACTION {}\001"> {};

template <>
struct std::formatter<mirc::event::numeric::welcome> : formatter_base<"001 {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::your_host> : formatter_base<"002 {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::created> : formatter_base<"003 {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::my_info> : formatter_base<"004 {} {} {} {} {}"> {};

template <>
struct std::formatter<mirc::event::numeric::isupport> : formatter_parse {
  constexpr auto format(const mirc::event::numeric::isupport& ev, std::format_context& ctx) const {
    auto out = std::format_to(ctx.out(), "005 {}", ev.target);
    for(auto tok : ev.tokens)
      out = std::format_to(out, " {}", tok);
    return out;
  }
};

template <>
struct std::formatter<mirc::event::numeric::umode_is> : formatter_base<"221 {} {}"> {};

template <>
struct std::formatter<mirc::event::numeric::luser_client> : formatter_base<"251 {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::luser_op> : formatter_base<"252 {} {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::luser_unknown> : formatter_base<"253 {} {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::luser_channels> : formatter_base<"254 {} {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::luser_me> : formatter_base<"255 {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::whois_user> : formatter_base<"311 {} {} {} {} * :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::whois_server> : formatter_base<"312 {} {} {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::whois_operator> : formatter_base<"313 {} {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::whois_idle> : formatter_base<"317 {} {} {} {}"> {};

template <>
struct std::formatter<mirc::event::numeric::end_of_whois> : formatter_base<"318 {} {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::whois_channels> : formatter_base<"319 {} {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::channel_mode_is> : formatter_base<"324 {} {} {}"> {};

template <>
struct std::formatter<mirc::event::numeric::no_topic> : formatter_base<"331 {} {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::topic> : formatter_base<"332 {} {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::topic_who_time> : formatter_base<"333 {} {} {} {}"> {};

template <>
struct std::formatter<mirc::event::numeric::inviting> : formatter_base<"341 {} {} {}"> {};

template <>
struct std::formatter<mirc::event::numeric::nam_reply> : formatter_base<"353 {} {} {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::end_of_names> : formatter_base<"366 {} {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::motd> : formatter_base<"372 {} :- {}"> {};

template <>
struct std::formatter<mirc::event::numeric::motd_start> : formatter_base<"375 {} :- {}"> {};

template <>
struct std::formatter<mirc::event::numeric::end_of_motd> : formatter_base<"376 {} :- {}"> {};

template <>
struct std::formatter<mirc::event::numeric::no_such_nick> : formatter_base<"401 {} {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::no_such_channel> : formatter_base<"403 {} {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::nick_name_in_use> : formatter_base<"433 {} {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric::unknown> : formatter_base<"{:03d} {} :{}"> {};

template <>
struct std::formatter<mirc::event::numeric_t> : formatter_parse {
  constexpr auto format(const mirc::event::numeric_t& ev, std::format_context& ctx) const {
    auto out = ctx.out();
    if(ev.prefix)
      out = std::format_to(out, "{} ", *ev.prefix);
    return std::visit(
        [&](auto&& v) {
          return std::format_to(out, "{}", v);
        },
        static_cast<const mirc::event::numeric::variant&>(ev));
  }
};

template <>
struct std::formatter<mirc::event::unknown> : formatter_base<"{}"> {};
