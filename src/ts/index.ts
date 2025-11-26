import path from "path"

const rootDir = path.join(
	__dirname,
	...(__filename.endsWith(".ts") ? ["..", ".."] : [".."])
)
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
	allow_validation_errors: boolean
}

export type FontPreset =
	| "disabled"
	| "strict-all"
	| "strict"
	| "moderate"
	| "lenient"

export interface FontSettings {
	preset: FontPreset
}

export interface ValidateSettings {
	font_settings: FontSettings
	validate_styles: boolean
	validate_text: boolean
}

export interface ParseSettings {
	strict_settings: StrictSettings
	validate_settings: ValidateSettings
}

export type StrictSettingsTS = "strict" | "non-strict" | StrictSettings

export type ValidateSettingsTS = "everything" | "nothing" | ValidateSettings

export interface ParseSettingsTS {
	strict_settings: StrictSettingsTS
	validate_settings: ValidateSettingsTS
}

export type LineType = "CrLf" | "Lf" | "Cr"

export type FileType =
	| "Unknown"
	| "UTF-8"
	| "UTF-16BE"
	| "UTF-16LE"
	| "UTF-32BE"
	| "UTF-32LE"

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

export type EventType =
	| "Dialogue"
	| "Comment"
	| "Picture"
	| "Sound"
	| "Movie"
	| "Command"

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

export type ScriptType = "Unknown" | "V4" | "V4Plus"

export type WrapStyle = "Smart" | "EOL" | "NoWrap" | "SmartLow"

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

export type DiagnosticSeverity = "warning" | "error"

export interface FilePos {
	line: SizeT
	column: SizeT
}
export interface Diagnostic {
	message: string
	severity: DiagnosticSeverity
	position?: FilePos
}

export interface AssParseResultBase {
	diagnostics: Diagnostic[]
}

export interface AssParseResultError {
	error: true
}

export interface AssParseResultSuccess {
	error: false
	result: AssResult
}

export type AssParseResult = AssParseResultBase &
	(AssParseResultError | AssParseResultSuccess)

type AssSource =
	| { type: "file"; name: string }
	| { type: "string"; content: string }

export class AssParser {
	static resolve_strict_settings(
		settings_ts: StrictSettingsTS
	): StrictSettings {
		if (settings_ts === "strict") {
			return {
				script_info: {
					allow_duplicate_fields: false,
					allow_missing_script_type: false,
				},
				allow_additional_fields: false,
				allow_number_truncating: false,
				allow_unrecognized_file_encoding: false,
				allow_validation_errors: false,
			}
		}

		if (settings_ts === "non-strict") {
			return {
				script_info: {
					allow_duplicate_fields: true,
					allow_missing_script_type: true,
				},
				allow_additional_fields: true,
				allow_number_truncating: true,
				allow_unrecognized_file_encoding: true,
				allow_validation_errors: true,
			}
		}

		return settings_ts
	}

	static resolve_validate_settings(
		settings_ts: ValidateSettingsTS
	): ValidateSettings {
		if (settings_ts === "everything") {
			return {
				font_settings: { preset: "strict-all" },
				validate_text: true,
				validate_styles: true,
			}
		}

		if (settings_ts === "nothing") {
			return {
				font_settings: { preset: "disabled" },
				validate_text: false,
				validate_styles: false,
			}
		}

		return settings_ts
	}

	static resolve_parse_settings(settings_ts: ParseSettingsTS): ParseSettings {
		return {
			strict_settings: AssParser.resolve_strict_settings(
				settings_ts.strict_settings
			),
			validate_settings: AssParser.resolve_validate_settings(
				settings_ts.validate_settings
			),
		}
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
				diagnostics: [
					{ message: (err as Error).message, severity: "error" },
				],
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
