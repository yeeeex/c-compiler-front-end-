#include"stdafx.h"
#include"RE2DFA.h"
#include<iostream>
using std::cout;
using std::endl;

#define DEBUG
#define beparent(paren,son)\
 son->parent.push_back(paren);\
 paren->edges.push_back(edgeto(son));\


#define beparent_1(paren,son,c)\
son->parent.push_back(paren);\
paren->edges.push_back(edgeto(son,c));



// debug mirco

#define cout_pos_container(container)\
for(auto& r:container){\
    switch (r.c_p_b){\
		case 0: cout<<r.c;break;\
        case 1: cout<<r.str;break;\
        case 2: cout<<r.str;break;\
        default: break;\
	}\
}\
cout<<endl;

REstr teststr1("a");
REstr teststr2("a+b");
REstr teststr3("a*");
REstr teststr4("a?");
REstr teststr5("ca+b*d?");
REstr teststr6("a+|b|c");
REstr teststr7("((ab)+|c*)|ab");
REstr teststr8("(ab)+c");
REstr teststr9("[a-z][0-9]");
REstr teststr10("\\+((\\+)|=)?	");
//

REstr identifier_str1("");
REstr identifier_str("($|_|[a-z]|[A-Z])($|_|[a-z]|[A-Z]|[0-9])*");
REstr ident_filename("($|_|[a-z]|[A-Z]|[0-9])+.[a-z]+");

#if 0
state s0;
#endif
int next_state_num = 0;
state* last_state;



void stack::push(pack__or_char& poc) {
	stackzone.push_back(poc);
	cursor++;
}
pack__or_char stack::pop(){
	
		pack__or_char rtemp(stackzone[cursor - 1]);
		stackzone.pop_back();
		cursor--;
		return rtemp;
	
	
}
stack::stack(string& str) {
	for (auto& r : str) {
		stackzone.push_back(pack__or_char(r));
	}
	cursor=str.size();
}
state::state() {
	state_num = next_state_num++;
	cout << "now is:"<<state_num << endl;
	
}



static void analysis_comp(stack& stack,int left_barcket) {
	stack.pop();
	std::vector<pack__or_char> items;
	do {
		cout_pos_container(stack.stackzone);
		items.push_back(stack.pop());
		cout_pos_container(stack.stackzone);
		
	} while (!((items.end() - 1)->c == '[' && !(items.end() - 1)->escape_c)); //[]内只能跟字符，不能再包含[]，只能[a-z]之类的格式
	items.pop_back();
	string strtemp;
	for(auto&r:items){
		strtemp.push_back(r.c);
	}
	std::reverse(strtemp.begin(), strtemp.end());
	
	stack.push(pack__or_char(strtemp,1));
	if (left_barcket == 0) {
		analysis_when__left_bracket_0(pack__or_char(strtemp,1),1);
	}

}
//将形如[a-z]格式的正则变成一个整体,毕竟和单个a表示的一样是单个字符的转移


