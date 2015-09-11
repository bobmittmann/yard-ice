{
	"sw1": { 
		"up" : [ 
			'led[0].on = true;',
			'/* enable all */',
			'var i;',
			'for (i = 0; i < 160; i = i + 1) {',
			'	sensor[i].en = true;',
			'	module[i].en = true;',
			'}'
		],

		"off" : [ 
			'/* turn off leds */',
			'led[0].on = false;',
			'led[1].on = false;'
		],

		"down" : [
			'led[1].on = true;',
			'/* enable all */',
			'var i;',
			'for (i = 0; i < 160; i = i + 1) {',
			'	sensor[i].en = true;',
			'	module[i].en = true;',
			'}'
		],
	},

	"sw2": { 
		"up" : [ 
			'led[2].on = true;',
			'/* trouble 1 */',
			'sensor[1].trouble = 1;'
		],

		"off" : [ 
			'/* turn off leds */',
			'led[2].on = false;',
			'led[3].on = false;',
			'/* clear trouble */',
			'sensor[1].trouble = 0;',
			'/* clear alarm */',
			'sensor[1].alarm = 0;'
		],

		"down" : [
			'led[3].on = true;',
			'/* alarm 3 */',
			'sensor[1].alarm = 3;'
		],
	},

}

