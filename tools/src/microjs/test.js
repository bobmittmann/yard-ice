/* 
   This is a Java Script comment
 */

function main(argv, argc)
{
	var i;

	for (i = 0; i < 0x30; ++i) {
		printf("This is a test: %d", i);
	}
	return 0;
}

// one line comment
dev = sens[4];
dev.pw5 = 4;
dev.pw5.lookup(32);

dev = sens[4];

sens.[5].pw3 = 0xffff;


function enable_all() 
{
	var i; // end of line comment

	for (i = 0; i < 160; ++i) {
		sens[i].enabled = true;
		mod[i].enabled = true;	
	}
}

function disable_all() 
{
	var i;

	for (i = 0; i < 160; ++i) {
		sens[i].enabled = false;
		mod[i].enabled = false;	
	}
}

function alarm(dev) 
{
	if (dev.enabled) {
		dev.pw5.lookup(2);
		dev.pw2.lookup(2);
	}
}

function alarm_all() 
{
	var i;

	for (i = 0; i < 160; ++i) {
		alarm(sens[i]) 
	}
}

function integrate(x, dt) 
{
	var y;
	var i;

	y = 0;
	for (i = 0; i < x.length; ++i) {
		y = y + x[i]; 
	}

	return y / dt;
}

function average(x) 
{
	var y;
	var i;
	var n;

	n = x.length;
	y = 0;
	for (i = 0; i < n; ++i) {
		y = y + x[i]; 
	}

	return y / n;
}

function sqrt(x)
{
	var rem = 0;
	var root = 0;
	var i;

	for (i = 0; i < 16; ++i) {
		root <<= 1;
		rem = ((rem << 2) + (x >> 30));
		x <<= 2;
		root++;
		if (root <= rem) {
			rem -= root;
			root++;
		} else
			root--;
	}

	return root >> 1;
}	

function f1(x, y) 
{
	return sqrt(x * x + y * y);
}

function log2(v)
{
	var log2_debruijn[32] = {
		0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
		31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
	};

	var x;

	v |= v >> 1; // first round down to power of 2 
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v = (v >> 1) + 1;

	x = (v * 0x077cb531) >> 27;

	return log2_debruijn[x];
}

function case_test(x)
{
	var y = 0;

	switch (x) {
	case 7:
		y = 14;
		break;
	case 6: 
		y = 1;
		break;
	}

	return y;

}

// this is a end of file comment with no end of line
