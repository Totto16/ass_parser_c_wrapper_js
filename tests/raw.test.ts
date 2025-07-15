import { expect } from "@jest/globals"
import path from "path"

function fail(reason = "fail was called in a test."): never {
	throw new Error(reason)
}

global.fail = fail

const rootDir = path.join(__dirname, "..")
const ass_parser = require("node-gyp-build")(rootDir)

describe("parse_ass", () => {
	it("should throw an error, when no first argument was given", async () => {
		try {
			ass_parser.parse_ass()

			fail("it should not reach here")
		} catch (e) {
			expect((e as any).toString()).toEqual(
				"TypeError: Wrong number of arguments"
			)
		}
	})
})

describe("exported properties", () => {
	it("should only have known properties", async () => {
		const expectedKeys = ["parse_ass", "version", "commit_hash"]

		const keys = Object.keys(ass_parser)
		expect(keys).toStrictEqual(expectedKeys)
	})

	it("should have the expected properties", async () => {
		const expectedProperties: Record<string, any> = {
			parse_ass: () => {},
			version: "0.0.3",
			commit_hash: "cd76a58fa443",
		}

		for (const key in expectedProperties) {
			const value = expectedProperties[key]
			const rawValue = ass_parser[key]

			if (typeof value === "string") {
				expect(value).toBe(rawValue)
			} else if (typeof value === "function") {
				expect(rawValue).toStrictEqual(expect.any(Function))
			} else {
				expect(value).toMatchObject(rawValue)
			}
		}
	})
})
