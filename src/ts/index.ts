import path from "path"

const rootDir = path.join(__dirname, "..", "..")
const ass_parser = require("node-gyp-build")(rootDir)

export type U64 = number | BigInt
export type U32 = number
export type U8 = number

export type SizeT = U64
export type Double = number

export interface ScriptInfoStrictSettings {
	allow_duplicate_fields: boolean
	allow_missing_script_type: boolean
}

export interface StrictSettings {
	script_info: ScriptInfoStrictSettings
	allow_additional_fields: boolean
	allow_number_truncating: boolean
	allow_unrecognized_file_encoding: boolean
}

export interface ParseSettings {
	strict_settings: StrictSettings
}

export type ParseSettingsTS = "strict" | "non-strict" | ParseSettings

export interface AssParseResultError {
	error: true
	message: string
}

export enum LineType {
	"CrLf",
	"Lf",
	"Cr",
}

export enum FileType {
	"unknown",
	"UTF-8",
	"UTF-16BE",
	"UTF-16LE",
	"UTF-32BE",
	"UTF-32LE",
}
FileType

export interface FileProps {
	line_type: LineType
	file_type: FileType
}

export type ExtraSectionEntry = Record<string, string>

export type ExtraSections = Record<string, ExtraSectionEntry>

export type MarginValue = "default" | SizeT

export interface AssTime {
	hour: U8
	min: U8
	sec: U8
	hundred: U8
}

export enum EventType {
	"Dialogue",
	"Comment",
	"Picture",
	"Sound",
	"Movie",
	"Command",
}

export interface AssEvent {
	// marks different event_types
	type: EventType
	// original fields
	layer: SizeT
	start: AssTime
	end: AssTime
	style: string
	name: string
	margin_l: MarginValue
	margin_r: MarginValue
	margin_v: MarginValue
	effect: string
	text: string
}

export enum AssAlignment {
	// bottom
	"AssAlignmentBL" = 1,
	"AssAlignmentBC",
	"AssAlignmentBR",
	// middle
	"AssAlignmentML",
	"AssAlignmentMC",
	"AssAlignmentMR",
	// top
	"AssAlignmentTL",
	"AssAlignmentTC",
	"AssAlignmentTR",
}

export enum BorderStyle {
	"Outline" = 1,
	"OpaqueBox" = 3,
}

export interface AssColor {
	r: U8
	g: U8
	b: U8
	a: U8
}

export interface AssStyle {
	name: string
	fontname: string
	fontsize: SizeT
	primary_colour: AssColor
	secondary_colour: AssColor
	outline_colour: AssColor
	back_colour: AssColor
	bold: boolean
	italic: boolean
	underline: boolean
	strike_out: boolean
	scale_x: SizeT
	scale_y: SizeT
	spacing: Double
	angle: Double
	border_style: BorderStyle
	outline: Double
	shadow: Double
	alignment: AssAlignment
	margin_l: SizeT
	margin_r: SizeT
	margin_v: SizeT
	encoding: SizeT
}

export enum ScriptType {
	"Unknown",
	"V4",
	"V4Plus",
}

export enum WrapStyle {
	"Smart" = 0,
	"EOL",
	"NoWrap",
	"SmartLow",
}

export interface AssScriptInfo {
	title: string
	original_script: string
	original_translation: string
	original_editing: string
	original_timing: string
	synch_point: string
	script_updated_by: string
	update_details: string
	script_type: ScriptType
	collisions: string
	play_res_y: SizeT
	play_res_x: SizeT
	play_depth: string
	timer: string
	wrap_style: WrapStyle
	// not documented, but present
	scaled_border_and_shadow: boolean
	video_aspect_ratio: SizeT
	video_zoom: SizeT
	ycbcr_matrix: string
}

export interface AssResult {
	script_info: AssScriptInfo
	styles: AssStyle[]
	events: AssEvent[]
	//fonts: AssFonts
	//graphics: AssGraphics
	extra_sections: ExtraSections
	file_props: FileProps
}

export interface AssParseResultSuccess {
	error: false
	result: AssResult
}

export interface Warning {
	message: string
}

export interface AssParseResultBase {
	warnings: Warning[]
}

export type AssParseResult = AssParseResultBase &
	(AssParseResultError | AssParseResultSuccess)

type AssSource =
	| { type: "file"; name: string }
	| { type: "string"; content: string }

export class AssParser {
	private static resolve_parse_settings(
		settings_ts: ParseSettingsTS
	): ParseSettings {
		if (settings_ts === "strict") {
			return {
				strict_settings: {
					script_info: {
						allow_duplicate_fields: false,
						allow_missing_script_type: false,
					},
					allow_additional_fields: false,
					allow_number_truncating: false,
					allow_unrecognized_file_encoding: false,
				},
			}
		}

		if (settings_ts === "non-strict") {
			return {
				strict_settings: {
					script_info: {
						allow_duplicate_fields: true,
						allow_missing_script_type: true,
					},
					allow_additional_fields: true,
					allow_number_truncating: true,
					allow_unrecognized_file_encoding: true,
				},
			}
		}

		return settings_ts
	}

	private static parse_ass(
		source: AssSource,
		settings_ts: ParseSettingsTS
	): AssParseResult {
		try {
			const settings: ParseSettings =
				AssParser.resolve_parse_settings(settings_ts)

			// this throws, when the argument are not as expected, just to be safe for JS land
			return ass_parser.parse_ass(source, settings)
		} catch (err) {
			return {
				error: true,
				message: (err as Error).message,
				warnings: [],
			}
		}
	}

	static parse_ass_file(
		file: string,
		settings: ParseSettingsTS
	): AssParseResult {
		return AssParser.parse_ass({ type: "file", name: file }, settings)
	}

	static parse_ass_string(
		file: string,
		settings: ParseSettingsTS
	): AssParseResult {
		return AssParser.parse_ass({ type: "string", content: file }, settings)
	}

	static get version(): string {
		return ass_parser.version
	}

	static get commit_hash(): string {
		return ass_parser.commit_hash
	}
}
