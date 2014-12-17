/* 
   This is a Java Script comment
 */

function set_pw5_all(val)
{
	var i;

	i = 0;
	while (i < 160) {
		sens[i].pw5 = val * sens[i].tbias / 128;
	}
}

function f1(x)
{
	var x0 = 0, x1 = 0;
	var y0 = 0, y1 = 0;
	var y = [];
	var i;

	i = 0;
	while (i < 10) {
		y0 = y1;
		x0 = x1;
		x1 = x[i];
		y1 = (4 * x0 + 3 * x1 - 7 * y0) / 10;
		y[i] = y1;
		i = i + 1;
	}

	return y;
}

function main()
{
	var x = [1, 0, 0, 1, 0, 0, 1, 1, 0, 0];
	var y = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0];

	y = f1(x);

	set_pw5_all(1700);
}

