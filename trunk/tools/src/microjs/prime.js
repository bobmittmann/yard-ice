//
// Print a list of 100 random prime numbers
//
var j, cnt = 0;

srand(time()); // initialize random number generator

printf("----------------------\n");

for (j = 0; j < 100; ) {
	var n = rand();
	var prime;
	if (n <= 3) {
		prime = n > 1;
	} else {
		if (n % 2 == 0 || n % 3 == 0) {
			prime = false;
		} else {
			var i;
			var m;
			m = sqrt(n) + 1;
			prime = true;
			for (i = 5; (i < m) && (prime); i = i + 6) {
				if (n % i == 0 || n % (i + 2) == 0) {
					prime = false;
				}
			}
		}
	}
	if (prime) {
		j = j + 1;
		printf("%3d %12d\n", j, n);
	}
	cnt = cnt + 1;
}

printf("----------------------\n");

var x = j * 10000 / cnt;

printf("%d out of %d are prime, %d.%d %%.\n", 
	   j, cnt, x / 100, x % 100);

printf("---\n\n");
