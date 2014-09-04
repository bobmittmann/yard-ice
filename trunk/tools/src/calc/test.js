/* 
   This is a Java Script comment
 */

function f1(x)
{
	var x0 = 0, x1 = 0;
	var y0 = 0, y1 = 0;
	var y[10];
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

