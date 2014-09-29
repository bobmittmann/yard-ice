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
		printf("%3d: %2d %2d %d -", i, alm, tbl, sensor[i].state);
		var j;
		for (j = 0; j < 4; j = j + 1) {
			sensor[i].level[j] = rand() % 256;
			printf(" %3d", sensor[i].level[j]);
		}
		printf("\n");


		alm = rand() % 32;
		if (alm < 16) {
			module[i].alarm = alm;
		}
		tbl = rand() % 32;
		if (tbl < 16) {
			module[i].trouble = tbl;
		}
		printf("%3d: %2d %2d %d -", i, alm, tbl, module[i].state);
		var j;
		for (j = 0; j < 4; j = j + 1) {
			module[i].level[j] = rand() % 256;
			printf(" %3d", module[i].level[j]);
		}
		printf("\n");
	}


}

var mod_tbl = 0;
var mod_alm = 0;
var mod_lvl = 0;
var sens_tbl = 0;
var sens_alm = 0;
var sens_lvl = 0;

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
			sens_alm = sens_alm + 1;
		} 
		if (state & 2) {
			// clear the trouble 
			sensor[i].trouble = 0;
			// count the trouble 
			sens_tbl = sens_tbl + 1;
		}

		var j;
		for (j = 0; j < 4; j = j + 1) {
			if (sensor[i].level[j] >= 128) {
				sens_lvl = sens_lvl + 1;
			}
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
			mod_alm = mod_alm + 1;
		} 
		if (state & 2) {
			// clear the trouble 
			module[i].trouble = 0;
			// count the trouble 
			mod_tbl = mod_tbl + 1;
		}

		var j;
		for (j = 0; j < 4; j = j + 1) {
			if (module[i].level[j] >= 128) {
				mod_lvl = mod_lvl + 1;
			}
		}
	}

} catch (err) {
	printf("Error: %d\n", err);
}

var tbl;
var alm;
var lvl;

tbl = mod_tbl + sens_tbl;
alm = mod_alm + sens_alm;
lvl = mod_lvl + sens_lvl;

printf(" - Trouble: %d sensors, %d modules, %3d total.\n", 
	   sens_tbl, mod_tbl, tbl);
printf(" -   Alarm: %d sensors, %d modules, %3d total.\n", 
	   sens_alm, mod_alm, alm);
printf(" -   Level: %d sensors, %d modules, %3d total.\n", 
	   sens_lvl, mod_lvl, lvl);

