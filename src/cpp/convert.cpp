

#include "./convert.hpp"

#include <limits>
#include <stb/ds.h>

// js to c

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

// c to js

// basic conversions

[[nodiscard]] static v8::Local<v8::Value> new_bigint_from_uint64(v8::Isolate* isolate,
                                                                 uint64_t value) {

	return v8::BigInt::NewFromUnsigned(isolate, value);
}

[[nodiscard]] static v8::Local<v8::Value> u64_to_appropiate_number(v8::Isolate* isolate,
                                                                   uint64_t value) {

	if(value > std::numeric_limits<uint32_t>::max()) {
		return new_bigint_from_uint64(isolate, value);
	}
	return Nan::New<v8::Uint32>(static_cast<uint32_t>(value));
}

[[nodiscard]] static v8::Local<v8::Value> u32_to_js(v8::Isolate* isolate, uint32_t value) {

	return Nan::New<v8::Uint32>(static_cast<uint32_t>(value));
}

[[nodiscard]] static v8::Local<v8::Value> size_t_to_js(v8::Isolate* isolate, size_t value) {

	return u64_to_appropiate_number(isolate, value);
}

[[nodiscard]] static v8::Local<v8::Value> final_str_to_js(v8::Isolate* isolate,
                                                          const FinalStr& str) {

	if(str.length == 0) {
		return Nan::EmptyString();
	}

	char* value = get_normalized_string(str);

	auto result = Nan::New<v8::String>(value).ToLocalChecked();

	free(value);

	return result;
}

[[nodiscard]] static v8::Local<v8::Value> bool_to_js(v8::Isolate* isolate, bool value) {

	if(value) {
		return Nan::True();
	}

	return Nan::False();
}

[[nodiscard]] static v8::Local<v8::Value> double_to_js(v8::Isolate* isolate, double value) {
	return Nan::New<v8::Boolean>(value);
}

// complex structs / objects to js

[[nodiscard]] static v8::Local<v8::Value> warning_to_js(v8::Isolate* isolate,
                                                        const WarningEntry& warning) {

	v8::Local<v8::Object> result = Nan::New<v8::Object>();

	ErrorStruct message = get_warnings_message_from_entry(warning);

	auto js_message = Nan::New<v8::String>(message.message).ToLocalChecked();

	free_error_struct(message);

	v8::Local<v8::String> message_key = Nan::New<v8::String>("message").ToLocalChecked();
	Nan::Set(result, message_key, js_message).Check();

	return result;
}

[[nodiscard]] static v8::Local<v8::Value> warnings_to_js(v8::Isolate* isolate,
                                                         const Warnings& warnings) {

	v8::Local<v8::Array> array = v8::Array::New(isolate);

	for(size_t i = 0; i < stbds_arrlenu(warnings.entries); ++i) {
		WarningEntry warning = warnings.entries[i];

		Nan::Set(array, i, warning_to_js(isolate, warning));
	}

	return array;
}

[[nodiscard]] static v8::Local<v8::Value> border_style_to_js(v8::Isolate* isolate,
                                                             const BorderStyle& style) {
	return Nan::New<v8::Uint32>(static_cast<uint32_t>(style));
}

[[nodiscard]] static v8::Local<v8::Value> alignment_to_js(v8::Isolate* isolate,
                                                          const AssAlignment& alignment) {
	return Nan::New<v8::Uint32>(static_cast<uint32_t>(alignment));
}

[[nodiscard]] static v8::Local<v8::Value> ass_color_to_js(v8::Isolate* isolate,
                                                          const AssColor& color) {

	v8::Local<v8::Object> result = Nan::New<v8::Object>();

	auto js_r = u32_to_js(isolate, color.r);

	auto js_g = u32_to_js(isolate, color.g);

	auto js_b = u32_to_js(isolate, color.b);

	auto js_a = u32_to_js(isolate, color.a);

	std::vector<std::pair<std::string, v8::Local<v8::Value>>> properties_vector{
		{ "r", js_r },
		{ "g", js_g },
		{ "b", js_b },
		{ "a", js_a },
	};

	for(const auto& [key, value] : properties_vector) {
		v8::Local<v8::String> keyValue = Nan::New<v8::String>(key).ToLocalChecked();
		Nan::Set(result, keyValue, value).Check();
	}

	return result;
}

