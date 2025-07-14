

#pragma once

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#if !defined(__clang__)
#pragma GCC diagnostic ignored "-Wtemplate-id-cdtor"
#endif
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <nan.h>

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

#include <expected>

#include <ass_parser_lib.h>

#include "./wrapper.h"

[[nodiscard]] std::expected<AssSourceCpp, v8::Local<v8::Value>>
get_ass_source_from_info(v8::Local<v8::Value> value);

[[nodiscard]] std::expected<ParseSettings, v8::Local<v8::Value>>
get_parse_settings_from_info(v8::Isolate* isolate, v8::Local<v8::Value> value);

[[nodiscard]] v8::Local<v8::Value> ass_parse_result_to_js(v8::Isolate* isolate,
                                                          AssParseResultCpp result);
