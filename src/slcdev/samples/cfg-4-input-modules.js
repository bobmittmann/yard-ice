{
	"info": {
		"tag" : "cfg-4-input-modules.js", 
		"author" : "Bob Mittmann", 
		"panel" : "Flexnet", 
		"version" : [0, 1]
		"desc" : "4 input modules", 
	},

	"module" : { 
		"rem" : "Input Module",
		"model" : "M500M",
		"enabled" : true,
		"ledno" : 3,
		"group" : [ 1 ],
		"addr" : [ 1 ]
	},

	"module" : { 
		"rem" : "Input Module",
		"model" : "M500M",
		"enabled" : true,
		"ledno" : 5,
		"group" : [ 1 ],
		"addr" : [ 2 ]
	},

	"module" : { 
		"rem" : "Input Module",
		"model" : "M500M",
		"enabled" : true,
		"ledno" : 4,
		"group" : [ 1 ],
		"addr" : [ 3 ]
	},

	"module" : { 
		"rem" : "Input Module",
		"model" : "M500M",
		"enabled" : true,
		"ledno" : 6,
		"group" : [ 1 ],
		"addr" : [ 4 ]
	},

	"module" : { 
		"rem" : "AP Input Module",
		"model" : "AP-Monitor-Module",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 2 ],
		"addr" : [ 101 ]
	},
	
	"sw1": { 
		"up" : [ 
			'module[3].alarm = 1;'
		],

		"off" : [ 
			'module[3].alarm = 0;',
			'module[1].alarm = 0;'
		],

		"down" : [
			'module[1].alarm = 1;'
		],
	},

	"sw2": { 
		"up" : [ 
			'module[4].alarm = 1;'
		],

		"off" : [ 
			'module[4].alarm = 0;',
			'module[2].alarm = 0;'
		],

		"down" : [
			'module[2].alarm = 1;'
		],
	},

	"trigger" : { 
		"sensor" : true,
		"addr" : 1,
		"script" : [
			'led[1].flash(200);',
		],
	},
}

