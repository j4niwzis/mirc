export module mirc.parser;

import std;
import ctre;
export import mirc.core;
export import mirc.event;

namespace mirc {

constexpr std::string_view param(const params& p, std::size_t i) {
  return i < p.size() ? p[i] : std::string_view{};
}

constexpr params slice_params(const params& p, std::size_t start) {
  params result;
  if (start >= p.size()) return result;
  auto n = p.size() - start;
  for (std::size_t i = 0; i < n; ++i) {
    result.values[i] = p[i + start];
  }
  result.count = n;
  return result;
}

template <typename T, typename... Args>
constexpr T make_struct(Args&&... args) {
  T obj{};
  auto& [...members] = obj;
  std::tie(members...) = std::forward_as_tuple(std::forward<Args>(args)...);
  return obj;
}

template <typename Event, typename... Args>
constexpr event_t make_numeric(std::optional<prefix> p, Args&&... args) {
  return event::numeric_t{std::move(p), make_struct<Event>(std::forward<Args>(args)...)};
}

export constexpr prefix parse_prefix(std::string_view raw) {
  auto m = ctre::match<"^([^!@]*)(?:!([^@]*))?(?:@(.*))?$">(raw);
  if (!m) {
    return make_struct<prefix>(raw, raw, std::string_view{}, std::string_view{});
  }

  auto nick = m.template get<1>().to_view();
  auto user = m.template get<2>() ? m.template get<2>().to_view() : std::string_view{};
  auto host = m.template get<3>() ? m.template get<3>().to_view() : std::string_view{};

  return make_struct<prefix>(raw, nick, user, host);
}

export constexpr std::optional<message> parse(std::string_view line) {
  if (!line.empty() && line.back() == '\r') {
    line.remove_suffix(1);
  }

  if (line.empty()) {
    return std::nullopt;
  }

  auto m = ctre::match<"^(:([^ ]+) )?([^ :][^ ]*)(.*)$">(line);
  if (!m) {
    return std::nullopt;
  }

  std::optional<prefix> prefix;
  if (auto prefix_match = m.template get<2>()) {
    prefix = parse_prefix(prefix_match.to_view());
  }

  auto command = m.template get<3>().to_view();
  auto rest = m.template get<4>().to_view();

  params params;
  auto pos = rest.find_first_not_of(' ');
  while (pos != std::string_view::npos) {
    if (rest[pos] == ':') {
      params.values[params.count++] = rest.substr(pos + 1);
      break;
    }

    auto next = rest.find(' ', pos);
    if (next == std::string_view::npos) {
      params.values[params.count++] = rest.substr(pos);
      break;
    }

    params.values[params.count++] = rest.substr(pos, next - pos);
    pos = rest.find_first_not_of(' ', next);
  }

  return make_struct<message>(prefix, command, params);
}

export constexpr event_t to_event(const message& msg) {
  auto cmd = msg.command;

  if (cmd == "PING") return make_struct<event::ping>(msg.params[0]);
  if (cmd == "PONG") return make_struct<event::pong>(msg.params[0]);
  if (cmd == "PRIVMSG" && msg.prefix) return make_struct<event::priv_msg>(*msg.prefix, msg.params[0], msg.params[1]);
  if (cmd == "JOIN" && msg.prefix) return make_struct<event::join>(*msg.prefix, msg.params[0]);
  if (cmd == "PART" && msg.prefix) return make_struct<event::part>(*msg.prefix, msg.params[0], msg.params[1]);
  if (cmd == "QUIT" && msg.prefix) return make_struct<event::quit>(*msg.prefix, msg.params[0]);
  if (cmd == "NICK" && msg.prefix) return make_struct<event::nick>(*msg.prefix, msg.params[0]);
  if (cmd == "MODE" && msg.prefix) return make_struct<event::mode>(*msg.prefix, msg.params[0], msg.params[1], slice_params(msg.params, 2));
  if (cmd == "KICK" && msg.prefix) return make_struct<event::kick>(*msg.prefix, msg.params[0], msg.params[1], msg.params[2]);

  if (ctre::match<"^[0-9]{3}$">(cmd)) {
    int code = (cmd[0] - '0') * 100 + (cmd[1] - '0') * 10 + (cmd[2] - '0');
    auto p = msg.prefix;
    auto target = param(msg.params, 0);

    switch (code) {
      case 1: return make_numeric<event::numeric::welcome>(p, target, msg.params[1]);
      case 2: return make_numeric<event::numeric::your_host>(p, target, msg.params[1]);
      case 3: return make_numeric<event::numeric::created>(p, target, msg.params[1]);
      case 4: return make_numeric<event::numeric::my_info>(p, target, msg.params[1], msg.params[2], msg.params[3], msg.params[4]);
      case 5: return make_numeric<event::numeric::isupport>(p, target, slice_params(msg.params, 1));
      case 221: return make_numeric<event::numeric::umode_is>(p, target, msg.params[1]);
      case 251: return make_numeric<event::numeric::luser_client>(p, target, msg.params[1]);
      case 252: return make_numeric<event::numeric::luser_op>(p, target, msg.params[1], msg.params[2]);
      case 253: return make_numeric<event::numeric::luser_unknown>(p, target, msg.params[1], msg.params[2]);
      case 254: return make_numeric<event::numeric::luser_channels>(p, target, msg.params[1], msg.params[2]);
      case 255: return make_numeric<event::numeric::luser_me>(p, target, msg.params[1]);
      case 311: return make_numeric<event::numeric::whois_user>(p, target, msg.params[1], msg.params[2], msg.params[3], msg.params[5]);
      case 312: return make_numeric<event::numeric::whois_server>(p, target, msg.params[1], msg.params[2], msg.params[3]);
      case 313: return make_numeric<event::numeric::whois_operator>(p, target, msg.params[1], msg.params[2]);
      case 317: return make_numeric<event::numeric::whois_idle>(p, target, msg.params[1], msg.params[2], msg.params[3]);
      case 318: return make_numeric<event::numeric::end_of_whois>(p, target, msg.params[1], msg.params[2]);
      case 319: return make_numeric<event::numeric::whois_channels>(p, target, msg.params[1], msg.params[2]);
      case 324: return make_numeric<event::numeric::channel_mode_is>(p, target, msg.params[1], msg.params[2]);
      case 331: return make_numeric<event::numeric::no_topic>(p, target, msg.params[1], msg.params[2]);
      case 332: return make_numeric<event::numeric::topic>(p, target, msg.params[1], msg.params[2]);
      case 333: return make_numeric<event::numeric::topic_who_time>(p, target, msg.params[1], msg.params[2], msg.params[3]);
      case 341: return make_numeric<event::numeric::inviting>(p, target, msg.params[1], msg.params[2]);
      case 353: return make_numeric<event::numeric::nam_reply>(p, target, msg.params[1], msg.params[2], msg.params[3]);
      case 366: return make_numeric<event::numeric::end_of_names>(p, target, msg.params[1], msg.params[2]);
      case 372: return make_numeric<event::numeric::motd>(p, target, msg.params[1]);
      case 375: return make_numeric<event::numeric::motd_start>(p, target, msg.params[1]);
      case 376: return make_numeric<event::numeric::end_of_motd>(p, target, msg.params[1]);
      case 401: return make_numeric<event::numeric::no_such_nick>(p, target, msg.params[1], msg.params[2]);
      case 403: return make_numeric<event::numeric::no_such_channel>(p, target, msg.params[1], msg.params[2]);
      case 433: return make_numeric<event::numeric::nick_name_in_use>(p, target, msg.params[1], msg.params[2]);
    }

    return make_numeric<event::numeric::unknown>(p, code, target, msg.params[1]);
  }

  return make_struct<event::unknown>(msg);
}

export constexpr std::optional<event_t> parse_event(std::string_view line) {
  auto msg = parse(line);
  if (!msg) return std::nullopt;
  return to_event(*msg);
}

}  // namespace mirc
