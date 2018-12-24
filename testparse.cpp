#include"stdafx.h"
#include"testparse.h"

#include<iostream>
using std::initializer_list;
using std::cout;
using std::endl;
using std::cin;

void first_search_result_store::search(token* t) { now_search = t; search_result.clear(); }

void first_search_result_store::push(token* t) { 
	search_result.push_back(t);
	FSET.push_back(t->self);
}

std::list<token*>&& first_search_result_store::pop_all(){
	return std::move(search_result);
}

std::ostream& operator<<(std::ostream& os, token& t) {
	cout << t.self << "->:" << endl;
	for (auto r = t.all_deduce.begin(); r != t.all_deduce.end(); r++) {
		if (!r->empty()) {
			
			for (auto q = r->begin(); q != r->end();q++){
				cout << (*q)->self;
			}
			cout << endl;
		}
		else {
			continue;
		}
	}
	return os;
}

std::list<token*>::iterator operator +(std::list<token*>::iterator l, int off) {
	for (int i = 0; i <= off; i++) {
		l++;
	}
	return l;
}




token* Goal = nullptr, *List = nullptr, *Pair = nullptr, *L_b = nullptr, *R_b = nullptr;
token* Epsilon = nullptr;
//不应该return 而应该放入一个集合里，应该是当前FIRST(a)的返回集合
first_search_result_store resultr_stack;
std::list<FIRST_SET> FSet;

void do_first_search(token* t) {
	if (t->visit_b == false) {
		t->visit_b = true;
		cout << "token: " << t->self << endl;
		if (!t->all_deduce.empty()) {
			for (auto r = t->all_deduce.begin(); r != t->all_deduce.end(); r++)
				if (!r->empty()) {
					if ((*r->begin())->S_T == true) {
						// TODO: return T;
						resultr_stack.push(*r->begin());

					}
					else {
						do_first_search(*r->begin());

					}
				}
				else {
					resultr_stack.push(Epsilon);
				}
		}
		else if (t->S_T == true) {
			resultr_stack.push(t);
		}
		else {

			// TODO:当 all_deduce为空，缺不为T时，出现了问题，处理
		}

		// todo: return T  按照推算结果，应该不需要 在这里返回，因为T必定
		//在其parent S里被推导出来
	}
	else {
		return;
	}
}
void First(initializer_list<token*>& expr){
	auto beg = expr.begin();
	while (beg != expr.end()) {
		resultr_stack.search(*beg);
		do_first_search(*beg);
		FIRST_SET temp(*beg,resultr_stack.FSET);
		FSet.push_back(temp);
		beg++;
	}
	//当list的当前为空，则指向下一个token*导出FIRST，知道队尾为空
}

/////////////////////////////////////////////////////////////////////////
void init_table() {
	token* goal=new token("Goal",false);
	token* list=new token("List",false);
	token* pair=new token("Pair",false);
	token* l_b=new token("(",true);
	token* r_b=new token(")",true);
	token* epsilon = new token("ε",true);

	Goal = goal;
	List = list;
	Pair = pair;
	L_b = l_b;
	R_b = r_b;
	Epsilon = epsilon;

	std::list<token*> g_d(1,list);
	Goal->all_deduce.push_back(g_d);
	
	std::list<token*> l_d1(1, pair);
	std::list<token*> l_d2;
	l_d2.push_back(list); l_d2.push_back(pair);
	List->all_deduce.push_back(l_d1); List->all_deduce.push_back(l_d2);

	std::list<token*> p_d1;
	std::list<token*> p_d2;
	p_d1.push_back(l_b); p_d1.push_back(pair); p_d1.push_back(r_b);
	p_d2.push_back(l_b); p_d2.push_back(r_b);
	pair->all_deduce.push_back(p_d1);
	pair->all_deduce.push_back(p_d2);
	

	//实现first集合&&前瞻号与标记点


	
}
void test_all_regular(initializer_list<token*>& expr) {
	for (auto r = expr.begin(); r != expr.end(); r++) {
		cout << **r;
	}
}
void test_FIRST() {
	init_table();
	initializer_list<token*> temp{ Goal,List,Pair };
	test_all_regular(temp);// rigth
	First(temp);
	for (auto r = FSet.begin(); r != FSet.end(); r++) {
		cout << "S: " << (*r->S).self << "  FIRST: ";
		for (auto q = r->SET.begin(); q != r->SET.end(); q++) {
			cout << *q << " ";
		}
		cout << endl;
	}
}

//////////////////////////////////////////////////////////////////////////////////
void closure(token* ga) {
	int nbeg = 0;
	for (auto iter1 = ga->all_deduce.begin(); iter1 != ga->all_deduce.end(); iter1++,nbeg++) {
		for (auto iter2 = (*iter1->begin() + ga->pos[nbeg])->all_deduce.begin(); iter2 != (*iter1->begin() + ga->pos[nbeg])->all_deduce.end(); iter2++) {
			initializer_list<token*> expr={}
		}
	}
}






