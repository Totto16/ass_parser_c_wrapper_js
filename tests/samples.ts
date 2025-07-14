import { AssParseResult, WrapStyle } from "../src/ts/index"

export interface TestFile {
	result: AssParseResult
	file: string
}

export const sampleFiles: TestFile[] = [
	{
		result: {
			error: false,
			warnings: [],
			result: {
				events: [],
				extra_sections: {},
				file_props: {
					file_type: "UTF-16BE",
					line_type: "Cr",
				},
				script_info: {
					title: "",
					original_script: "",
					original_translation: "",
					original_editing: "",
					original_timing: "",
					synch_point: "",
					script_updated_by: "",
					update_details: "",
					script_type: "Unknown",
					collisions: "",
					play_res_y: 1,
					play_res_x: 1,
					play_depth: "",
					timer: "",
					wrap_style: WrapStyle.EOL,
					// not documented
					scaled_border_and_shadow: true,
					video_aspect_ratio: 1,
					video_zoom: 1,
					ycbcr_matrix: "",
				},
				styles: [],
			},
		},
		file: "test.ass",
	},
]
