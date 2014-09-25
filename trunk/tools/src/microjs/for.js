var i, j, k;

for (i = 0; i < 20; i = i + 1) {
	print(i);
}

k = 0;

for (i = 10, j = 0; i < 40; i = i + 1, j = j + 2) {
	if (i < j) {
		for (k = 0; k < 10; k = k + 1) {
			print(i, j, k);
		}
	} else {
		print(1 << k);
		k = k + 1;
	}
}

for (i = 0; i < 4; i = i + 1) {
	for (j = 0; j < 4; j = j + 1) {
		for (k = 0; k < 4; k = k + 1) {
			print(i, j, k);
		}
	}
}

