//
// Count the sensors and modules in alarm or trouble
// Clear the troubles and alarms
//

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
		if (sens_state(i) == 1) {
			// clear alarm
			sens_alarm(i, 0);
			// count the alarms
			sens_alrm = sens_alrm + 1;
		} else {
			if (sens_state(i) == 2) {
				// clear the trouble 
				sens_trouble(i, 0);
				// count the trouble 
				sens_trbl = sens_trbl + 1;
			}
		}
	}

	//
	// Process all modules 
	//
	for (i = 0; i < 160; i = i + 1) {
		if (mod_state(i) == 1) {
			// clear alarm
			mod_alarm(i, 0);
			// count the alarms
			mod_alrm = mod_alrm + 1;
		} else {
			if (mod_state(i) == 2) {
				// clear the trouble 
				mod_trouble(i, 0);
				// count the trouble 
				mod_trbl = mod_trbl + 1;
			}
		}
	}

} catch (err) {
	printf("Error: %d\n", err);
}

var trbl;
var alrm;

trbl = mod_trbl + sens_trbl;
alrm = mod_alrm + sens_alrm;

