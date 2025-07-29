#include "./wrapper.hpp"

AssParseResultCpp::AssParseResultCpp(AssParseResult* c_pointer) : m_c_value{ c_pointer } {}

AssParseResultCpp::~AssParseResultCpp() {
	free_parse_result(m_c_value);
}

[[nodiscard]] Diagnostics AssParseResultCpp::diagnostics() {
	if(m_c_value == nullptr) {
		return Diagnostics{ .entries = STBDS_ARRAY_EMPTY };
	}

	return get_diagnostics_from_result(m_c_value);
}

[[nodiscard]] std::variant<AssParseResultErrorCpp, AssParseResultOkCpp>
AssParseResultCpp::result() {
	if(m_c_value == nullptr) {
		return AssParseResultErrorCpp{};
	}

	if(parse_result_is_error(m_c_value)) {
		return AssParseResultErrorCpp{};
	}

	return AssParseResultOkCpp{ .result = parse_result_get_value(m_c_value) };
}

[[nodiscard]] std::unique_ptr<AssParseResultCpp> parse_ass_cpp(AssSourceCpp source,
                                                               ParseSettings settings) {

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

	auto final_result = std::make_unique<AssParseResultCpp>(result);

	return final_result;
}
