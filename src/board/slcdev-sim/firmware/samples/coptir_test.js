{
	"info": {
		"desc" : "Simple COPTIR test", 
		"version" : [0, 2, 1]
	},

	"sensor" : { 
		"model" : "2251 COPTIR", 
		"enabled" : true,
		"ledno" : 3,
		"group" : [1, 2],
		"addr" : [ 1 ]
		"event" : "usr1"
	},

	"sensor" : { 
		"model" : "2251 COPTIR", 
		"enabled" : true,
		"ledno" : 5,
		"group" : [1, 2],
		"addr" : [ 2 ]
		"event" : "usr2"
	},

	"sensor" : { 
		"model" : "2251 COPTIR", 
		"enabled" : true,
		"ledno" : 2,
		"group" : [1, 3],
		"addr" : [ 3, 4, 5, 6, 7, 8, 9, 10 ]
	},

	"module" : { 
		"rem" : "Sample Input Module",
		"model" : "M500M",
		"enabled" : false,
		"group" : [ 4 ],
		"addr" : [ 1, 2, 4, 8 ]
	},

	"events": { 
		"init" : [
			'printf("\nSimple COPTIR test!\n");',
		],

		"tmr1" : [
			'printf("\nPW3=%d %d.", s[1].pw3, ticks());',
		],

		"usr1" : [
			'var t0, t1;'
			't0 = t1;'
			't1 = ticks();'
			'printf("\nS1: PW4=%d dt=%d cnt=%d", s[1].pw4, t1 - t0, s[1].pcnt);',
		],
		"usr2" : [
			'var t2, t3;'
			't2 = t3;'
			't3 = ticks();'
			'printf("\nS2: PW4=%d dt=%d cnt=%d", s[2].pw4, t3 - t2, s[2].pcnt);',
		],
	}
	
	"sw1": { 
		"up" : [ 
			'led[4].on = true;',
			'sensor[1].trouble = 4;',
		],

		"off" : [ 
			'/* turn off trouble led */',
			'led[4].on = false;',
			'sensor[1].trouble = 0;',
			'sensor[1].alarm = 0;',
		],

		"down" : [
			'sensor[1].alarm = 2;',
		],
	},

	"sw2": { 
		"up" : [ 
			'led[6].on = true;',
			'sensor[2].trouble = 4;',
		],

		"off" : [ 
			'/* turn off trouble led */',
			'led[6].on = false;',
			'sensor[2].trouble = 0;',
			'sensor[2].alarm = 0;',
		],

		"down" : [
			'sensor[2].alarm = 2;',
		],
	},

}

