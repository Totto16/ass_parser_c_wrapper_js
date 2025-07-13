import fs from "fs"
import path from "path"

const rootDir = path.join(__dirname, "..", "..")
const ass_parser = require("node-gyp-build")(rootDir)

export type U64 = number | BigInt
export type U32 = number
export type U8 = number

export interface ParseSettings {
	todo: string
}

export interface AssResultError {
	error: true
	message: string
}

export interface AssResultSuccess {
	error: false
}

export type AssResult = AssResultError | AssResultSuccess

type AssSource =
	| { type: "file"; name: string }
	| { type: "string"; content: string }

export class AssParser {
	private static parse_ass(
		source: AssSource,
		settings: ParseSettings
	): AssResult {
		try {
			// this throws, when the argument are not as expected, just to be safe for JS land
			return ass_parser.parse_ass(source, settings)
		} catch (err) {
			return {
				error: true,
				message: (err as Error).message,
			}
		}
	}

	static parse_ass_file(file: string, settings: ParseSettings): AssResult {
		return AssParser.parse_ass({ type: "file", name: file }, settings)
	}

	static parse_ass_string(file: string, settings: ParseSettings): AssResult {
		return AssParser.parse_ass({ type: "string", content: file }, settings)
	}

	static get version(): string {
		return ass_parser.version
	}

	static get commit_hash(): string {
		return ass_parser.commit_hash
	}
}