static void analysis_bracket(stack& stack1, int cursor,int have_left_bracket) {
	//若果是括号内的括号，将只会构造一个闭包，而不会链接
	std::vector<pack__or_char>items;
	stack1.pop();
	std::vector<std::vector<pack_or_char_or_bracket>> or_branch;
	do {
		items.push_back(stack1.pop());
	} while (!((items.end() - 1)->c == '('&& !(items.end() - 1)->escape_c));
	int left_or = 0;
	items.pop_back();
	std::reverse(items.begin(), items.end());
	state* blank1 = new state();
	state* end1 = new state();
	state* temp_last = nullptr;
	auto beg = items.begin();
	bool have_or_before = false;
	for (auto r = items.begin();; r++) {
		if (r != items.end()) {
			if (r->c_p_b == 0) {
				if (r->c == '|'&&!r->escape_c) {
					have_or_before = true;
					or_branch.push_back(std::vector<pack_or_char_or_bracket>(beg, r));
					beg = r + 1;
				}
			}
			continue;
		}
		else {
			if (have_or_before)
			{
				or_branch.push_back(std::vector<pack_or_char_or_bracket>(beg, r));
			}
			break;
		}
	}
	//test how to divided?
	if (!or_branch.empty()) {
#if 0 
		cout << "test branch" << endl;
		for (auto &r : or_branch) {
			cout_pos_container(r);
		}
#endif // DEBUG
		beparent(last_state, blank1);
		last_state = blank1;
		state* last_state1 = last_state;
		int siz = 0;
		for (auto& branch : or_branch)
		{// last_state 应该指向的是上一块()的开始，实际指向这一块内存的开始
			state* newbegin = new state();
			beparent(last_state, newbegin);
			last_state = newbegin;
		
			stack realstack;
			state* temp;
			auto r = branch.begin();
			for (; r != branch.end(); r++)
			{
				realstack.push(pack__or_char(*r));
				if (r->c_p_b == 0 && r->c == '*'&&!r->escape_c) {
					realstack.pop();
					analysis_symbol(realstack.stackzone, realstack.cursor, r->c);
				}
				if (r->c_p_b == 0 && r->c == '?'&&!r->escape_c) {
					realstack.pop();
					analysis_symbol(branch, realstack.cursor, r->c);
				}
				if (r->c_p_b == 0 && r->c == '+'&&!r->escape_c) {
					realstack.pop();
					analysis_symbol(branch, realstack.cursor, r->c);
				}
				else {
					temp = analysis_when__left_bracket_0(pack__or_char(*r));
					
				}
				
			}
			beparent(last_state, end1);
			siz++;
			
			last_state = last_state1;
			
			
			
		}
		
		
		//对每个分支进行单独分析，完成后要将整个()内的东西组合为一个pack_or_char
	}
	/*形如(abc(de(f))g)的格式，并不需要知道de里面还有没有括号，只需要知道de的出发和
	结束state就行了*/
	else {
		if (!have_left_bracket) {
			beparent(last_state, blank1);
			last_state = blank1;
		}
		else {
			temp_last = last_state;
			last_state = blank1;
		}
		
		
		auto r = items.begin();
		for (; r != items.end(); r++) {
			
			    if (r->c_p_b==0&&r->c == '*'&&!r->escape_c) {
					
					analysis_symbol(items, r-items.begin(), r->c);
				}
				if (r->c_p_b == 0 && r->c == '+' && !r->escape_c) {
					analysis_symbol(items, r - items.begin(), r->c);
				}
				if (r->c_p_b == 0 && r->c == '?' && !r->escape_c) {
					analysis_symbol(items, r - items.begin(), r->c);
				}
				else {
					
					analysis_when__left_bracket_0(*r,1);
				}
			}
		
			
		}


	
	string pop_out_str;
	for (auto&r : items) {
		if (r.c_p_b == 0)
			pop_out_str.push_back(r.c);
		else if(r.c_p_b==1||r.c_p_b==2)
			for (auto&q : r.str)
				pop_out_str.push_back(q);
	}
	pack_or_char_or_bracket push_in(pop_out_str,2);
	push_in.begin_of_bracket = blank1;

#ifdef debugmsg
	cout << "beg: " << push_in.begin_of_bracket->state_num << endl;
#endif // debugmsg

	
	push_in.end_of_bracket = end1;
	if (temp_last != nullptr) {
		
		last_state = temp_last;
	}
	else {
		
		last_state = end1;
}
	push_in.c_p_b = 2;
	push_in.constructed = true;
	
	stack1.push(push_in);
	 //如果括号内没有分支，就用else构造NFA，有分支就用if里的
}
//将整个括号弹出，进行递归分析，顺便构建括号的NFA

