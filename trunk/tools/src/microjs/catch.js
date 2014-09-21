var i = 0;
//var j = 0;

srand(time()); // initialize random number generator

try {
	while (i < 200) {
		var x = rand();
		write(i, x);

		if (x % 11 == 0){
			throw 11; // divisible by 11
		}

		if (x % 23 == 0){
			throw 23; /// divisible by 23 
		}

		i = i + 1;
		if (i > 20) {
			throw 20;
		}
	}
	throw 20;
} catch (err) {
	printf("exception %d\n", err);
}

write(9999);