[[nodiscard]] static v8::Local<v8::Value> style_to_js(v8::Isolate* isolate,
                                                      const AssStyleEntry& style) {

	v8::Local<v8::Object> result = Nan::New<v8::Object>();

	auto js_name = final_str_to_js(isolate, style.name);

	auto js_fontname = final_str_to_js(isolate, style.fontname);

	auto js_fontsize = size_t_to_js(isolate, style.fontsize);

	auto js_primary_colour = ass_color_to_js(isolate, style.primary_colour);

	auto js_secondary_colour = ass_color_to_js(isolate, style.secondary_colour);

	auto js_outline_colour = ass_color_to_js(isolate, style.outline_colour);

	auto js_back_colour = ass_color_to_js(isolate, style.back_colour);

	auto js_bold = bool_to_js(isolate, style.bold);

	auto js_italic = bool_to_js(isolate, style.italic);

	auto js_underline = bool_to_js(isolate, style.underline);

	auto js_strike_out = bool_to_js(isolate, style.strike_out);

	auto js_scale_x = size_t_to_js(isolate, style.scale_x);

	auto js_scale_y = size_t_to_js(isolate, style.scale_y);

	auto js_spacing = double_to_js(isolate, style.spacing);

	auto js_angle = double_to_js(isolate, style.angle);

	auto js_border_style = border_style_to_js(isolate, style.border_style);

	auto js_outline = double_to_js(isolate, style.outline);

	auto js_shadow = double_to_js(isolate, style.shadow);

	auto js_alignment = alignment_to_js(isolate, style.alignment);

	auto js_margin_l = size_t_to_js(isolate, style.margin_l);

	auto js_margin_r = size_t_to_js(isolate, style.margin_r);

	auto js_margin_v = size_t_to_js(isolate, style.margin_v);

	auto js_encoding = size_t_to_js(isolate, style.encoding);

	std::vector<std::pair<std::string, v8::Local<v8::Value>>> properties_vector{
		{ "name", js_name },
		{ "fontname", js_fontname },
		{ "fontsize", js_fontsize },
		{ "primary_colour", js_primary_colour },
		{ "secondary_colour", js_secondary_colour },
		{ "outline_colour", js_outline_colour },
		{ "back_colour", js_back_colour },
		{ "bold", js_bold },
		{ "italic", js_italic },
		{ "underline", js_underline },
		{ "strike_out", js_strike_out },
		{ "scale_x", js_scale_x },
		{ "scale_y", js_scale_y },
		{ "spacing", js_spacing },
		{ "angle", js_angle },
		{ "border_style", js_border_style },
		{ "outline", js_outline },
		{ "shadow", js_shadow },
		{ "alignment", js_alignment },
		{ "margin_l", js_margin_l },
		{ "margin_r", js_margin_r },
		{ "margin_v", js_margin_v },
		{ "encoding", js_encoding },

	};

	for(const auto& [key, value] : properties_vector) {
		v8::Local<v8::String> keyValue = Nan::New<v8::String>(key).ToLocalChecked();
		Nan::Set(result, keyValue, value).Check();
	}

	return result;
}

[[nodiscard]] static v8::Local<v8::Value> styles_to_js(v8::Isolate* isolate,
                                                       const AssStyles& styles) {

	v8::Local<v8::Array> array = v8::Array::New(isolate);

	for(size_t i = 0; i < stbds_arrlenu(styles.entries); ++i) {
		AssStyleEntry style = styles.entries[i];

		Nan::Set(array, i, style_to_js(isolate, style));
	}

	return array;
}

[[nodiscard]] static const char* script_type_to_string(ScriptType script_type) {
	switch(script_type) {
		case ScriptTypeUnknown: return "Unknown";
		case ScriptTypeV4: return "V4";
		case ScriptTypeV4Plus: return "V4Plus";
		default: {
			assert(false && "UNREACHABLE");
			return "<ERROR>";
		}
	}
}

[[nodiscard]] static v8::Local<v8::Value> script_type_to_js(v8::Isolate* isolate,
                                                            const ScriptType& script_type) {
	return Nan::New<v8::String>(script_type_to_string(script_type)).ToLocalChecked();
}

[[nodiscard]] static v8::Local<v8::Value> wrap_style_to_js(v8::Isolate* isolate,
                                                           const WrapStyle& style) {
	return Nan::New<v8::Uint32>(static_cast<uint32_t>(style));
}

