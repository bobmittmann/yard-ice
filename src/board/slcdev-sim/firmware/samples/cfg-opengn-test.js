{
	"info": {
		"tag" : "OpenGN Test", 
		"author" : "Bob Mittmann", 
		"panel" : "Flexnet", 
		"version" : [0, 1]
		"desc" : "Config used to test OpenGN network problems", 
	},

	"sensor" : { 
		"rem" : "Photo",
		"model" : "2251B",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 1 ],
		"addr" : [ 1 ]
	},

	"sensor" : { 
		"rem" : "Photo",
		"model" : "2251TB",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 1 ],
		"addr" : [ 2 ],
	},

	"sensor" : { 
		"rem" : "Heat",
		"model" : "5251B",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 1 ],
		"addr" : [ 3 ],
	},

	"sensor" : { 
		"rem" : "Heat",
		"model" : "5251RB",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 1 ],
		"addr" : [ 4 ]
	},

	"sensor" : { 
		"rem" : "Heat",
		"model" : "5251H",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 1 ],
		"addr" : [ 5 ]
	},

	"sensor" : { 
		"rem" : "Ion (DNRW)",
		"model" : "1251B",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 1 ],
		"addr" : [ 6 ]
	},

	"sensor" : { 
		"rem" : "Photo (DNR)",
		"model" : "2251BR",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 1 ],
		"addr" : [ 7 ]
	},


	"sensor" : { 
		"rem" : "Acclimate",
		"model" : "2251TM",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 1 ],
		"addr" : [ 8 ]
	},

	"module" : { 
		"rem" : "Input Module",
		"model" : "M500M",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 2 ],
		"addr" : [ 1 ]
	},

	"module" : { 
		"rem" : "Supervised Output Module",
		"model" : "M500S",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 2 ],
		"addr" : [ 2 ]
	},

	"module" : { 
		"rem" : "Relay Module",
		"model" : "M500R",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 2 ],
		"addr" : [ 3 ] 
	},

	"module" : { 
		"rem" : "Input Module",
		"model" : "M501M",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 2 ],
		"addr" : [ 4 ]
	},

	"module" : { 
		"rem" : "Telephone Module",
		"model" : "M500FP",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 2 ],
		"addr" : [ 5 ] 
	},

	"module" : { 
		"rem" : "M500DMR1 (Output)",
		"model" : "M500R",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 2, 5 ],
		"addr" : [ 6 ]
	},

	"module" : { 
		"rem" : "M500DMR1 (Input)",
		"model" : "M500M",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 2, 5 ],
		"addr" : [ 7 ]
	},

	"module" : { 
		"rem" : "Dual Input Module",
		"model" : "M500DM",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 2 ],
		"addr" : [ 8, 9 ]
	},

	"module" : { 
		"rem" : "10x Input Module",
		"model" : "IM-10",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 2, 3],
		"addr" : [ 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 ]
	},

	"module" : { 
		"rem" : "6x Relay Module",
		"model" : "CR-6",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 2, 4 ],
		"addr" : [ 20, 21, 22, 23, 24, 25 ] 
	},

	"module" : { 
		"rem" : "6x Supervised Output Module",
		"model" : "SC-6",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 2, 5 ],
		"addr" : [ 30, 31, 32, 33, 34, 35 ]
	},

	"module" : { 
		"rem" : "6x Conventional Zone Interface Module",
		"model" : "CZ-6",
		"enabled" : true,
		"ledno" : 2,
		"group" : [ 2, 6 ],
		"addr" : [ 40, 41, 42, 43, 44, 45 ]
	},


	
	"sw1": { 
		"up" : [ 
			'led[4].on = true;'
		],

		"off" : [ 
			'led[4].on = false;',
			'led[3].on = false;'
		],

		"down" : [
			'led[3].on = true;'
		],
	},

	"sw2": { 
		"up" : [ 
			'led[6].on = true;'
		],

		"off" : [ 
			'led[6].on = false;',
			'led[5].on = false;'
		],

		"down" : [
			'led[5].on = true;'
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

