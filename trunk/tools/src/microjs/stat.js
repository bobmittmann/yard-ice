//
// Generate the Fibonacci sequence up to the maximum 32 bits signed integer
//

var trbl;
var alrm;
var sens_alrm;
var sens_trbl;
var mod_alrm;
var mod_trbl;

try {
	if (trbl != sens_trbl + mod_trbl) {
		throw("TROUBLE");
	}
	if (alrm != sens_alrm + mod_alrm) {
		throw("ALARM");
	}

	printf("%s: trbl=%d alrm=%d!\n", "sensors", sens_trbl, sens_alrm);
	printf("%s: trbl=%d alrm=%d!\n", "modules", sens_trbl, sens_alrm);
	printf("%s: trbl=%d alrm=%d!\n", "total", trbl, alrm);

} catch(err) {
	printf("#error: inconsistent %s stats!\n", err);
}

