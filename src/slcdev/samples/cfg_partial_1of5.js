{
	"info": {
		"author" : "Bob",
		"tag" : "1 of 5", 
		"desc" : "2 Photo and 2 Monitor", 
		"version" : [0, 1, 1]
	},

	"sensor" : { 
		"rem" : "Photo 1",
		"model" : "2251B",
		"enabled" : true,
		"ledno" : 1,
		"addr" : [ 1 ]
	},

	"sensor" : { 
		"rem" : "Photo 2",
		"model" : "2251B",
		"enabled" : true,
		"ledno" : 2,
		"addr" : [ 2 ]
	},	

	"module" : { 
		"rem" : "Input Module 1",
		"model" : "M500M",
		"enabled" : true,
		"ledno" : 3,
		"addr" : [ 1 ]
	},

	"module" : { 
		"rem" : "Input Module 2",
		"model" : "M500M",
		"enabled" : true,
		"ledno" : 4,
		"addr" : [ 2 ]
	},	


	"sw1": { 
		"up" : [ 
			'module[1].trouble = 1;',
		],

		"off" : [ 
			'module[1].trouble = 0;',
			'module[1].alarm = 0;',
		],

		"down" : [
			'module[1].alarm = 1;',
		],
	},

	"sw2": { 
		"up" : [ 
			'sensor[1].trouble = 1;'
		],

		"off" : [ 
			'sensor[1].trouble = 0;',
			'sensor[1].alarm = 0;',
		],

		"down" : [
			'sensor[1].alarm = 3;',
		],
	},
}

