{
	"info": {
		"author" : "Bob",
		"tag" : "3 of 5", 
		"desc" : "2 Photo and 2 Monitor", 
		"version" : [0, 1, 3]
	},

	"sensor" : { 
		"rem" : "Photo 1",
		"model" : "2251B",
		"enabled" : true,
		"ledno" : 1,
		"addr" : [ 5 ]
	},

	"sensor" : { 
		"rem" : "Photo 2",
		"model" : "2251B",
		"enabled" : true,
		"ledno" : 2,
		"addr" : [ 6 ]
	},	

	"module" : { 
		"rem" : "Input Module 1",
		"model" : "M500M",
		"enabled" : true,
		"ledno" : 3,
		"addr" : [ 5 ]
	},

	"module" : { 
		"rem" : "Input Module 2",
		"model" : "M500M",
		"enabled" : true,
		"ledno" : 4,
		"addr" : [ 6 ]
	},	


	"sw1": { 
		"up" : [ 
			'module[5].trouble = 1;',
		],

		"off" : [ 
			'module[5].trouble = 0;',
			'module[5].alarm = 0;',
		],

		"down" : [
			'module[5].alarm = 1;',
		],
	},

	"sw2": { 
		"up" : [ 
			'sensor[6].trouble = 1;'
		],

		"off" : [ 
			'sensor[6].trouble = 0;',
			'sensor[6].alarm = 0;',
		],

		"down" : [
			'sensor[6].alarm = 3;',
		],
	},

}