static void analysis_when__left_bracket_0(pack__or_char& poc,bool changelast){
	// change last =false 更新last_stae，否则不更新
	
	if (poc.c_p_b == 2) {
		beparent(last_state, poc.begin_of_bracket);
#ifdef debugmsg
		cout << "treat:" << poc.str << "cpb:" << poc.c_p_b << endl;
#endif // debugmsg

		
		
		if (changelast) {
			last_state = poc.end_of_bracket;
		}
		return;
	}
	state* _from = new state();
	state* _to = new state();
	if (poc.str.empty()&&poc.c_p_b==0)
	{
#ifdef debugmsg
		cout << "treat:" << poc.c << endl;
#endif
		beparent_1(_from, _to,poc.c);
	}
	else if(poc.c_p_b==1) {
#ifdef debugmsg
		cout << "treat:" << poc.str << endl;
#endif
		beparent_1(_from, _to,poc.str.c_str());
	}
	
	
	beparent(last_state,_from);
	if (changelast) {
		last_state = _to;
	}
}
//当字节不在括号中时，将每个字符串用来构造成NFA
static state* analysis_when__left_bracket_0(pack__or_char& poc) {
	
		
	if (poc.c_p_b == 2) {
		beparent(last_state, poc.begin_of_bracket);
		
		last_state = poc.end_of_bracket;
		
		return poc.end_of_bracket;
	}
	state* _from = new state();
	state* _to = new state();
	if (poc.str.empty() && poc.c_p_b == 0)
	{
		beparent_1(_from, _to, poc.c);
	}
	else if (poc.c_p_b == 1) {

		beparent_1(_from, _to, poc.str.c_str());
	}


	beparent(last_state, _from);
	last_state = _to;
	return _to;


}

void analysis_symbol(std::vector<pack_or_char_or_bracket>& container,int pos,char c) {
#ifdef debugmsg
	cout << "as:" << container[pos - 1].str << endl;
	cout << "as1:" << container[pos - 1].c << endl;
#endif
	switch (c)
	{
	case '*':
		if (container[pos - 1].c_p_b == 2) {
			cout << container[pos - 1].str << endl;
			state* beg = container[pos - 1].begin_of_bracket;
			state* end1 = new state();

			beparent(last_state, end1);
			for (auto&r : beg->parent) {
				beparent(r, end1);
			}
			//beg is null?
			beparent(last_state,beg);// why  can not  find ?8ik9o
			last_state = end1;
		}
		else if (container[pos - 1].c_p_b == 0 || container[pos - 1].c_p_b == 1) {
#if 0
			assert(!last_state->parent.empty() && !last_state->parent[0]->parent.empty());
			state* beg = last_state->parent[0]->parent[0];
			assert(beg->state_num == 0 || !beg->parent.empty());
			state* end = new state();
			beparent(beg, end);
			beparent(last_state, last_state->parent[0]);
			beparent(last_state, end);
			last_state = end;
#endif
			assert(!last_state->parent.empty());
			state* end = new state();
			beparent(last_state, end);
			for (auto& beg : last_state->parent) {
				beparent(beg, end);
				beparent(last_state,beg);
			}
			
           last_state = end;
		}//因为nfa的特殊性before->parent一定只有一个分支 ab s1->s2->s3->4 a->b必须通过一个空转移
		break;
	case '?':
		if (container[pos - 1].c_p_b == 2) {
			state* beg = container[pos - 1].begin_of_bracket;
			state* end1 = new state();
			beparent(last_state, end1);
			for (auto&r : beg->parent) {
				beparent(r, end1);
			}
			last_state = end1;
		}
		else if (container[pos - 1].c_p_b == 0 || container[pos - 1].c_p_b == 1) {
#if 0
			assert(!last_state->parent.empty());
			for(auto& beg:last_state->parent)
			state* beg = last_state->parent[0]->parent[0];
			assert(beg->state_num == 0 || !beg->parent.empty());
			state* end = new state();
			beparent(beg, end);
			beparent(last_state, end);
			last_state = end;
#endif
			
			state* end = new state();
			beparent(last_state, end);
			for (auto& beg : last_state->parent)
			{
				
				beparent(beg, end);
			}
			last_state = end;
		}
		break;
	case '+':
		if(container[pos-1].c_p_b==2){
			
			state* beg = container[pos - 1].begin_of_bracket;
			state* end = new state();
			beparent(last_state, beg);
			
			beparent(last_state, end);
			last_state = end;
		}
		else if (container[pos - 1].c_p_b == 0 || container[pos - 1].c_p_b == 1) {
#if 0
			//test str2
			state* test = &s0;
			cout << test->state_num << endl;
			cout << test->edges[0].transto->state_num << endl;
			cout << test->edges[0].transto->edges[0].transto->state_num << endl;
			//test end;
#endif		//test ok, s0->s1->s2->s3 by edges,test parent?	

			
			assert(!last_state->parent.empty());
			
			state* end = new state();

			beparent(last_state, end);
			for (auto&beg : last_state->parent) {
				beparent(last_state, beg);
			}
			last_state = end;
		}
	
		break;
	default:
		break;
	}
}

