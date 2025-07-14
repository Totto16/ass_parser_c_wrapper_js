

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

[[nodiscard]] static std::expected<ScriptInfoStrictSettings, v8::Local<v8::Value>>
get_sscript_info_strict_settings_from_js(v8::Isolate* isolate, v8::Local<v8::Object> object) {

	auto allow_duplicate_fields_key =
	    Nan::New<v8::String>("allow_duplicate_fields").ToLocalChecked();

	if(!object->Has(Nan::GetCurrentContext(), allow_duplicate_fields_key).ToChecked()) {
		return std::unexpected{ Nan::TypeError(
			"the 'script_info' argument needs to have a 'allow_duplicate_fields' key") };
	}

	auto allow_duplicate_fields_value_raw =
	    object->Get(Nan::GetCurrentContext(), allow_duplicate_fields_key).ToLocalChecked();

	if(!allow_duplicate_fields_value_raw->IsBoolean()) {
		return std::unexpected{ Nan::TypeError(
			"script_info.allow_duplicate_fields needs to be a boolean") };
	}

	auto allow_duplicate_fields = allow_duplicate_fields_value_raw->ToBoolean(isolate)->Value();

	auto allow_missing_script_type_key =
	    Nan::New<v8::String>("allow_missing_script_type").ToLocalChecked();

	if(!object->Has(Nan::GetCurrentContext(), allow_missing_script_type_key).ToChecked()) {
		return std::unexpected{ Nan::TypeError(
			"the 'script_info' argument needs to have a 'allow_missing_script_type' key") };
	}

	auto allow_missing_script_type_value_raw =
	    object->Get(Nan::GetCurrentContext(), allow_missing_script_type_key).ToLocalChecked();

	if(!allow_missing_script_type_value_raw->IsBoolean()) {
		return std::unexpected{ Nan::TypeError(
			"script_info.allow_missing_script_type needs to be a boolean") };
	}

	auto allow_missing_script_type =
	    allow_missing_script_type_value_raw->ToBoolean(isolate)->Value();

	ScriptInfoStrictSettings script_info_strict_settings = { .allow_duplicate_fields =
		                                                         allow_duplicate_fields,
		                                                     .allow_missing_script_type =
		                                                         allow_missing_script_type };

	return { script_info_strict_settings };
}

[[nodiscard]] static std::expected<StrictSettings, v8::Local<v8::Value>>
get_strict_settings_from_js(v8::Isolate* isolate, v8::Local<v8::Object> object) {

	auto script_info_key = Nan::New<v8::String>("script_info").ToLocalChecked();

	if(!object->Has(Nan::GetCurrentContext(), script_info_key).ToChecked()) {
		return std::unexpected{ Nan::TypeError(
			"the 'strict_settings' argument needs to have a 'script_info' key") };
	}

	auto script_info_value_raw =
	    object->Get(Nan::GetCurrentContext(), script_info_key).ToLocalChecked();

	if(!script_info_value_raw->IsObject()) {
		return std::unexpected{ Nan::TypeError(
			"strict_settings.script_info needs to be an object") };
	}

	auto script_info_value =
	    script_info_value_raw->ToObject(Nan::GetCurrentContext()).ToLocalChecked();

	auto script_info = get_sscript_info_strict_settings_from_js(isolate, script_info_value);

	if(not script_info.has_value()) {
		return std::unexpected{ script_info.error() };
	}

	auto allow_additional_fields_key =
	    Nan::New<v8::String>("allow_additional_fields").ToLocalChecked();

	if(!object->Has(Nan::GetCurrentContext(), allow_additional_fields_key).ToChecked()) {
		return std::unexpected{ Nan::TypeError(
			"the 'strict_settings' argument needs to have a 'allow_additional_fields' key") };
	}

	auto allow_additional_fields_value_raw =
	    object->Get(Nan::GetCurrentContext(), allow_additional_fields_key).ToLocalChecked();

	if(!allow_additional_fields_value_raw->IsBoolean()) {
		return std::unexpected{ Nan::TypeError(
			"strict_settings.allow_additional_fields needs to be a boolean") };
	}

	auto allow_additional_fields = allow_additional_fields_value_raw->ToBoolean(isolate)->Value();

	auto allow_number_truncating_key =
	    Nan::New<v8::String>("allow_number_truncating").ToLocalChecked();

	if(!object->Has(Nan::GetCurrentContext(), allow_number_truncating_key).ToChecked()) {
		return std::unexpected{ Nan::TypeError(
			"the 'strict_settings' argument needs to have a 'allow_number_truncating' key") };
	}

	auto allow_number_truncating_value_raw =
	    object->Get(Nan::GetCurrentContext(), allow_number_truncating_key).ToLocalChecked();

	if(!allow_number_truncating_value_raw->IsBoolean()) {
		return std::unexpected{ Nan::TypeError(
			"strict_settings.allow_number_truncating needs to be a boolean") };
	}

	auto allow_number_truncating = allow_number_truncating_value_raw->ToBoolean(isolate)->Value();

	auto allow_unrecognized_file_encoding_key =
	    Nan::New<v8::String>("allow_unrecognized_file_encoding").ToLocalChecked();

	if(!object->Has(Nan::GetCurrentContext(), allow_unrecognized_file_encoding_key).ToChecked()) {
		return std::unexpected{ Nan::TypeError("the 'strict_settings' argument needs to have a "
			                                   "'allow_unrecognized_file_encoding' key") };
	}

	auto allow_unrecognized_file_encoding_value_raw =
	    object->Get(Nan::GetCurrentContext(), allow_unrecognized_file_encoding_key)
	        .ToLocalChecked();

	if(!allow_unrecognized_file_encoding_value_raw->IsBoolean()) {
		return std::unexpected{ Nan::TypeError(
			"strict_settings.allow_unrecognized_file_encoding needs to be a boolean") };
	}

	auto allow_unrecognized_file_encoding =
	    allow_unrecognized_file_encoding_value_raw->ToBoolean(isolate)->Value();

	StrictSettings strict_settings = { .script_info = script_info.value(),
		                               .allow_additional_fields = allow_additional_fields,
		                               .allow_number_truncating = allow_number_truncating,
		                               .allow_unrecognized_file_encoding =
		                                   allow_unrecognized_file_encoding };

	return { strict_settings };
}

