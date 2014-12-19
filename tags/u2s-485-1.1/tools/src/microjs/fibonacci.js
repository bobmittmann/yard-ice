//
// Generate the Fibonacci sequence up to the maximum 32 bits integer
//

var x0 = 0, x1 = 1, x, i;

for (i = 1; i < 47; i = i + 1) {
	x = x1 + x0;
	x0 = x1;
	x1 = x;
	printf("%2d | %10u\n", i, x);
}

