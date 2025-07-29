import { expect } from "@jest/globals"
import path from "path"
import fs from "fs"
import { sampleFiles } from "./samples"
import { AssParser, type ParseSettingsTS } from "../src/ts/index"

function fail(reason = "fail was called in a test."): never {
	throw new Error(reason)
}

global.fail = fail

function getFilePath(name: string): string {
	return path.join(__dirname, `files`, name)
}

const DEFAULT_SETTINGS: ParseSettingsTS = {
	strict_settings: "non-strict",
	validate_settings: "everything",
}

describe("parse_ass: works as expected", () => {
	it("should return an error for non existent file", async () => {
		const file = getFilePath("NON-EXISTENT.ass")

		expect(fs.existsSync(file)).toBe(false)

		const result = AssParser.parse_ass_file(file, DEFAULT_SETTINGS)
		expect(result).toMatchObject({
			error: true,
			diagnostics: [{ message: "no such file", severity: "error" }],
		})
	})

	it("should return error for incorrect file", async () => {
		const file = getFilePath("incorrect.ass")

		expect(fs.existsSync(file)).toBe(true)

		const result = AssParser.parse_ass_file(file, DEFAULT_SETTINGS)
		expect(result).toMatchObject({
			error: true,
			diagnostics: [
				{
					message: "first line must be the script info section",
					position: {
						column: 1,
						line: 0,
					},
					severity: "error",
				},
			],
		})
	})

	it("should return an object for correct file", async () => {
		const file = getFilePath("test.ass")

		expect(fs.existsSync(file)).toBe(true)

		const result = AssParser.parse_ass_file(file, DEFAULT_SETTINGS)
		expect(result).not.toBe(null)
	})

	it("should return correct objects for correct files", async () => {
		for (const { file, result } of sampleFiles) {
			const filePath = getFilePath(file)
			expect(fs.existsSync(filePath)).toBe(true)

			const actual_result = AssParser.parse_ass_file(
				filePath,
				DEFAULT_SETTINGS
			)
			expect(actual_result).toMatchObject(result as any)
		}
	})
})
