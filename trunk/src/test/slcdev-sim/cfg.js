{
	"sensor" : { 
		"model" : "1251",
		"enabled" : true,
		"group" : [1],
		"addr" : [ 1 ]
	},

	"sensor" : { 
		"model" : "1251",
		"enabled" : false,
		"group" : [ 2, 4 ],
		"addr" : [ 2, 3, 4, 5, 6, 7, 8, 9, 10 ]
	},

	"sensor" : { 
		"model" : "2251",
		"enabled" : true,
		"group" : [ 1 ],
		"addr" : [ 11 ]
	},

	"sensor" : { 
		"model" : "2251",
		"enabled" : false,
		"group" : [ 2, 4 ],
		"addr" : [ 12, 13, 14, 15, 16, 17, 18, 19, 20 ]
	},

	"sensor" : { 
		"model" : "5251P",
		"enabled" : true,
		"group" : [ 1 ],
		"addr" : [ 21 ]
	},

	"sensor" : { 
		"model" : "5251P",
		"enabled" : false,
		"group" : [ 2, 4 ],
		"addr" : [ 22, 23, 24, 25, 26, 27, 28, 29, 30 ]
	},

	"sensor" : { 
		"model" : "2251TM",
		"group" : [ 1 ],
		"addr" : [ 31 ]
	},

	"sensor" : { 
		"model" : "2251TM",
		"enabled" : false,
		"group" : [ 2, 4 ],
		"addr" : [ 32, 33, 34, 35, 36, 37, 38, 39, 40 ]
	},

	"sensor" : { 
		"model" : "2251 COPTIR",
		"enabled" : true,
		"group" : [ 1 ],
		"addr" : [ 41 ] 
	},

	"sensor" : { 
		"model" : "2251 COPTIR",
		"enabled" : false,
		"group" : [ 2, 4 ],
		"addr" : [ 42, 43, 44, 45, 46, 47, 48, 49, 50 ] 
	},

	"sensor" : { 
		"model" : "2251",
		"enabled" : false,
		"group" : [ 3, 4 ],
		"addr" : [ 
			    51, 52, 53, 54, 55, 56, 57, 58, 59,
			60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
			70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
			80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
			90, 91, 92, 93, 94, 95, 96, 97, 98, 99 
		] 
	},

	"module" : { 
		"model" : "M500M",
		"enabled" : true,
		"group" : [ 1 ],
		"addr" : [ 1 ]
	},

	"module" : { 
		"model" : "M500M",
		"enabled" : false,
		"group" : [ 2, 4 ],
		"addr" : [ 2, 3, 4, 5, 6, 7, 8, 9, 10 ]
	},

	"module" : { 
		"model" : "M500M-4-20",
		"enabled" : true,
		"group" : [ 1 ],
		"addr" : [ 11 ]
	},

	"module" : { 
		"model" : "M500M-4-20",
		"enabled" : false,
		"group" : [ 2, 4 ],
		"addr" : [ 12, 13, 14, 15, 16, 17, 18, 19, 20 ]
	},

	"module" : { 
		"model" : "M500R",
		"enabled" : true,
		"group" : [ 1 ],
		"addr" : [ 21 ] 
	},

	"module" : { 
		"model" : "M500R",
		"enabled" : false,
		"group" : [ 2, 4 ],
		"addr" : [ 22, 23, 24, 25, 26, 27, 28, 29, 30 ]
	},

	"module" : { 
		"model" : "M500S",
		"enabled" : true,
		"group" : [ 1 ],
		"addr" : [ 31 ]
	},

	"module" : { 
		"model" : "M500S",
		"enabled" : false,
		"group" : [ 2, 4 ],
		"addr" : [ 32, 33, 34, 35, 36, 37, 38, 39, 40 ]
	},


	"module" : { 
		"model" : "M500FP",
		"enabled" : true,
		"group" : [ 1 ],
		"addr" : [ 41 ] 
	},

	"module" : { 
		"model" : "M500FP",
		"enabled" : false,
		"group" : [ 2, 4 ],
		"addr" : [ 42, 43, 44, 45, 46, 47, 48, 49, 50 ] 
	},

	"module" : { 
		"model" : "M500S",
		"enabled" : false,
		"group" : [ 3, 4 ],
		"addr" : [ 
			    51, 52, 53, 54, 55, 56, 57, 58, 59,
			60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
			70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
			80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
			90, 91, 92, 93, 94, 95, 96, 97, 98, 99 
		] 
	},
	
	"sw1": { 
		"up" : [ 
			'/* enable all */',
			'var i',
			'for (i = 0; i < 160; ++i) {',
			'	sens_en(i, true);',
			'	mod_en(i, true);',
			'}'
		],

		"off" : [ 
			"goto enable_all"
		],

		"down" : [
			"sensor[1].alarm = true"
		],
	},

	"sw2": { 
		"up" : [ 
			"goto disable_all"
		],

		"off" : [ 
			"goto enable_all"
		],

		"down" : [
			"sensor[1].alarm = true"
		],
	},

	"script": {
		"tag" : "enable_all",
		"js" : [
			'var i",
			'for (i = 0; i < 160; ++i) {',
			'	sensor[i].enabled = true',
			'	module[i].enabled = true',
			'}"
		],
	},

	"script": {
		"tag" : "test",
		"js" : [
			'var i = 0',
			'while (i < 20) {',
			'	if (sensor[i].type == "2251TM") {',
			'		sensor[i].enabled = true',
			'		sensor[i].pw2.lookup(2)',
			'	}',
			'	i = i + 1',
			'}',
		],
	},

	"script": {
		"smoke1_alm" : [
			'var i = 0',
			'while (i < 20) {',
			'	sensor[i].pw4.lookup(4)',
			'	sensor[i].enabled = yes',
			'	i = i + 1',
			'}',
		],
	},
}

