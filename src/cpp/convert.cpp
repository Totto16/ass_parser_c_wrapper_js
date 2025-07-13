

#include "./convert.hpp"

[[nodiscard]] std::expected<AssSourceCpp, v8::Local<v8::Value>>
get_ass_source_from_info(v8::Local<v8::Value> value) {

	if(!value->IsObject()) {
		return std::unexpected{ Nan::TypeError("the 'source' argument needs to be an object") };
	}

	auto object = value->ToObject(Nan::GetCurrentContext()).ToLocalChecked();

	auto type_key = Nan::New<v8::String>("type").ToLocalChecked();

	if(!object->Has(Nan::GetCurrentContext(), type_key).ToChecked()) {
		return std::unexpected{ Nan::TypeError(
			"the 'source' argument needs to have a 'type' key") };
	}

	auto type_value_raw = object->Get(Nan::GetCurrentContext(), type_key).ToLocalChecked();

	if(!type_value_raw->IsString()) {
		return std::unexpected{ Nan::TypeError("source.type needs to be a string") };
	}

	auto type_value = std::string{ *Nan::Utf8String(type_value_raw) };

	if(type_value == "file") {

		auto name_key = Nan::New<v8::String>("name").ToLocalChecked();

		if(!object->Has(Nan::GetCurrentContext(), name_key).ToChecked()) {
			return std::unexpected{ Nan::TypeError(
				"the 'source' argument needs to have a 'name' key, if the type is 'file'") };
		}

		auto name_value_raw = object->Get(Nan::GetCurrentContext(), name_key).ToLocalChecked();

		if(!name_value_raw->IsString()) {
			return std::unexpected{ Nan::TypeError("source.name needs to be a string") };
		}

		auto name_value = std::string{ *Nan::Utf8String(name_value_raw) };

		FileSourceCpp result = { .file = name_value };

		return { result };
	} else if(type_value == "string") {

		auto content_key = Nan::New<v8::String>("content").ToLocalChecked();

		if(!object->Has(Nan::GetCurrentContext(), content_key).ToChecked()) {
			return std::unexpected{ Nan::TypeError(
				"the 'source' argument needs to have a 'content' key, if the type is 'string'") };
		}

		auto content_value_raw =
		    object->Get(Nan::GetCurrentContext(), content_key).ToLocalChecked();

		if(!content_value_raw->IsString()) {
			return std::unexpected{ Nan::TypeError("source.content needs to be a string") };
		}

		auto content_value = std::string{ *Nan::Utf8String(content_value_raw) };

		StringSourceCpp result = { .str = content_value };

		return { result };
	} else {
		return std::unexpected{ Nan::TypeError(
			"source.type needs to be either 'file' or 'string'") };
	}
}

[[nodiscard]] std::expected<ParseSettings, v8::Local<v8::Value>>
get_parse_settings_from_info(v8::Local<v8::Value> value) {}

v8::Local<v8::Value> ass_parse_result_to_js(v8::Isolate* isolate, const AssParseResult* result) {

	return Nan::Null();
}
