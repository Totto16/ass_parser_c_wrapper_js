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

const DEFAULT_SETTINGS: ParseSettingsTS = "non-strict"

describe("parse_ass: works as expected", () => {
	it("should return an error for non existent file", async () => {
		const file = getFilePath("NON-EXISTENT.ass")

		expect(fs.existsSync(file)).toBe(false)

		const result = AssParser.parse_ass_file(file, DEFAULT_SETTINGS)
		expect(result).toBe(null)
	})

	it("should return error for incorrect file", async () => {
		const file = getFilePath("incorrect.rec")

		expect(fs.existsSync(file)).toBe(true)

		const result = AssParser.parse_ass_file(file, DEFAULT_SETTINGS)
		expect(result).toBe(null)
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
