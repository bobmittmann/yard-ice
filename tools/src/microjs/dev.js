//
// Count the sensors and modules in alarm or trouble
// Clear the troubles and alarms
//

srand(time()); // initialize random number generator

try {
	var i;

	for (i = 0; i < 160; i = i + 1) {
		var alm;
		var tbl;

		alm = rand() % 32;
		if (alm < 16) {
			sensor[i].alarm = alm;
		}
		tbl = rand() % 32;
		if (tbl < 16) {
			sensor[i].trouble = tbl;
		}
		printf("%3d: %2d %2d %d\n", i, alm, tbl, sensor[i].state);

		alm = rand() % 32;
		if (alm < 16) {
			module[i].alarm = alm;
		}
		tbl = rand() % 32;
		if (tbl < 16) {
			module[i].trouble = tbl;
		}
		printf("%3d: %2d %2d %d\n", i, alm, tbl, module[i].state);
	}
}

var mod_trbl = 0;
var mod_alrm = 0;
var sens_trbl = 0;
var sens_alrm = 0;

try {
	var i;

	//
	// Process all sensors
	//
	for (i = 0; i < 160; i = i + 1) {
		var s;
		
		s = sensor[i];

		var state = sensor[i].state;
		if (state & 1) {
			// clear alarm
			sensor[i].alarm = 0;
			// count the alarms
			sens_alrm = sens_alrm + 1;
		} 
		if (state & 2) {
			// clear the trouble 
			sensor[i].trouble = 0;
			// count the trouble 
			sens_trbl = sens_trbl + 1;
		}
	}

	//
	// Process all modules 
	//
	for (i = 0; i < 160; i = i + 1) {
		var state = module[i].state;
		if (state & 1) {
			// clear alarm
			module[i].alarm = 0;
			// count the alarms
			mod_alrm = mod_alrm + 1;
		} 
		if (state & 2) {
			// clear the trouble 
			module[i].trouble = 0;
			// count the trouble 
			mod_trbl = mod_trbl + 1;
		}
	}

} catch (err) {
	printf("Error: %d\n", err);
}

var trbl;
var alrm;

trbl = mod_trbl + sens_trbl;
alrm = mod_alrm + sens_alrm;

printf(" - Trouble: %d sensors, %d modules, %3d total.\n", 
	   sens_trbl, mod_trbl, trbl);
printf(" -   Alarm: %d sensors, %d modules, %3d total.\n", 
	   sens_alrm, mod_alrm, alrm);

