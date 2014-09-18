var i = 0;
//var j = 0;

try {
	while (i < 5) {
		i = i + 1;
		if (i > 2) {
			throw 1;
		}
		write(i);
	}
} 

write(9999);
