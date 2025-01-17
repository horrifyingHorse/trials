import std.stdio;
import std.algorithm;

void main()
{
	string s = "diz crazy dude";
	writeln("sick world ", s);

	writeln("Creating dynamic arrays!");
	char[] c;
	c ~= "a";
	writeln(c);
	c ~= "y";
	writeln(c);
	c ~= "a";
	writeln(c);
	c ~= "a";
	writeln(c);
	c ~= "n";
	writeln(c);

	int[] i = [1, 4, 3, 2];
	i.sort();
	writeln(i.map!(item => item * 2));

	writeln("Enter a string: ");
	s = readln();
	writeln(s[1 .. 3], ": This text has been sliced XD");

	auto conc = (string a, string b) => a ~ b;

	auto concAndRule = (string a, string b) {
		writeln("well, no => is req");
		writeln("Apparently it all works fine here");
		return a ~ b;
	};

	string a = "Sick, ";
	string b = "World!";
	writeln(conc(a, b));
	writeln(concAndRule(a, b));
}
