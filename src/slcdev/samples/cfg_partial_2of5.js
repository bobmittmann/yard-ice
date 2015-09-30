{
	"info": {
		"author" : "Bob",
		"tag" : "2 of 5", 
		"desc" : "2 Photo and 2 Monitor", 
		"version" : [0, 1, 2]
	},

	"sensor" : { 
		"rem" : "Photo 1",
		"model" : "2251B",
		"enabled" : true,
		"ledno" : 1,
		"addr" : [ 3 ]
	},

	"sensor" : { 
		"rem" : "Photo 2",
		"model" : "2251B",
		"enabled" : true,
		"ledno" : 2,
		"addr" : [ 4 ]
	},	

	"module" : { 
		"rem" : "Input Module 1",
		"model" : "M500M",
		"enabled" : true,
		"ledno" : 3,
		"addr" : [ 3 ]
	},

	"module" : { 
		"rem" : "Input Module 2",
		"model" : "M500M",
		"enabled" : true,
		"ledno" : 4,
		"addr" : [ 4 ]
	},	


	"sw1": { 
		"up" : [ 
			'module[3].trouble = 1;',
		],

		"off" : [ 
			'module[3].trouble = 0;',
			'module[3].alarm = 0;',
		],

		"down" : [
			'module[3].alarm = 1;',
		],
	},

	"sw2": { 
		"up" : [ 
			'sensor[4].trouble = 1;'
		],

		"off" : [ 
			'sensor[4].trouble = 0;',
			'sensor[4].alarm = 0;',
		],

		"down" : [
			'sensor[4].alarm = 3;',
		],
	},

}

