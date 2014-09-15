var i = 10;

if (i) {
	var x = 2;
	var i = 4;
	i = i - 1;
	printf("\ti=%d x=%d\n", i, x);
} else {
	i = i + 1;
}

printf("\ti=%d\n", i);

print(i);

if (i) {
	i = i - 1;
} else {
	i = i + 1;
}

print(i);
