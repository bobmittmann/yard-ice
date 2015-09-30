{
	"info": {
		"author" : "Bob",
		"tag" : "4 of 5", 
		"desc" : "2 photo and 2 monitor", 
		"version" : [0, 1, 4]
	},

	"sensor" : { 
		"rem" : "Photo 1",
		"model" : "2251B",
		"enabled" : true,
		"ledno" : 1,
		"addr" : [ 7 ]
	},

	"sensor" : { 
		"rem" : "Photo 2",
		"model" : "2251B",
		"enabled" : true,
		"ledno" : 2,
		"addr" : [ 8 ]
	},	

	"module" : { 
		"rem" : "Input Module 1",
		"model" : "M500M",
		"enabled" : true,
		"ledno" : 3,
		"addr" : [ 7 ]
	},

	"module" : { 
		"rem" : "Input Module 2",
		"model" : "M500M",
		"enabled" : true,
		"ledno" : 4,
		"addr" : [ 8 ]
	},	


	"sw1": { 
		"up" : [ 
			'module[7].trouble = 1;',
		],

		"off" : [ 
			'module[7].trouble = 0;',
			'module[7].alarm = 0;',
		],

		"down" : [
			'module[7].alarm = 1;',
		],
	},

	"sw2": { 
		"up" : [ 
			'sensor[8].trouble = 1;'
		],

		"off" : [ 
			'sensor[8].trouble = 0;',
			'sensor[8].alarm = 0;',
		],

		"down" : [
			'sensor[8].alarm = 3;',
		],
	}
}

