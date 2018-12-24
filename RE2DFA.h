#pragma once
#include<string>
#include<cassert>
#include<deque>
#include<vector>
#include<algorithm>
#include<list>


#if 0
class nfa_index {
public:
	nfa_index(char* c, state* s) :transto(s) {
		memcpy(tranc, c, 4);
	}
	nfa_index(char c, state* s) :transto(s) {
		tranc[0] = c;
		tranc[1] = 0;
		tranc[2] = 0;
		tranc[3] = 0;
	}
	nfa_index(nfa_index&nfa) :transto(nfa.transto), son_index(nfa.son_index) {
		memcpy(tranc, nfa.tranc, 4);
	}
	nfa_index(const nfa_index&nfa) :transto(nfa.transto), son_index(nfa.son_index) {
		memcpy(tranc, nfa.tranc, 4);
	}
	char tranc[4];
	state* transto;
	nfa_index* son_index = nullptr;
};
void analysis_bracket(stack& stack, int cursor, bool left_bracket);
void DFAtoMIN();
void fill_table();
#endif
#define pack__or_char pack_or_char_or_bracket
using std::deque;
using std::string;
typedef string REstr;
class state;

class state_group {
public:
	state_group() {
		memset(transc, 0, 4);
	}
	state_group(char* c) { memcpy(transc, c, 3); transc[3] = 0; };

	char transc[4];
	void erease(state*);
	
	std::vector<state_group*> connectgroup;
	std::list<state*> group;
	std::list<int> num;
};
class edgeto {
public:
	edgeto(state* t1, const char *c) :transto(t1){
		memcpy(transc, c, 3);
		transc[3] = 0;
	};
	edgeto(state* t1, char c) :transto(t1){
		transc[0] = c;
		transc[1] = '\0';
		transc[2] = '\0';
	};
	edgeto(state* t1) :transto(t1) {
		memset(transc, 0, 4);
	};//当空转移时调用这个构造函数
	edgeto(edgeto& e) {
		transto = e.transto;
		memcpy(transc, e.transc, 4);
	}
	edgeto(const edgeto& e) {
		transto = e.transto;
		memcpy(transc, e.transc, 4);
	}
	state* transto;   //转移到哪个state
	char transc[4];//接受到什么符号时转移
private:
	
};
class state {
public:
	state();
	state(const state& s):state_num(s.state_num){}
	state(state& s) :state_num(s.state_num) {}
	int state_num = 0;
	std::vector<state*> parent;
	std::vector<edgeto> edges;
	std::vector<bool> p_in_edges;
	bool is_route = false;
	bool traversal_before = false;
	bool traversal_before1 = false;
	bool set_end_t_b = false;
	bool test_accept_t_b = false;
	int traversal_for_nfa2dfa = 0;
	int can_be_visit_times = 0;
	int self_edge_cursor = 0;
	bool accept_state = false;
	state_group* belong2g = nullptr;
	std::vector<int>find_e_times;
	bool s_or_p_in_g = false;
};
class pack_or_char_or_bracket{
public:
	pack__or_char(string& s) :str(s), c('\0') {
	}
	pack_or_char_or_bracket(string&s ,int cpb):str(s),c_p_b(cpb){}
	pack__or_char(char c) : c(c),c_p_b(0) {}
	pack__or_char(char c,bool ec) : c(c), c_p_b(0), escape_c(ec){}
	pack__or_char(pack__or_char& poc) :str(poc.str), c(poc.c),c_p_b(poc.c_p_b),begin_of_bracket(poc.begin_of_bracket),end_of_bracket(poc.end_of_bracket),escape_c(poc.escape_c) {}
	pack__or_char(const pack__or_char& poc) :str(poc.str), c(poc.c), c_p_b(poc.c_p_b),begin_of_bracket(poc.begin_of_bracket), end_of_bracket(poc.end_of_bracket), escape_c(poc.escape_c) {}
	
	//需要提供一个const版本
	//一定要注意copy构造函数的完整性
	string str;
	char c=0;
	char c_p_b = 0;//如果是字符，cpb=0 方括号cpb=1 圆括号cpb=2;
	state* begin_of_bracket = nullptr;
	state* end_of_bracket = nullptr;
	bool constructed = false;//括号中的括号可能连续2次被用来链接state，这个bool为了说明已经被访问过了
	bool escape_c = false;
private:
	pack__or_char() = default;
};
class stack {
public:
	stack() = default;
	stack(string& str);
	void push(pack__or_char&);
	pack__or_char pop();
	~stack() { }
	std::vector<pack__or_char> stackzone;

	int cursor = 0;
	
};



static char keywords[][13] = {
	"auto","const","extern","register","static","volatile",
	"char","short","int","long","signed","unsigned","float",
	"double","struct","enum","union","void","typedf",
	"continue","break","switch","case","default","do","while",
	"if","else","for","goto",
	"return",
	"nullptr",
	"true","false","class"
	
	 
};
static char operatorsymbol[][5]{
	"\\(\\)","\\[\\]","->","\\(","\\)","\\[","\\]",
	"!","~","\\+\\+","--","\\+","-","\\|\\*","","&",
	"\\*","/","%",
	"\\+","-",
	"<<",">>",
	"<","<=",">",">=",
	"==","!=",
	"&",
	"^",
	"\\|",
	"&&",
	"\\|\\|",
	"\\?",":",
	"=","\\+=","-=","\\*=","/=","&=","^=","\\|=","<<=",">>=",
	",",
	
};
static char newoperatorsymbol[][17]{
	"\\(\\)?","\\[\\]?","\\)","\\]",
	"~","\\+((\\+)|=)?","-(-|>|=)?","\\|((\\|)|=)?",
	"\\*=?","/=?","%",
	"<(<=?|=)?",">((>=?)|=)?",
	"==?","!=?",
	"^=?",
	"&(&|=)?",
	"\\?",":",
	","

}; //(\\+)被忽略了
static char preprocess_s[] = "#";
static char text_body[][2] = { "{", "}" };
static char long_operatorsymbol[][10] = { "sizeof" };
static char semi[] = ";";


void analysis_comp(stack& stack, int left_bracket);
void analysis_when__left_bracket_0(pack__or_char&,bool);
state* analysis_when__left_bracket_0(pack__or_char&);/*带返回值的版本*/
void analysis_symbol(std::vector<pack_or_char_or_bracket>& container, int pos, char c);

void REtoNFA(REstr& re);
void RFA2DFA(int next_num, state* s0, bool if_sy, state* &dfa_s0);


void printf_nfa_table(state&s0, int next_state_num);
void release_nfa_memory(state& s0);
void printf_nfa_table_op(state&s0, int next_state_num);

void achieve_nfa_sy(state& s_start, state*&);
void achieve_nfa_op(state& s_start, state*&);


void traversal_state(state* s);
void traversal_set_0(state* s);