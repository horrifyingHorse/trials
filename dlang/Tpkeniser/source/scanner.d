import token;
import tokenType;
import std.conv;
import std.string;

TokenType[string] keywords = null;

static this()
{
	keywords["let"] = TokenType.LET;
	keywords["const"] = TokenType.CONST;
}

class Scanner
{
	this(string src)
	{
		this.src = src;
	}

	Token[] scanTokens()
	{
		while (1)
		{
			start = current;
			scanToken();
		}

		tokens ~= new Token(TokenType.EOF, "", line);
		return tokens;
	}

private:
	string src;
	Token[] tokens;
	int start = 0, current = 0, line = 1;

	void scanToken()
	{
		char c = advance();
		switch (c)
		{
		case '(':
			addToken(TokenType.LEFT_PAREN);
			break;
		case ')':
			addToken(TokenType.RIGHT_PAREN);
			break;
		case '{':
			addToken(TokenType.LEFT_BRACE);
			break;
		case '}':
			addToken(TokenType.RIGHT_BRACE);
			break;
		case '-':
			addToken(TokenType.MINUS);
			break;
		case '+':
			addToken(TokenType.PLUS);
			break;
		case ';':
			addToken(TokenType.SEMICOLON);
			break;
		case '*':
			addToken(TokenType.STAR);
			break;
		case '!':
			addToken(match('=') ? TokenType.BANG_EQUAL : TokenType.BANG);
			break;
		case '=':
			addToken(match('=') ? TokenType.EQUAL_EQUAL : TokenType.EQUAL);
			break;
		case '<':
			addToken(match('=') ? TokenType.LESS_EQUAL : TokenType.LESS);
			break;
		case '>':
			addToken(match('=') ? TokenType.GREATER_EQUAL : TokenType.GREATER);
			break;
		case ' ':
		case '\r':
		case '\t':
			// Ignore whitespace.
			break;
		case '\n':
			line++;
			break;

		default:
			if (isDigit(c))
			{
				number();
				break;
			}
			else if (isAlpha(c))
			{
				identifier();
				break;
			}
			throw new Exception("Unexpected character.");
		}
	}

	void identifier()
	{
		while (isAlphaNumeric(peek()))
			advance();
		string text = src[start .. current].strip();
		TokenType type = TokenType.IDENTIFIER;
		if (text in keywords)
			type = keywords[text];
		addToken(type);
	}

	void number()
	{
		while (isDigit(peek()))
			advance();

		if (peek() == '.' && isDigit(peekNext()))
		{
			advance();
			while (isDigit(peek()))
				advance();
		}

		/* addToken(TokenType.NUMBER, to!double(src[start .. current])); */
	}

	bool isDigit(char c)
	{
		return c >= '0' && c <= '9';
	}

	bool isAlpha(char c)
	{
		return (c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') || c == '_';
	}

	bool isAlphaNumeric(char c)
	{
		return isAlpha(c) || isDigit(c);
	}

	bool match(char expected)
	{
		if (isAtEnd())
			return false;
		if (src[current] != expected)
			return false;
		current++;
		return true;
	}

	char peek()
	{
		if (isAtEnd())
			return '\0';
		return src[current];
	}

	char peekNext()
	{
		if (current + 1 >= src.length)
			return '\0';
		return src[current + 1];
	}

	char advance()
	{
		return src[current++];
	}

	void addToken(TokenType type)
	{
		string lexeme = src[start .. current];
		tokens ~= new Token(type, lexeme, line);
	}

	bool isAtEnd()
	{
		return current >= src.length;
	}
}
