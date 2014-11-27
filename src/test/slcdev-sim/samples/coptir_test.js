{
	"info": {
		"desc" : "Simple COPTIR test", 
		"version" : [0, 2]
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
			'timer[1].sec = 3;'
		],

		"usr1" : [
			'var t0, t1, dt, avg;'
			't0 = t1;'
			't1 = ticks();'
			'dt = t1 - t0;'
			'avg = (avg * 3 + dt) / 4;'
			'printf("\nS1: PW3=%d %d %d.", s[1].pw3, dt, avg);',
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

