#include "./wrapper.h"

AssParseResultCpp::AssParseResultCpp(AssParseResult* c_pointer) : m_c_value{ c_pointer } {}

AssParseResultCpp::~AssParseResultCpp() {
	free_parse_result(m_c_value);
}

[[nodiscard]] Warnings AssParseResultCpp::warnings() {
	if(m_c_value == nullptr) {
		return Warnings{ .entries = STBDS_ARRAY_EMPTY };
	}

	return get_warnings_from_result(m_c_value);
}

[[nodiscard]] std::variant<AssParseResultErrorCpp, AssParseResultOkCpp>
AssParseResultCpp::result() {
	if(m_c_value == nullptr) {
		return AssParseResultErrorCpp{ .message = "allocation error" };
	}

	if(parse_result_is_error(m_c_value)) {
		return AssParseResultErrorCpp{ .message = parse_result_get_error(m_c_value) };
	}

	return AssParseResultOkCpp{ .result = parse_result_get_value(m_c_value) };
}

[[nodiscard]] AssParseResultCpp parse_ass_cpp(AssSourceCpp source, ParseSettings settings) {

	AssSourceCpp copy = source;

	AssSource c_source = std::visit(
	    helper::Overloaded{
	        [](const FileSourceCpp& file_source) -> AssSource {
		        return { .type = AssSourceTypeFile, .data = { .file = file_source.file.c_str() } };
	        },
	        [](const StringSourceCpp& string_source) -> AssSource {
		        SizedPtr str = { .data = (void*)string_source.str.c_str(),
			                     .len = string_source.str.size() };
		        return { .type = AssSourceTypeFile, .data = { .str = str } };
	        },
	    },
	    copy);

	auto* result = parse_ass(c_source, settings);

	auto final_result = AssParseResultCpp{ result };

	return final_result;
}