static void REtoNFA(REstr& re) {
#ifdef debugmsg
	cout << "re:" << re << endl;
#endif
	int left_bracket = 0;//检测字节流的时候里面字节前面有几个括号未闭合 (a(b)c) 走到b=2,走到c=1;
	bool left_comp = false;
	stack restack(re);

#if 0

	//test push&&pop
	stack teststack1(re);
	stack test;

	while (!teststack1.stackzone.empty()) {
		test.push(teststack1.pop());//插入的是倒序的，后面需要注意
	}
	cout << "test push&&pop" << endl;
	std::reverse(test.stackzone.begin(), test.stackzone.end());
	for (auto&r : test.stackzone) {
		cout << r.c;
	}
	cout << endl;
	cout << test.cursor << " " << teststack1.cursor << endl;
	cout << re.size() << endl;
	cout << "test end" << endl;
#endif // DEBUG  this means push and pop are right
	
	int cursor = 0; //字符串流中的当前位置
	std::vector<REstr>branchs;
	int left_brach = 0;
	auto strbeg = re.begin();
	bool escape_c_before = false;
	for (auto r = re.begin(); r != re.end(); r++) {
		//要实现转义字符 \( \) \[ \]
		if (re[cursor] == '\\') {
			escape_c_before = true;
			continue;
		}
		if (*r == '('&&!escape_c_before)
			left_bracket++;
		if (*r == ')' && !escape_c_before)
			left_bracket--;
		if (*r == '|'&&left_bracket == 0 && !escape_c_before)
		{
			branchs.push_back(string(strbeg, r));
			strbeg = r + 1;
			assert(strbeg != re.end());
			left_brach++;
		}
		escape_c_before = false;
	}
	escape_c_before = false;
	//这里的转义字符识别有问题
	if (left_brach > 0) {
		branchs.push_back(string(strbeg, re.end()));
	}
#ifdef debugmsg
	if (left_brach > 0) {
		cout << "have branchs" << endl;
		for (auto&r : branchs)
			std::cout << r << std::endl;
		
	}
#endif // DEBUG
	left_bracket = 0;
	cout << endl;
	if (!branchs.empty()) {
		state* temp_last = nullptr;
		state* beg = new state();
		beparent(last_state, beg);
		
		state* end = new state();
		last_state = beg;
		temp_last = last_state;	
		for (auto&re : branchs) {
			left_bracket = 0;
			last_state = temp_last;
			stack realstack;
			auto re_iter = re.begin();
			cursor = 0;
			
			for (; cursor != re.size(); cursor++) {
				if (re[cursor] == '\\') {
					escape_c_before = true;
					continue;
				}
				realstack.push(pack__or_char(re[cursor], escape_c_before));
				
				//(,),|,*,?,+,other
				if (re[cursor] == '(' && !escape_c_before) {
					left_bracket++;
					
				}
				if (re[cursor] == ')' && !escape_c_before) {
					left_bracket--;
					analysis_bracket(realstack, cursor,left_bracket);
					
					//遇到了)会将left_bracket-1
				}
				if (re[cursor] == '[' && !escape_c_before) {
					left_comp = true;
					
				}
				if (re[cursor] == ']' && !escape_c_before) {
					assert(left_comp == true);
					analysis_comp(realstack, left_bracket);
					left_comp == false;
					
				}
				if (re[cursor] == '*' && !escape_c_before) {
					if (left_bracket == 0)
					{
						realstack.pop();
						analysis_symbol(realstack.stackzone, realstack.cursor, re[cursor]);
					}
				
				}
				if (re[cursor] == '+' && !escape_c_before) {
					if (left_bracket == 0)
					{
						realstack.pop();
						analysis_symbol(realstack.stackzone, realstack.cursor, re[cursor]);

					}
					
				}
				if (re[cursor] == '?' && !escape_c_before) {
					if (left_bracket == 0)
					{
						realstack.pop();
						analysis_symbol(realstack.stackzone, realstack.cursor, re[cursor]);

					}
				
				}

				if (re[cursor] == '$' || re[cursor] == '_' || 47 < re[cursor]&& re[cursor] < 58 ||
					64 < re[cursor] && re[cursor] < 91 || 96 < re[cursor] && re[cursor] < 123
					|| (re[cursor] == '(' && escape_c_before) || (re[cursor] == ')' && escape_c_before)
					|| (re[cursor] == '[' && escape_c_before) || (re[cursor] == ']' && escape_c_before)
					|| (re[cursor] == '+' && escape_c_before) || (re[cursor] == '?' && escape_c_before)
					|| (re[cursor] == '*' && escape_c_before) || (re[cursor] == '|' && escape_c_before)
					|| (re[cursor] == '-' ) || (re[cursor] == '/') 
					|| (re[cursor] == '>') || (re[cursor] == '<' )
					|| (re[cursor] == '=' ) || (re[cursor] == ':')
					|| (re[cursor] == ',') || (re[cursor] == '!')
					|| (re[cursor] == '~' ) || (re[cursor] == '&' )
					|| (re[cursor] == '%' ) || (re[cursor] == '^' )){
					if (left_bracket == 0) {
#if 0
						state* blank = new state();
						blank->state_num = next_state_num++;
						state* _from = new state();
						_from->state_num = next_state_num++;
						state* _to = new state();
						_to->state_num = next_state_num++;
						edgeto temp(_to, re[cursor]);
						_from->edges.push_back(temp);
						blank->edges.push_back(edgeto(_from));
						last_state->edges.push_back(blank);
						last_state = _to;
#endif;
						analysis_when__left_bracket_0(pack__or_char(re[cursor]), 1);
					}
					
					//如果有括号，需要将整个完整的括号进行计算
				}
				escape_c_before = false;
			}
			realstack.stackzone.clear();
			beparent(last_state, end);
			
		}
	}
	else {
		
		stack realstack;
		//re.size有问题，re是字符串，并不会受影响导致vector访问溢出，明天改
#if 0
		for (; cursor != re.size(); cursor++) {
			realstack.push(pack__or_char(re[cursor]));
			//(,),|,*,?,+,other

			if (re[cursor] == '(') {
				left_bracket++;
				continue;
			}
			if (re[cursor] == ')') {
				left_bracket--;
				analysis_bracket(realstack, cursor,left_bracket);
				//cursor应该重新设定
				
				continue;
				//遇到了)会将left_bracket-1
			}

			if (re[cursor] == '[') {
				left_comp = true;
				continue;
			}
			if (re[cursor] == ']') {
				assert(left_comp == true);
				cout << "S:"<<realstack.stackzone.size() << endl;
				analysis_comp(realstack, left_bracket);
				cout << "c:" << cursor << endl;
				cout << "S:" << realstack.stackzone.size() << endl;
				left_comp == false;
				continue;

			}

			if (re[cursor] == '*') {
				if (left_bracket == 0)
				{
					realstack.pop();
					analysis_symbol(realstack.stackzone, realstack.cursor, re[cursor]);
				}
					continue;
			}
			if (re[cursor] == '+') {
				if (left_bracket == 0)
				{	realstack.pop();
				    analysis_symbol(realstack.stackzone, realstack.cursor, re[cursor]);
				}
					continue;
			}
			if (re[cursor] == '?') {
				if (left_bracket == 0)
				{
					realstack.pop();
					analysis_symbol(realstack.stackzone, realstack.cursor, re[cursor]);
				}
					continue;
			}
			if (re[cursor] == '$' || re[cursor] == '_' || 47 < re[cursor]&& re[cursor] < 58 ||
				64 < re[cursor] && re[cursor] < 91 || 96 < re[cursor] && re[cursor] < 123) {
				if (left_bracket == 0&&left_comp!=true) {
#if 0
					state* blank = new state();
					blank->state_num = next_state_num++;
					state* _from = new state();
					_from->state_num = next_state_num++;
					state* _to = new state();
					_to->state_num = next_state_num++;
					edgeto temp(_to, re[cursor]);
					_from->edges.push_back(temp);
					blank->edges.push_back(edgeto(_from));
					last_state->edges.push_back(blank);
					last_state = _to;
#endif;
					analysis_when__left_bracket_0(pack__or_char(re[cursor]), 1);
			}
				continue;
				//如果有括号，需要将整个完整的括号进行计算
		}

	}
#endif
		for (; cursor != re.size(); cursor++) {
			if (re[cursor] == '\\') {
				escape_c_before = true;
				continue;
			}
			realstack.push(pack__or_char(re[cursor], escape_c_before));

			//(,),|,*,?,+,other
			if (re[cursor] == '(' && !escape_c_before) {
				left_bracket++;
				
			}
			if (re[cursor] == ')' && !escape_c_before) {
				left_bracket--;
				analysis_bracket(realstack, cursor, left_bracket);
				
				//遇到了)会将left_bracket-1
			}
			if (re[cursor] == '[' && !escape_c_before) {
				left_comp = true;
				
			}
			if (re[cursor] == ']' && !escape_c_before) {
				assert(left_comp == true);
				analysis_comp(realstack, left_bracket);
				left_comp == false;
				
			}
			if (re[cursor] == '*' && !escape_c_before) {
				if (left_bracket == 0)
				{
					realstack.pop();
					analysis_symbol(realstack.stackzone, realstack.cursor, re[cursor]);
				}
				
			}
			if (re[cursor] == '+' && !escape_c_before) {
				if (left_bracket == 0)
				{
					realstack.pop();
					analysis_symbol(realstack.stackzone, realstack.cursor, re[cursor]);

				}
				
			}
			if (re[cursor] == '?' && !escape_c_before) {
				if (left_bracket == 0)
				{
					realstack.pop();
					analysis_symbol(realstack.stackzone, realstack.cursor, re[cursor]);

				}
			
			}

			if (re[cursor] == '$' || re[cursor] == '_' || 47 < re[cursor] && re[cursor] < 58 ||
				64 < re[cursor] && re[cursor] < 91 || 96 < re[cursor] && re[cursor] < 123 
				|| (re[cursor] == '(' && escape_c_before) || (re[cursor] == ')' && escape_c_before)
				|| (re[cursor] == '[' && escape_c_before) || (re[cursor] == ']' && escape_c_before)
				|| (re[cursor] == '+' && escape_c_before) || (re[cursor] == '?' && escape_c_before)
				|| (re[cursor] == '*' && escape_c_before) || (re[cursor] == '|' && escape_c_before)
				|| (re[cursor] == '-') || (re[cursor] == '/')
				|| (re[cursor] == '>') || (re[cursor] == '<')
				|| (re[cursor] == '=') || (re[cursor] == ':')
				|| (re[cursor] == ',') || (re[cursor] == '!')
				|| (re[cursor] == '~') || (re[cursor] == '&')
				|| (re[cursor] == '%') || (re[cursor] == '^'))
			{
				if (left_bracket == 0) {
#if 0
					state* blank = new state();
					blank->state_num = next_state_num++;
					state* _from = new state();
					_from->state_num = next_state_num++;
					state* _to = new state();
					_to->state_num = next_state_num++;
					edgeto temp(_to, re[cursor]);
					_from->edges.push_back(temp);
					blank->edges.push_back(edgeto(_from));
					last_state->edges.push_back(blank);
					last_state = _to;
#endif;
					analysis_when__left_bracket_0(pack__or_char(re[cursor]), 1);
				}
				
				//如果有括号，需要将整个完整的括号进行计算
			}
			escape_c_before = false;
		}
#ifdef DEBUG
		for (auto&r : restack.stackzone) {
			switch (r.c_p_b)
			{
			case 0:
				std::cout << r.c;
				break;
			case 1:
				std::cout << r.str;
				break;
			default:
				break;
			}
		}
		cout << endl;
#endif // DEBUG
}

}

