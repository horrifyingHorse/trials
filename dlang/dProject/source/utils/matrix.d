module utils.matrix;

import std.stdio;
import std.conv;

/* By default, the data members and methods are public!
 */
class Matrix2D(T)
{
	/*
	 * T[n][m] matrix -> incorrect, in D : arrays are exclusively
	 * either static(dimensions known at compile time) or dynamic
	 * Since n&m are not known at compile time, they have to be
	 * dynamic!
	 *
	 * it get's even crazier;
	 * Static Declaration -> inside out
	 *		==> [5][3] actually means 3 arrays with 5 elements
	 *
	 * Dynamic Declaration -> natural one
	 *		==> [5][3] means 5 arrays with 3 elements
	 */

	private int n, m;
	private T[][] matrix;

	this(int n, int m)
	{
		this.n = n;
		this.m = m;
		this.matrix = new T[][](n, m);
	}

	/* Op overloading is, kinda makes sense?
	 * Cpp was straightforward with it
	 * here operator=			is		opAssign
	 *			operator[]		is		opIndexAssign		
	 *
	 * I prefer cpp syntax for this one
		*/

	void opAssign(T[][] m)
	{
		ulong row = m.length;
		ulong col = m[0].length;
		if (row != this.n && col != this.m)
		{
			string errString = "Cannot assign the Matrix2D of dims " ~ to!string(
				this.n) ~ "x" ~ to!string(this.m) ~ " to one with dims " ~ to!string(
				row) ~ "x" ~ to!string(col);
			throw new Exception(errString);
		}

		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				this.matrix[i][j] = m[i][j];
			}
		}
	}

	void opIndexAssign(T value, int row, int col)
	{
		if (row * col < 0 || row > this.n || col > this.m)
		{
			throw new Exception("Out of bounds: [" ~ to!string(row) ~ "][" ~ to!string(col) ~ "]");
		}

		this.matrix[row][col] = value;
	}

	void opIndexAssign(T[] value, int row)
	{
		if (row < 0 || row > this.n)
		{
			throw new Exception("Out of bounds: [" ~ to!string(row) ~ "]");
		}
		this.matrix[row] = value;
	}

	bool opEqauls(Matrix2D!(T) matrix)
	{
		int[2] dim = matrix.dim();
		if (this.n != dim[0] && this.m != dim[1])
			return false;

		T[][] rawMatrix = matrix.rawMatrix();

		for (int i = 0; i < this.n; i++)
		{
			for (int j = 0; j < this.m; j++)
			{
				if (this.matrix[i][j] != rawMatrix[i][j])
					return false;
			}
		}

		return true;
	}

	T[][] rawMatrix()
	{
		return this.matrix;
	}

	int[2] dim()
	{
		return [this.n, this.m];
	}

	Matrix2D transpose()
	{
		int row = this.n;
		int col = this.m;
		T[][] newMatrix = new T[][](col, row);
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				newMatrix[j][i] = this.matrix[i][j];
			}
		}

		this.n = col;
		this.m = row;
		this.matrix = newMatrix;

		return this;
	}

	double determinant()
	{
		/* So this is interesting,
		 * the static keyword, is used to specify to compiler
		 * to evaluate this at compile time!
		 * here, if is a runtime statement, while template param
		 * T is a compiler time entity. Now to run this if at
		 * compile time, we use `static if`
		 *
		 * So this will evaluate true anytime a string instance of
		 * this template class is created!
			*/
		static if (is(T == string))
		{
			throw new Exception("Cannot find Determinant of a string matrix");
		}

		if (this.n != this.m)
		{
			throw new Exception("Cannot find Determinant of matrix that is not a Square matrix");
		}

		return calcDeterminant(this.matrix);
	}

	void display()
	{
		foreach (row; matrix)
		{
			foreach (item; row)
			{
				write(item, " ");
			}
			write("\n");
		}
	}

	private double calcDeterminant(T[][] matrix)
	{
		if (matrix.length == 2 && matrix[0].length == 2)
		{
			return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
		}

		double result = 0;
		ulong row = matrix.length;
		ulong col = matrix[0].length;

		for (int j = 0; j < col; j++)
		{
			T[][] dispatchMatrix = new T[][](row - 1, col - 1);
			int dispatchRow = 0, dispatchCol = 0;
			for (int x = 1; x < row; x++)
			{
				for (int y = 0; y < col; y++)
				{
					if (y == j)
						continue;
					dispatchMatrix[dispatchRow][dispatchCol++] = matrix[x][y];
				}
				dispatchCol = 0;
				dispatchRow++;
			}

			double d = (j % 2)
				? -1 * calcDeterminant(dispatchMatrix) : calcDeterminant(dispatchMatrix);
			result += d * matrix[0][j];
		}

		return result;
	}

}
