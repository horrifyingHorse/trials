import std.stdio;

import utils.matrix;

void main()
{
	auto m = new Matrix2D!int(2, 2);
	m = [[4, 5], [1, 2]];
	m.display();
	writeln(m.determinant());
	m[1, 1] = 9;
	m.display();
	m[0] = [420, 69];
	m.display();

	writeln("Now time for a 3x3");
	m = new Matrix2D!int(3, 3);
	m = [[1, -2, 3], [2, 0, 3], [1, 5, 4]];
	m.display();
	writeln(m.determinant());

	writeln("Now for 4x4");
	auto n = new Matrix2D!int(4, 4);
	n = [
		[4, 3, 2, 2],
		[0, 1, -3, 3],
		[0, -1, 3, 3],
		[0, 3, 1, 1]
	];
	n.display();
	writeln(m.determinant());
	n.transpose().display();

	writeln(m == n);
}
