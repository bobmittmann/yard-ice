{
	"sensor" : { 
		"model" : "2251TM",
		"enabled" : false,
		"tag" : "smoke0",
		"ilat" : 80,
		"ipre" : 50,
		"imode" : 3,
		"irate" : 0,
		"addr" : [ 
			     4,  5,  6,  7,  8,  9, 
			10, 11, 12, 13, 14, 15, 16, 17, 18, 19
		] 
	},

	"sensor" : { 
		"model" : "2251TM",
		"enabled" : false,
		"tag" : "smoke1",
		"ilat" : 50,
		"ipre" : 35,
		"imode" : 2,
		"irate" : 3,
		"tbias" : 100
		"addr" : [ 1, 2, 3 ] 
	},

	"sensor" : { 
		"model" : "2251TM",
		"enabled" : false,
		"tag" : "smoke2",
		"ilat" : 100,
		"ipre" : 70,
		"imode" : 3,
		"irate" : 0,
		"tbias" : 105,
		"addr" : [ 2 ] 
	},

	"sensor" : { 
		"model" : "1251",
		"enabled" : true,
		"addr" : [ 3 ]
	},

	"sensor" : { 
		"model" : "Photo",
		"enabled" : true,
		"addr" : [ 5 ]
	},

	"sensor" : { 
		"model" : "2251TM",
		"enabled" : false,
		"tag" : "smoke3",
		"addr" : [ 
			20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
			30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
		] 
	},

	"sensor" : { 
		"model" : "5251P",
		"enabled" : false,
		"tag" : "Heat 1",
		"addr" : [ 
			40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
			50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
			60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
			70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
			80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
			90, 91, 92, 93, 94, 95, 96, 97, 98, 99 
		] 
	},

	"module" : { 
		"model" : "M500R",
		"enabled" : false,
		"tag" : "Relay 1",
		"addr" : [ 
			     2,  3,  4,  5,  6,  7,  8,  9, 
			10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
			20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
			30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
			40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
			50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
			60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
			70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
			80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
			90, 91, 92, 93, 94, 95, 96, 97, 98, 99 
		] 
	},

	"module" : { 
		"model" : "M500R",
		"enabled" : false,
		"tag" : "Relay 2",
		"ilat" : 200,
		"ipre" : 50,
		"imode" : 0,
		"irate" : 4,
		"addr" : [ 1 ] 
	},


	"sw1": { 
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
		"enable_all" : [
			"var i",
			"for (i = 0; i < 160; ++i) {",
			"	sensor[i].enabled = true",
			"	module[i].enabled = true",
			"}"
		],
	},

	"script": {
		"alarm1" : [
			'var i = 0',
			'while (i < 20) {',
			'	if (sensor[i].type == "2251TM") {',
			'		sensor[i].enabled = yes',
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



