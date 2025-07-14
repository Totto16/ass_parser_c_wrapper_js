

#include "./convert.hpp"

#include <limits>
#include <stb/ds.h>

// generic helper functions

[[nodiscard]] static v8::Local<v8::Value> c_str_to_js(const char* str) {

	return Nan::New<v8::String>(str).ToLocalChecked();
}

[[nodiscard]] static v8::Local<v8::Value> str_to_js(const std::string& str) {

	return Nan::New<v8::String>(str).ToLocalChecked();
}

// js to c

[[nodiscard]] std::expected<AssSourceCpp, v8::Local<v8::Value>>
get_ass_source_from_info(v8::Local<v8::Value> value) {

	if(!value->IsObject()) {
		return std::unexpected{ Nan::TypeError("the 'source' argument needs to be an object") };
	}

	auto object = value->ToObject(Nan::GetCurrentContext()).ToLocalChecked();

	auto type_key = c_str_to_js("type");

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

		auto name_key = c_str_to_js("name");

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

		auto content_key = c_str_to_js("content");

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

	auto allow_duplicate_fields_key = c_str_to_js("allow_duplicate_fields");

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

	auto allow_missing_script_type_key = c_str_to_js("allow_missing_script_type");

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

	auto script_info_key = c_str_to_js("script_info");

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

	auto allow_additional_fields_key = c_str_to_js("allow_additional_fields");

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

	auto allow_number_truncating_key = c_str_to_js("allow_number_truncating");

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

	auto allow_unrecognized_file_encoding_key = c_str_to_js("allow_unrecognized_file_encoding");

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

	auto strict_settings_key = c_str_to_js("strict_settings");

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

[[nodiscard]] static v8::Local<v8::Value> u32_to_js(v8::Isolate* isolate, uint32_t value) {
	UNUSED(isolate);
	return Nan::New<v8::Uint32>(value);
}

[[nodiscard]] static v8::Local<v8::Value> u64_to_appropiate_number(v8::Isolate* isolate,
                                                                   uint64_t value) {

	if(value > std::numeric_limits<uint32_t>::max()) {
		return new_bigint_from_uint64(isolate, value);
	}
	return u32_to_js(isolate, value);
}

[[nodiscard]] static v8::Local<v8::Value> size_t_to_js(v8::Isolate* isolate, size_t value) {

	return u64_to_appropiate_number(isolate, value);
}

[[nodiscard]] static v8::Local<v8::Value> final_str_to_js(v8::Isolate* isolate,
                                                          const FinalStr& str) {

	UNUSED(isolate);

	if(str.length == 0) {
		return Nan::EmptyString();
	}

	char* value = get_normalized_string(str);

	auto result = c_str_to_js(value);

	free(value);

	return result;
}

[[nodiscard]] static v8::Local<v8::Value> bool_to_js(v8::Isolate* isolate, bool value) {
	UNUSED(isolate);

	if(value) {
		return Nan::True();
	}

	return Nan::False();
}

[[nodiscard]] static v8::Local<v8::Value> double_to_js(v8::Isolate* isolate, double value) {
	UNUSED(isolate);
	return Nan::New<v8::Number>(value);
}

using ObjectProperties = std::vector<std::pair<std::string, v8::Local<v8::Value>>>;

[[nodiscard]] static v8::Local<v8::Value> make_js_object(v8::Isolate* isolate,
                                                         const ObjectProperties& properties) {
	UNUSED(isolate);

	v8::Local<v8::Object> object = Nan::New<v8::Object>();

	for(const auto& [key, value] : properties) {
		v8::Local<v8::String> key_value = str_to_js(key);
		Nan::Set(object, key_value, value).Check();
	}

	return object;
}

// complex structs / objects to js

[[nodiscard]] static v8::Local<v8::Value> warning_to_js(v8::Isolate* isolate,
                                                        const WarningEntry& warning) {

	UNUSED(isolate);

	v8::Local<v8::Object> result = Nan::New<v8::Object>();

	ErrorStruct message = get_warnings_message_from_entry(warning);

	auto js_message = c_str_to_js(message.message);

	free_error_struct(message);

	v8::Local<v8::String> message_key = c_str_to_js("message");
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

[[nodiscard]] static const char* line_type_to_string(LineType line_type) {
	switch(line_type) {
		case LineTypeCrLf: return "CrLf";
		case LineTypeLf: return "Lf";
		case LineTypeCr: return "Cr";
		default: {
			assert(false && "UNREACHABLE");
			return "<ERROR>";
		}
	}
}

[[nodiscard]] static v8::Local<v8::Value> line_type_to_js(v8::Isolate* isolate,
                                                          const LineType& line_type) {
	UNUSED(isolate);
	return c_str_to_js(line_type_to_string(line_type));
}

[[nodiscard]] static const char* file_type_to_string(FileType file_type) {
	switch(file_type) {
		case FileTypeUnknown: return "Unknown";
		case FileTypeUtf8: return "UTF-8";
		case FileTypeUtf16BE: return "UTF-16BE";
		case FileTypeUtf16LE: return "UTF-16LE";
		case FileTypeUtf32BE: return "UTF-32BE";
		case FileTypeUtf32LE: return "UTF-32LE";
		default: {
			assert(false && "UNREACHABLE");
			return "<ERROR>";
		}
	}
}

[[nodiscard]] static v8::Local<v8::Value> file_type_to_js(v8::Isolate* isolate,
                                                          const FileType& file_type) {
	UNUSED(isolate);
	return c_str_to_js(file_type_to_string(file_type));
}

[[nodiscard]] static v8::Local<v8::Value> file_props_to_js(v8::Isolate* isolate,
                                                           const FileProps& file_props) {

	auto js_line_type = line_type_to_js(isolate, file_props.line_type);

	auto js_file_type = file_type_to_js(isolate, file_props.file_type);

	ObjectProperties properties{
		{ "line_type", js_line_type },
		{ "file_type", js_file_type },

	};

	return make_js_object(isolate, properties);
}

[[nodiscard]] static v8::Local<v8::Value>
extra_section_entry_to_js(v8::Isolate* isolate, const ExtraSectionEntry& entry) {

	v8::Local<v8::Object> result = Nan::New<v8::Object>();

	size_t hm_length = stbds_shlenu(entry.fields);

	for(size_t i = 0; i < hm_length; ++i) {
		SectionFieldEntry hm_entry = entry.fields[i];

		v8::Local<v8::String> key_value = c_str_to_js(hm_entry.key);

		auto js_value = final_str_to_js(isolate, hm_entry.value);

		Nan::Set(result, key_value, js_value).Check();
	}

	return result;
}

[[nodiscard]] static v8::Local<v8::Value>
extra_sections_to_js(v8::Isolate* isolate, const ExtraSections& extra_sections) {

	v8::Local<v8::Object> result = Nan::New<v8::Object>();

	size_t hm_length = stbds_shlenu(extra_sections.entries);

	for(size_t i = 0; i < hm_length; ++i) {
		ExtraSectionHashMapEntry entry = extra_sections.entries[i];

		v8::Local<v8::String> key_value = c_str_to_js(entry.key);

		auto js_value = extra_section_entry_to_js(isolate, entry.value);

		Nan::Set(result, key_value, js_value).Check();
	}

	return result;
}

[[nodiscard]] static v8::Local<v8::Value> margin_to_js(v8::Isolate* isolate,
                                                       const MarginValue& value) {

	if(value.is_default) {
		return c_str_to_js("default");
	}

	return size_t_to_js(isolate, value.data.value);
}

[[nodiscard]] static v8::Local<v8::Value> ass_time_to_js(v8::Isolate* isolate,
                                                         const AssTime& time) {

	auto js_hour = u32_to_js(isolate, time.hour);

	auto js_min = u32_to_js(isolate, time.min);

	auto js_sec = u32_to_js(isolate, time.sec);

	auto js_hundred = u32_to_js(isolate, time.hundred);

	ObjectProperties properties{
		{ "hour", js_hour },
		{ "min", js_min },
		{ "sec", js_sec },
		{ "hundred", js_hundred },
	};

	return make_js_object(isolate, properties);
}

[[nodiscard]] static const char* event_type_to_string(EventType event_type) {
	switch(event_type) {
		case EventTypeDialogue: return "Dialogue";
		case EventTypeComment: return "Comment";
		case EventTypePicture: return "Picture";
		case EventTypeSound: return "Sound";
		case EventTypeMovie: return "Movie";
		case EventTypeCommand: return "Command";
		default: {
			assert(false && "UNREACHABLE");
			return "<ERROR>";
		}
	}
}

[[nodiscard]] static v8::Local<v8::Value> event_type_to_js(v8::Isolate* isolate,
                                                           const EventType& event_type) {
	UNUSED(isolate);
	return c_str_to_js(event_type_to_string(event_type));
}

[[nodiscard]] static v8::Local<v8::Value> event_to_js(v8::Isolate* isolate,
                                                      const AssEventEntry& event) {

	auto js_type = event_type_to_js(isolate, event.type);

	auto js_layer = size_t_to_js(isolate, event.layer);

	auto js_start = ass_time_to_js(isolate, event.start);

	auto js_end = ass_time_to_js(isolate, event.end);

	auto js_style = final_str_to_js(isolate, event.style);

	auto js_name = final_str_to_js(isolate, event.name);

	auto js_margin_l = margin_to_js(isolate, event.margin_l);

	auto js_margin_r = margin_to_js(isolate, event.margin_r);

	auto js_margin_v = margin_to_js(isolate, event.margin_v);

	auto js_effect = final_str_to_js(isolate, event.effect);

	auto js_text = final_str_to_js(isolate, event.text);

	ObjectProperties properties{
		{ "type", js_type },         { "layer", js_layer },       { "start", js_start },
		{ "end", js_end },           { "style", js_style },       { "name", js_name },
		{ "margin_l", js_margin_l }, { "margin_r", js_margin_r }, { "margin_v", js_margin_v },
		{ "effect", js_effect },     { "text", js_text },

	};

	return make_js_object(isolate, properties);
}

[[nodiscard]] static v8::Local<v8::Value> events_to_js(v8::Isolate* isolate,
                                                       const AssEvents& events) {

	v8::Local<v8::Array> array = v8::Array::New(isolate);

	for(size_t i = 0; i < stbds_arrlenu(events.entries); ++i) {
		AssEventEntry event = events.entries[i];

		Nan::Set(array, i, event_to_js(isolate, event));
	}

	return array;
}

[[nodiscard]] static v8::Local<v8::Value> border_style_to_js(v8::Isolate* isolate,
                                                             const BorderStyle& style) {
	return u32_to_js(isolate, static_cast<uint32_t>(style));
}

[[nodiscard]] static v8::Local<v8::Value> alignment_to_js(v8::Isolate* isolate,
                                                          const AssAlignment& alignment) {
	return u32_to_js(isolate, static_cast<uint32_t>(alignment));
}

[[nodiscard]] static v8::Local<v8::Value> ass_color_to_js(v8::Isolate* isolate,
                                                          const AssColor& color) {

	auto js_r = u32_to_js(isolate, color.r);

	auto js_g = u32_to_js(isolate, color.g);

	auto js_b = u32_to_js(isolate, color.b);

	auto js_a = u32_to_js(isolate, color.a);

	ObjectProperties properties{
		{ "r", js_r },
		{ "g", js_g },
		{ "b", js_b },
		{ "a", js_a },
	};

	return make_js_object(isolate, properties);
}

[[nodiscard]] static v8::Local<v8::Value> style_to_js(v8::Isolate* isolate,
                                                      const AssStyleEntry& style) {

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

	ObjectProperties properties{
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

	return make_js_object(isolate, properties);
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
	UNUSED(isolate);
	return c_str_to_js(script_type_to_string(script_type));
}

[[nodiscard]] static v8::Local<v8::Value> wrap_style_to_js(v8::Isolate* isolate,
                                                           const WrapStyle& style) {
	return u32_to_js(isolate, static_cast<uint32_t>(style));
}

[[nodiscard]] static v8::Local<v8::Value> script_info_to_js(v8::Isolate* isolate,
                                                            const AssScriptInfo& script_info) {

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

	ObjectProperties properties{
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

	return make_js_object(isolate, properties);
}

[[nodiscard]] static v8::Local<v8::Value> ass_result_to_js(v8::Isolate* isolate,
                                                           const AssResult& ass_result) {

	auto js_script_info = script_info_to_js(isolate, ass_result.script_info);

	auto js_styles = styles_to_js(isolate, ass_result.styles);

	auto js_events = events_to_js(isolate, ass_result.events);

	auto js_extra_sections = extra_sections_to_js(isolate, ass_result.extra_sections);

	auto js_file_props = file_props_to_js(isolate, ass_result.file_props);

	ObjectProperties properties{ { "script_info", js_script_info },
		                         { "styles", js_styles },
		                         { "events", js_events },
		                         { "extra_sections", js_extra_sections },
		                         { "file_props", js_file_props } };

	return make_js_object(isolate, properties);
}

v8::Local<v8::Value> ass_parse_result_to_js(v8::Isolate* isolate, AssParseResultCpp result) {

	auto js_warnings = warnings_to_js(isolate, result.warnings());

	ObjectProperties properties{ { "warnings", js_warnings } };

	std::visit(helper::Overloaded{
	               [&properties](const AssParseResultErrorCpp& result_err) -> void {
		               properties.emplace_back("error", Nan::True());

		               auto message_js = str_to_js(result_err.message);

		               properties.emplace_back("message", message_js);
	               },
	               [&properties, isolate](const AssParseResultOkCpp& result_ok) -> void {
		               properties.emplace_back("error", Nan::False());

		               auto ass_result_js = ass_result_to_js(isolate, result_ok.result);

		               properties.emplace_back("result", ass_result_js);
	               },
	           },
	           result.result());

	return make_js_object(isolate, properties);
}
