#pragma once
#include<list>
#include<string>
#include<initializer_list>
using std::string;


class token {
public:
	token(char* s,bool i) :self(s),S_T(i){
		memset(pos, 0, 40);
	};
	token(char* s) :self(s) {
		memset(pos, 0, 40);
	};

	bool visit_b = false;
	bool S_T=0;  // 0 ∑«÷’Ω·  1 ÷’Ω·
	string self;
	int pos[10];
	std::list<std::list<token*>> all_deduce;
};

class first_search_result_store {
public:
	void search(token* t);
	void push(token* t);
	std::list<token*>&& pop_all();
	std::list<string> FSET;
	int pos = 0;
	
private:
	token * now_search = nullptr;
	std::list<token*> search_result;
	
};

class FIRST_SET {
public:
	FIRST_SET(token* t):S(t){}
	FIRST_SET(token* t, std::list<string>& l) :S(t),SET(l){}
	FIRST_SET(const FIRST_SET& f) :S(f.S), SET(f.SET) {}
	FIRST_SET(FIRST_SET& f) :S(f.S), SET(f.SET) {}
	token* S;
	std::list<string> SET;
};

void test_FIRST();