
#pragma once

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

struct AssParseResultErrorCpp {
	std::string message;
};

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

	~AssParseResultCpp();

	[[nodiscard]] Warnings warnings();
	[[nodiscard]] std::variant<AssParseResultErrorCpp, AssParseResultOkCpp> result();
};

[[nodiscard]] AssParseResultCpp parse_ass_cpp(AssSourceCpp source, ParseSettings settings);
