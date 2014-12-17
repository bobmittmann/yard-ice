#!/usr/bin/octave
%
% File:     sin_gen.m
% Author:   Robinson Mittmann (bobmittmann@gmail.com)
% Comment:
% Copyright(C) 2012 Bob Mittmann. All Rights Reserved.
%
% This program is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
% 
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
% 
% You should have received a copy of the GNU General Public License
% along with this program.  If not, see <http://www.gnu.org/licenses/>.
%
n = 1024 * 1024 * 20;
c = [0.1432 % mean = 0.1
	0.337 % mean = 0.15
	0.5607 % mean = 0.2
	0.8490 % mean = 0.25
	1.2580 % mean = 0.3
	1.9148 % mean = 0.35
	3.2000 % mean = 0.4
	6.9697]; % mean = 0.45

%x0 = c(4);
x0 = 0.625;
x0 = 1;
z = unifrnd(x0, 1 + x0, n, 1);
x = z;
z = unifrnd(x0, 1 + x0, n, 1);
x = x .* z;
z = unifrnd(x0, 1 + x0, n, 1);
x = x .* z;
z = unifrnd(x0, 1 + x0, n, 1);
x = x .* z;
x = x - (x0 ** 4);
x = x * (1 / (((1 + x0) ** 4) - (x0 ** 4)));
k = x;
bins = -0.00:0.001:1;
y = histc(k, bins)';

[m, i] = max(y);

mv = (i - 1) * 0.001;

printf('x0=%f\n', x0);
printf('mean=%f\n', mean(k));
printf('peak=%f\n', mv);
% Plot
plot(bins, y, 'b');
%pause;
%
