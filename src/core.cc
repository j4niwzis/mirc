export module mirc.core;

import std;

namespace mirc {

namespace internal {

export struct comparable {
  template <typename self_t>
  constexpr bool operator==(this const self_t& self, const self_t& other) {
    const auto& [... a] = self;
    const auto& [... b] = other;
    return (true && ... && (a == b));
  }

  template <std::same_as<comparable> self_t>
  constexpr bool operator==(this self_t, self_t) {
    return true;
  }
};

}  // namespace internal

export struct prefix : internal::comparable {
  std::string_view raw;
  std::string_view nick;
  std::string_view user;
  std::string_view host;
};

export struct params : internal::comparable {
  std::array<std::string_view, 15> values{};
  std::size_t count = 0;

  constexpr std::string_view operator[](std::size_t i) const {
    return values[i];
  }

  constexpr std::size_t size() const {
    return count;
  }

  constexpr auto begin() const {
    return values.begin();
  }

  constexpr auto end() const {
    return values.begin() + static_cast<std::ptrdiff_t>(count);
  }
};

export struct message : internal::comparable {
  std::optional<prefix> prefix;
  std::string_view command;
  params params;
};

}  // namespace mirc

export template <std::size_t N>
struct static_string {
  char data[N]{};
  constexpr static_string(const char (&s)[N]) {
    for (std::size_t i = 0; i < N; ++i) data[i] = s[i];
  }
};

export struct formatter_parse {
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
};

export template <static_string S>
struct formatter_base : formatter_parse {
  template <typename T>
  constexpr auto format(const T& obj, std::format_context& ctx) const {
    auto [...vs] = obj;
    return std::format_to(ctx.out(), S.data, vs...);
  }
};

template <>
struct std::formatter<mirc::prefix> : formatter_parse {
  constexpr auto format(const mirc::prefix& p, std::format_context& ctx) const {
    return std::format_to(ctx.out(), ":{}", p.raw);
  }
};

template <>
struct std::formatter<mirc::message> : formatter_parse {
  constexpr auto format(const mirc::message& msg, std::format_context& ctx) const {
    auto out = ctx.out();
    if (msg.prefix) out = std::format_to(out, ":{} ", msg.prefix->raw);
    out = std::format_to(out, "{}", msg.command);
    for (std::size_t i = 0; i < msg.params.size(); ++i) {
      if (i + 1 == msg.params.size()) {
        out = std::format_to(out, " :{}", msg.params[i]);
      } else {
        out = std::format_to(out, " {}", msg.params[i]);
      }
    }
    return out;
  }
};
