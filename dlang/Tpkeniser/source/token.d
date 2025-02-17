import tokenType;
import std.stdio;

class Token
{
	private
	{
		TokenType type;

		string lexeme;
		int line;
	}

	this(TokenType type, string lexeme, int line)
	{
		this.type = type;
		this.lexeme = lexeme;
		this.line = line;
	}

	string view()
	{
		return lexeme;
	}
}
