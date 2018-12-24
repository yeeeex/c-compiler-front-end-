#pragma once
#include"RE2DFA.h"
#include<vector>
#include<list>
#if 0
class transto_without_0 {
public:
	transto_without_0(int i,state* s){
		state_num = i;
		to.push_back(s);
	}
	void push(state* s) { to.push_back(s); }
	int state_num;
	
	std::vector<state*> to;

	transto_without_0() = default;
};
class index_of_state {
public:
	index_of_state(int i) :state_num(i) {}
	index_of_state(index_of_state& rhs) {
		state_num = rhs.state_num;
		next = rhs.next;
	};
	index_of_state(const index_of_state& rhs) {
		state_num = rhs.state_num;
		next = rhs.next;
	};
	int state_num;
	std::vector<_son_of_state_to> next;
private:
	index_of_state() = default;
};
class last_visit {
public:
	last_visit * before;
	state* self;
	last_visit* next;
};
class state_call_stack_header {
public:
	state_call_stack * header = nullptr;
};
class parent_erease_index {
public:
	parent_erease_index(state* i) :parent_need_e(i) {};
	state* parent_need_e = nullptr;
	std::vector<std::vector<edgeto>::iterator> son_erease;
};
#endif
class _son_of_state_to {
public:
	_son_of_state_to(int p) :pos(p) {
		str[0] = 0; str[1] = 0; str[2] = 0; str[3] = 0;
	}
	_son_of_state_to(int p, char*s) :pos(p) {
		memcpy(str, s, 3);
		str[3] = 0;
	}
	_son_of_state_to(char* s) :pos(-1) {
		memcpy(str, s, 3);
		str[3] = 0;
	}
	_son_of_state_to(_son_of_state_to& r) {
		pos = r.pos;
		memcpy(str, r.str, 4);
	}
	_son_of_state_to(const _son_of_state_to& r) {
		pos = r.pos;
		memcpy(str, r.str, 4);
	}
	int pos;
	char str[4];
};

class _son_of_state_to_with_pointer:public _son_of_state_to
{
public:
	_son_of_state_to_with_pointer(char* s, state* s1):_son_of_state_to(s), company_nfa(s1){}
	_son_of_state_to_with_pointer(int p, state* s) :_son_of_state_to(p) {
		company_nfa = s;
	}
	_son_of_state_to_with_pointer(int p,char* str,state* s):_son_of_state_to(p,str), company_nfa(s){}
	_son_of_state_to_with_pointer(_son_of_state_to_with_pointer& s):_son_of_state_to(s),company_nfa(s.company_nfa){}
	state* company_nfa = nullptr;
private:
};

class state_to {
public:
	void push(_son_of_state_to& r) {
		cursor.push_back(r);
	}
	int traversal_before = 0;
	std::vector<_son_of_state_to> cursor;
};

class state_call_stack {
public:
	
	state_call_stack* next = nullptr;
	std::vector<int> state_n;
};

class group_need_er {
public:
	group_need_er(state_group* s):sg(s){}
	group_need_er(const group_need_er &e):sg(e.sg),n_e(e.n_e){}
	group_need_er(group_need_er &e) : sg(e.sg), n_e(e.n_e) {}
	state_group* sg = nullptr;
	std::vector<state*>n_e;
};



