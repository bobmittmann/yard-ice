var mod_tbl = 0;
var mod_alm = 0;
var sens_tbl = 0;
var sens_alm = 0;

try {
	var i;

	// Process sensors
	for (i = 0; i < 160; i = i + 1) {
		var s;

		s = sensor[i];

		var state = sensor[i].state;
		if (state & 1) {
			sensor[i].alarm = 0;
			sens_alm = sens_alm + 1;
		}
		if (state & 2) {
			sensor[i].trouble = 0;
			sens_tbl = sens_tbl + 1;
		}
	}

	// Process modules
	for (i = 0; i < 160; i = i + 1) {
		var state = module[i].state;
		if (state & 1) {
			module[i].alarm = 0;
			mod_alm = mod_alm + 1;
		}
		if (state & 2) {
			module[i].trouble = 0;
			mod_tbl = mod_tbl + 1;
		}
	}

} catch (err) {
	printf("Error: %d\n", err);
}

