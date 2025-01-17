# Trying to make sense of DLang
- `dmd`      -> _official_ __compiler__
- `dub`      -> _official_ __package manager__ (Did not expect it to have one)
- `serve-d`  -> LSP

## Installation:
```bash
sudo pacman -S dlang-dmd
dub fetch serve-d	# LSP for dlang
```

## Language Features:
- Similar to C/C++ & Java(GC moment)
- __Dynamic Arrays__ are __possible__ (thnx to `~=` op)
```d
import std.stdio;

void main() {
	writeln("Creating dynamic arrays!");
	char[] c;
	c ~= "a";       // ~= operator appends an array!
	writeln(c);
	c ~= "y";
	writeln(c);
	c ~= "a";
	writeln(c);
	c ~= "a";
	writeln(c);
	c ~= "n";
	writeln(c);
}
```
- __std.algorithm__ includes: _array manipulations_
```d
import std.stdio;
import std.algorithm;

void main() {
    int[] i = [1, 2, 3, 4];
    i.sort();
    write(i.map!(item => item * 2));
}
```
- __string handling__
```d
import std.stdio

void main() {
	writeln("Enter a string: ");
	s = readln();
	writeln(s[1 .. 3], ": This text has been sliced XD");
    // Slices from 1 upto 3 ie. displays s[1] + s[2]
}
```
- _cat_ operator (`~`)
  - concatenates two arrays
```d
	string a = "Sick, ";
	string b = "World!";
	auto result = a ~ b;	

```

- __lambda functions()__
```d
import std.stdio

void main() {
	// single statement lambda
	auto conc = (string a, string b) => a ~ b;
	
	// multi statement lambda
	auto concAndRule = (string a, string b) {
		writeln("well, no => is req");
		writeln("Apparently it all works fine here");
		return a ~ b;
	};

	string a = "Sick, ";
	string b = "World!";
	writeln(conc(a, b));
}
```



## dub
> Create A Project
```bash
dub init projectName
```
> Install Dependencies
```bash
dub upgrade
```
> Run || Build A Project
```bash
dub run     # builds and run
dub build
```
> Intall || Uninstall A Module
```bash
dub fetch name
dub remove name
```

