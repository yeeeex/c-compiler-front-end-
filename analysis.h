#pragma once
#include<string>

using std::string;

class lex_with_att {
public:
	lex_with_att(lex_with_att& l) :lex(l.lex), att(l.att) {};
	lex_with_att(string&s, int i) :lex(s), att(i) {};
	lex_with_att(char* s, int i) :lex(s), att(i) {};
	string lex;
	int att=-1;
};

class beg_end_quote {
public:
	beg_end_quote(int b, int e) :beg(b), end(e) {};
	beg_end_quote(beg_end_quote& be) :beg(be.beg), end(be.end) {};
	int beg;
	int end;
private:
	beg_end_quote() = default;
};

void read_file_and_analysis(state* dfa_sy, state* dfa_op);