void set_end_state(state* s) {
	if (s->set_end_t_b == true)
		return;
	s->set_end_t_b = true;
	if (s->edges.empty())
	{
		s->accept_state = true;
		return;
	}
	for (auto&r : s->edges) {
		set_end_state(r.transto);
	}
}

#ifdef debugmsg
state* recursion_find(state* s, char* x, int size1, int* back) {
	if (s->edges.empty())
		return nullptr;
	std::deque<state*> queue;
	queue.push_back(s);
	state* temp = nullptr;
	char cmp[4] = { 0,0,0,0 };
	int i = 0;
	bool pop = true;
	state* re1 = nullptr;
	while (true&&i<size1) {
		
		if (!queue.empty()||!pop) {
			if (pop) {
				temp = queue[0];
				queue.pop_front();
			}
			else {
				queue.clear();
				pop = true;
			}
			for (auto&r : temp->edges) {
					if (!strncmp(r.transc, &x[i], 1)) {
						temp = r.transto;
						re1 = temp;
						pop = false;
						i++;
						if (i == size1 - 1)
						{
							*back = i;
							return temp;
						}
						break;
					}
					if (!strncmp(r.transc, cmp, 4)) {
						queue.push_back(r.transto);
					}
				}
			}
		else {
				break;
			}
		
		
	}
	*back = i;
	return re1;
}
#endif

