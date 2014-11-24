{

	"sensor" : { 
		"model" : "2251 COPTIR", 
		"enabled" : true,
		"ledno" : 3,
		"addr" : [ 1 ]
	},

	"sensor" : { 
		"model" : "2251 COPTIR", 
		"enabled" : true,
		"ledno" : 5,
		"addr" : [ 2 ]
	},

	"sensor" : { 
		"model" : "2251 COPTIR", 
		"enabled" : true,
		"ledno" : 2,
		"addr" : [ 3, 4, 5, 6, 7, 8, 9, 10 ]
	},

	"events": { 
		"init" : [
			'var count = 1;',
			'printf("\nHello!\n");',
		],

		"tmr1" : [
			'printf("\nPW3=%d count=%d", s[1].pw3, count);',
			'timer[1].sec = 10;'
		],
	}
	
	"sw1": { 
		"up" : [ 
			'led[4].on = true;',
			'sensor[1].trouble = 1;',
		],

		"off" : [ 
			'/* turn off trouble led */',
			'led[4].on = false;',
			'sensor[1].trouble = 0;',
			'sensor[1].alarm = 0;',
		],

		"down" : [
			'sensor[1].alarm = 1;',
		],
	},

	"sw2": { 
		"up" : [ 
			'led[6].on = true;',
			'sensor[2].trouble = 1;',
			'count = count + 1;'
		],

		"off" : [ 
			'/* turn off trouble led */',
			'led[6].on = false;',
			'sensor[2].trouble = 0;',
			'sensor[2].alarm = 0;',
			'timer[1].sec = 0;'
		],

		"down" : [
			'sensor[2].alarm = 1;',
			'timer[1].ms = 1000;',
		],
	},

}

