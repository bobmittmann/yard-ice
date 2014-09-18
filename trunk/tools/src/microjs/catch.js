var i = 0;
//var j = 0;

try {
	while (i < 13) {
		i = i + 1;
		if (i > 12) {
			throw 11;
		}
		write(i);
	}
} catch (err) {
	printf("exception %d\n", err);
}

write(9999);