void achieve_nfa_op(state& s_start,state* &dfa_s0) {
	
	for (int i = 0; i != 20; i++) {
		last_state = &s_start;
		REtoNFA(string(newoperatorsymbol[i]));
	}
	printf_nfa_table_op(s_start, next_state_num);
	next_state_num = 0;
	dfa_s0 = new state();
	traversal_set_0(&s_start);
	set_end_state(&s_start);
	RFA2DFA(next_state_num, &s_start,false,dfa_s0);
}
void achieve_nfa_sy(state& s_start, state* &dfa_s0) {
#if 0
	for (int i = 0; i != 47; i++) {
		cout << string(operatorsymbol[i]) <<"start: " <<endl;
		last_state = &s0;
		int size1 = strnlen(operatorsymbol[i], 5);
		int pos = 0;
		state* temp = &s0;
		state* temp1 = nullptr;
		temp1 = recursion_find(temp, operatorsymbol[i],size1,&pos);	
		//开场的时候进行一次检测，如果能查到相同的转移，在那个转移之后构造
		if (temp1 != nullptr)
			last_state = temp1;
		REtoNFA(string(operatorsymbol[i]+pos));
	}

#endif

	
	last_state = &s_start;
	REtoNFA(identifier_str);
	printf_nfa_table(s_start, next_state_num);

	next_state_num = 0;
	dfa_s0 = new state();
	traversal_set_0(&s_start);
	set_end_state(&s_start);
	RFA2DFA(next_state_num, &s_start,true,dfa_s0);
#if 0
	release_nfa_memory(s0);
#endif
}

/*
分配动态内存，运用state指向state的方式表示nfa,state中储存edgeto
用来表示每条边
*/