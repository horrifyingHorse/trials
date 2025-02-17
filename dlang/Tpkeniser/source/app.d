import std.stdio;
import std.string;

void main()
{
	int[string] k;
	k["lol"] = 1;
	k["ok"] = 3;
	string s;
	write("Enter to search: ");
	s = readln().strip();

	if (s in k)
	{
		int i = k[s];
		writeln(i);
	}
}
