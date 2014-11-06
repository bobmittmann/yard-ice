//
// Generate the Fibonacci sequence up to the maximum 32 bits signed integer
//

var x;
var x0 = 1;
var x1 = 1;
var x2 = 1;
var x3 = 1;
var y0 = 1;
var y1 = 1;
var y2 = 1;
var y3 = 1;
var i = 100;

while (i) {
	x = x0 * y0 + x1 * y1 + x2 * y2 + x3 * y3;
	i = i - 1;
}

printf("macs = %d\n", 100 * 16);

