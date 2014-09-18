//
// Generate the Fibonacci sequence up to the maximum 32 bits signed integer
//

var x0 = 0, x1 = 1, x, i = 1;

while (1) {
	if (0x7fffffff - x1 <= x0) {
		throw 1; // overflow
	}
	x = x1 + x0;
	x0 = x1;
	x1 = x;
	printf("%2d | %10u\n", i, x);
	i = i + 1;
}

