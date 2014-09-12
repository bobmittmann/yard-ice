//
// Expression evaluation example
//

var x;
var i;
var odd;
var even;

i = 100;
odd = 0;
even = 0;

srand(time()); // initialize random number generator

while (i) {
	x = rand();
	if (x % 2) {
		odd = odd + 1;
		printf("%08x odd\n", x);
	} else {
		even = even + 1;
		printf("%08x even\n", x);
	}
	i = i - 1;
}

printf("----------------------\n");
printf("%d odd, %d even\n", odd, even);
printf("----------------------\n");
