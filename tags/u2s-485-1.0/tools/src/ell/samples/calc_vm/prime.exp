//
// Print a list of 100 random prime numbers
//
var n, j;	
var prime;

srand(time()); // initialize random number generator

for (j = 0; j < 100; ) {
	n = rand();
	if (n <= 3) {
		prime = n > 1;
		write(n, 3, 3);
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
		print(j, n, prime);
	}
}

