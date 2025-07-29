
#pragma once

#include <memory>
#include <string>
#include <variant>

#include <ass_parser_lib.h>

struct FileSourceCpp {
	std::string file;
};

struct StringSourceCpp {
	std::string str;
};

using AssSourceCpp = std::variant<FileSourceCpp, StringSourceCpp>;

using AssParseResultErrorCpp = std::monostate;

struct AssParseResultOkCpp {
	AssResult result;
};

namespace helper {

template <class... Ts> struct Overloaded : Ts... {
	using Ts::operator()...;
};
template <class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;
} // namespace helper

#define UNUSED(v) ((void)(v))

struct AssParseResultCpp {
  private:
	AssParseResult* m_c_value;

  public:
	explicit AssParseResultCpp(AssParseResult* c_pointer);

	AssParseResultCpp operator=(const AssParseResultCpp& c_pointerresult) = delete;
	AssParseResultCpp(const AssParseResultCpp& c_pointerresult) = delete;

	~AssParseResultCpp();

	[[nodiscard]] Diagnostics diagnostics();
	[[nodiscard]] std::variant<AssParseResultErrorCpp, AssParseResultOkCpp> result();
};

[[nodiscard]] std::unique_ptr<AssParseResultCpp> parse_ass_cpp(AssSourceCpp source,
                                                               ParseSettings settings);
