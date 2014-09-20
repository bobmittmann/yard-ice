//
// Generate the Fibonacci sequence up to the maximum 32 bits signed integer
//

var x0 = 1;
var x1 = 1;
var x2 = 1;
var x3 = 1;
var x4 = 1;
var x5 = 1;
var x6 = 1;
var x7 = 1;
var y0 = 1;
var y1 = 1;
var y2 = 1;
var y3 = 1;
var y4 = 1;
var y5 = 1;
var y6 = 1;
var y7 = 1;
var z0;
var z1;
var z2;
var z3;

var i = 100;

while (i) {
	z0 = x0 * y0 + x1 * y1 + x2 * y2 + x3 * y3;
	z1 = x4 * y4 + x5 * y5 + x6 * y6 + x7 * y7;
	z2 = x0 * y0 + x1 * y1 + x2 * y2 + x3 * y3;
	z3 = x4 * y4 + x5 * y5 + x6 * y6 + x7 * y7;
	i = i - 1;
}

printf("macs = %d\n", 100 * 16);

