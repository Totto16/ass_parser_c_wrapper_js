
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

struct AssParseResultCpp {
	Warnings warnings;
	std::variant<AssParseResultErrorCpp, AssParseResultOkCpp> result;
};

[[nodiscard]] AssParseResultCpp parse_ass_cpp(AssSourceCpp source, ParseSettings settings);