[[nodiscard]] std::expected<ParseSettings, v8::Local<v8::Value>>
get_parse_settings_from_info(v8::Isolate* isolate, v8::Local<v8::Value> value) {

	if(!value->IsObject()) {
		return std::unexpected{ Nan::TypeError("the 'settings' argument needs to be an object") };
	}

	auto object = value->ToObject(Nan::GetCurrentContext()).ToLocalChecked();

	auto strict_settings_key = Nan::New<v8::String>("strict_settings").ToLocalChecked();

	if(!object->Has(Nan::GetCurrentContext(), strict_settings_key).ToChecked()) {
		return std::unexpected{ Nan::TypeError(
			"the 'settings' argument needs to have a 'strict_settings' key") };
	}

	auto strict_settings_value_raw =
	    object->Get(Nan::GetCurrentContext(), strict_settings_key).ToLocalChecked();

	if(!strict_settings_value_raw->IsObject()) {
		return std::unexpected{ Nan::TypeError("settings.strict_settings needs to be an object") };
	}

	auto strict_settings_value =
	    strict_settings_value_raw->ToObject(Nan::GetCurrentContext()).ToLocalChecked();

	auto strict_settings = get_strict_settings_from_js(isolate, strict_settings_value);

	if(not strict_settings.has_value()) {
		return std::unexpected{ strict_settings.error() };
	}

	ParseSettings settings = { .strict_settings = strict_settings.value() };

	return { settings };
}

[[nodiscard]] static v8::Local<v8::Value> warnings_to_js(v8::Isolate* isolate,
                                                         const Warnings& warnings) {

	// TODO
	return Nan::Null();
}

[[nodiscard]] static v8::Local<v8::Value> ass_result_to_js(v8::Isolate* isolate,
                                                           const AssResult& ass_Result) {

	// TODO
	return Nan::Null();
}

v8::Local<v8::Value> ass_parse_result_to_js(v8::Isolate* isolate, AssParseResultCpp result) {

	v8::Local<v8::Object> object = Nan::New<v8::Object>();

	auto js_warnings = warnings_to_js(isolate, result.warnings());

	std::vector<std::pair<std::string, v8::Local<v8::Value>>> properties_vector{ { "warnings",
		                                                                           js_warnings } };

	std::visit(helper::Overloaded{
	               [&properties_vector](const AssParseResultErrorCpp& result_err) -> void {
		               properties_vector.emplace_back("error", Nan::True());

		               auto message_js = Nan::New<v8::String>(result_err.message).ToLocalChecked();

		               properties_vector.emplace_back("message", message_js);
	               },
	               [&properties_vector, isolate](const AssParseResultOkCpp& result_ok) -> void {
		               properties_vector.emplace_back("error", Nan::False());

		               auto ass_result_js = ass_result_to_js(isolate, result_ok.result);

		               properties_vector.emplace_back("result", ass_result_js);
	               },
	           },
	           result.result());

	for(const auto& [key, value] : properties_vector) {
		v8::Local<v8::String> keyValue = Nan::New<v8::String>(key).ToLocalChecked();
		Nan::Set(object, keyValue, value).Check();
	}

	return object;
}