[[nodiscard]] static v8::Local<v8::Value> script_info_to_js(v8::Isolate* isolate,
                                                            const AssScriptInfo& script_info) {

	v8::Local<v8::Object> result = Nan::New<v8::Object>();

	auto js_title = final_str_to_js(isolate, script_info.title);

	auto js_original_script = final_str_to_js(isolate, script_info.original_script);

	auto js_original_translation = final_str_to_js(isolate, script_info.original_translation);

	auto js_original_editing = final_str_to_js(isolate, script_info.original_editing);

	auto js_original_timing = final_str_to_js(isolate, script_info.original_timing);

	auto js_synch_point = final_str_to_js(isolate, script_info.synch_point);

	auto js_script_updated_by = final_str_to_js(isolate, script_info.script_updated_by);

	auto js_update_details = final_str_to_js(isolate, script_info.update_details);

	auto js_script_type = script_type_to_js(isolate, script_info.script_type);

	auto js_collisions = final_str_to_js(isolate, script_info.collisions);

	auto js_play_res_y = size_t_to_js(isolate, script_info.play_res_y);

	auto js_play_res_x = size_t_to_js(isolate, script_info.play_res_x);

	auto js_play_depth = final_str_to_js(isolate, script_info.play_depth);

	auto js_timer = final_str_to_js(isolate, script_info.timer);

	auto js_wrap_style = wrap_style_to_js(isolate, script_info.wrap_style);

	auto js_scaled_border_and_shadow = bool_to_js(isolate, script_info.scaled_border_and_shadow);

	auto js_video_aspect_ratio = size_t_to_js(isolate, script_info.video_aspect_ratio);

	auto js_video_zoom = size_t_to_js(isolate, script_info.video_zoom);

	auto js_ycbcr_matrix = final_str_to_js(isolate, script_info.ycbcr_matrix);

	std::vector<std::pair<std::string, v8::Local<v8::Value>>> properties_vector{
		{ "title", js_title },
		{ "original_script", js_original_script },
		{ "original_translation", js_original_translation },
		{ "original_editing", js_original_editing },
		{ "original_timing", js_original_timing },
		{ "synch_point", js_synch_point },
		{ "script_updated_by", js_script_updated_by },
		{ "update_details", js_update_details },
		{ "script_type", js_script_type },
		{ "collisions", js_collisions },
		{ "play_res_y", js_play_res_y },
		{ "play_res_x", js_play_res_x },
		{ "play_depth", js_play_depth },
		{ "timer", js_timer },
		{ "wrap_style", js_wrap_style },
		{ "scaled_border_and_shadow", js_scaled_border_and_shadow },
		{ "video_aspect_ratio", js_video_aspect_ratio },
		{ "video_zoom", js_video_zoom },
		{ "ycbcr_matrix", js_ycbcr_matrix },
	};

	for(const auto& [key, value] : properties_vector) {
		v8::Local<v8::String> keyValue = Nan::New<v8::String>(key).ToLocalChecked();
		Nan::Set(result, keyValue, value).Check();
	}

	return result;
}

[[nodiscard]] static v8::Local<v8::Value> ass_result_to_js(v8::Isolate* isolate,
                                                           const AssResult& ass_result) {

	v8::Local<v8::Object> result = Nan::New<v8::Object>();

	auto js_script_info = script_info_to_js(isolate, ass_result.script_info);

	auto js_styles = styles_to_js(isolate, ass_result.styles);

	auto js_events = events_to_js(isolate, ass_result.events);

	auto js_extra_sections = extra_sections_to_js(isolate, ass_result.extra_sections);

	auto js_file_props = file_props_to_js(isolate, ass_result.file_props);

	std::vector<std::pair<std::string, v8::Local<v8::Value>>> properties_vector{
		{ "script_info", js_script_info },
		{ "styles", js_styles },
		{ "events", js_events },
		{ "extra_sections", js_extra_sections },
		{ "file_props", js_file_props }
	};

	for(const auto& [key, value] : properties_vector) {
		v8::Local<v8::String> keyValue = Nan::New<v8::String>(key).ToLocalChecked();
		Nan::Set(result, keyValue, value).Check();
	}

	return result;
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
