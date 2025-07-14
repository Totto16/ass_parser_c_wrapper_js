
#include "./convert.hpp"

#include <ass_parser_lib.h>

NAN_METHOD(parse_ass) {

	if(info.Length() != 2) {
		info.GetIsolate()->ThrowException(Nan::TypeError("Wrong number of arguments"));
		return;
	}

	auto source = get_ass_source_from_info(info[0]);

	if(not source.has_value()) {
		info.GetIsolate()->ThrowException(source.error());
		return;
	}

	auto settings = get_parse_settings_from_info(info.GetIsolate(), info[1]);

	if(not settings.has_value()) {
		info.GetIsolate()->ThrowException(settings.error());
		return;
	}

	auto parsed = parse_ass_cpp(source.value(), settings.value());

	auto result = ass_parse_result_to_js(info.GetIsolate(), parsed);

	info.GetReturnValue().Set(result);
}

NAN_MODULE_INIT(InitAll) {
	Nan::Set(target, Nan::New("parse_ass").ToLocalChecked(),
	         Nan::GetFunction(Nan::New<v8::FunctionTemplate>(parse_ass)).ToLocalChecked());

	Nan::Set(target, Nan::New("version").ToLocalChecked(),
	         Nan::New<v8::String>(ass_parser_lib_version()).ToLocalChecked());

	Nan::Set(target, Nan::New("commit_hash").ToLocalChecked(),
	         Nan::New<v8::String>(ass_parser_lib_commit_hash()).ToLocalChecked());
}

NODE_MODULE(AssParserWrapper, InitAll)
