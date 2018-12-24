#include"stdafx.h"
#include"RE2DFA.h"
#include<iostream>
#include<algorithm>
#include<cassert>
#include"NFA2DFA.h"
using namespace std;


#define  dfa_state state

#define beparent_1(paren,son,c)\
son->parent.push_back(paren);\
paren->edges.push_back(edgeto(son,c));


#define beparent_2(paren,son,c) paren->edges.push_back(edgeto(son,c));

#define beparent(paren,son)\
 son->parent.push_back(paren);\
 paren->edges.push_back(edgeto(son));\



#define ebeg(x) x->edges.begin()

#define eend(x) x->edges.end()


#if 0
dfa_state dfa_s0;  // this's state_num is 0 
#endif
extern int next_state_num;
char cmp[3] = { 0,0,0 };

state* line[50];
int visit_times[500];
int visit_once[500];
state_to realindex[500];
std::vector<_son_of_state_to_with_pointer> branch_arrary;
state* dfa2nfa_index[500]; //用来记录下一次traverasl_rfa的调用传入哪个state;
vector<dfa_state*> accept_states;
vector<dfa_state*> not_accept_states;
int skip_times[500];

std::list<state_group*> group_queue;

typedef vector<dfa_state*> dfa_vector;
typedef std::vector<state*> merge_states;

dfa_state* cpb_dfa = nullptr;
dfa_state* now_cpb_dfa = nullptr;
bool return_branch = false;
bool route_return = false;

state_call_stack head;
state_call_stack* now_pointer = &head;
//true means return all call_back function

void least_DFA(dfa_state*);

void state_group::erease(state* s){
	for (auto&r = group.begin(); r != group.end(); r++) {
		if (*r == s) {
			group.erase(r);
			return;
		}
	}
}




void first_init_visit_times(state* s0) {

}
bool init_state_visit_times(state* s) {
#if 0
	if (s->parent.size() == 0)
		s->can_be_visit_times = 1;
	else {
		s->can_be_visit_times = s->parent.size();
	}
#endif
	if (s->edges.empty())
	{
		memset(visit_once, 2000, 0);
		return false;
	}
	if (visit_once[s->state_num] >= 1)
	{
		memset(visit_once, 2000, 0);
		return true;
	}
	visit_once[s->state_num]++;
	
	bool return_value = init_state_visit_times(s->edges[s->self_edge_cursor].transto);
	if (s->self_edge_cursor >= s->edges.size() - 1)
		s->self_edge_cursor = 0;
	else {
		s->self_edge_cursor++;
	}
	if (return_value == true)
			return true;
	
	//不应该用递归实现，应该用迭代实现
	//?可能还是递归比较好，思路是递归遍历每个节点，每遍历一次就将visit_times+1
}
//现在的想法是设定一个visit_time,控制访问次数
void recursion(dfa_state* s1, int pos)
{

	//每个分支要互连
	if (realindex[pos].traversal_before > 1) {
		std::cout << "circle route" << endl;
		return;
	}
	
#if 0
	cout << "sdt" << pos << endl;
	cout << "Sq" << realindex[pos].cursor.size() << endl;
#endif
	for (int i=0;i!=realindex[pos].cursor.size();i++)
	{
		if (strcmp(realindex[pos].cursor[i].str, cmp))
		{

			dfa_state* temp = new dfa_state();
			std::cout << "s1: " << s1->state_num << "temp:" << temp->state_num << "c: " << realindex[pos].cursor[i].str << endl;
			beparent_1(s1, temp, realindex[pos].cursor[i].str);
			continue;
		}
		else {
			std::cout << "s1: " << s1->state_num <<"pos:"<< realindex[pos].cursor[i].pos << endl;
			recursion(s1, realindex[pos].cursor[i].pos);
		}
	}

}
// branch_re的visit_times 有问题
// 还有branch_re会将路径访问过一次，所以recursion应该利用branch_re访问的数据
//当找到环路的时候调用

void recursion1(dfa_state* s1, state* s2) {
	if (visit_times[s2->state_num] >= 1)
		return;
	
	if (s2->edges.empty())
		return;
	for (auto&r : s2->edges) {
		if (strcmp(r.transc, cmp)) {
			for (auto&q : s1->parent) {
				dfa_state* temp = new dfa_state();
				beparent_1(q, temp, r.transc);
				
			}
		}
	}
}


void branch_recursion(dfa_state*s1,state* s0){
	
	if (s0->edges.empty())
	{
		now_pointer->state_n.push_back(s0->state_num);
		s1->accept_state = true;
		return;
	}
	if (visit_times[s0->state_num] >= 1)
	{
		if (s0->edges.size()>=1) {
			
			for (auto&r : cpb_dfa->parent[0]->edges)
			{
				
				beparent_1(cpb_dfa, r.transto,r.transc);
				
			}
		}
		return_branch = true;
		route_return = true;
		return;
		//找到重复的一点，确认是不是上个分叉点，是的话就将dfa的每个子节点连接起来
	}
	// 当遇到环路的时候，需要进行连接
	//因为这个，导致第二个括号不会再被访问
	//考虑(a|b|c)*cd的情况，不能因为出现了环路就停止递归
	now_pointer->state_n.push_back(s0->state_num);
	visit_times[s0->state_num]++;
	
	for (auto& r : s0->edges) {
		if (strcmp(r.transc, cmp)) {
			branch_arrary.push_back(_son_of_state_to_with_pointer(r.transc,r.transto));
			realindex[s0->state_num].push(_son_of_state_to(r.transto->state_num,r.transc));
			continue;
		}
		else {
			realindex[s0->state_num].push(_son_of_state_to(r.transto->state_num));
			branch_recursion(s1,r.transto);
		}
	}
	
}

void ctrl_branch_arrary(dfa_state* s0,state* s1) {
	//现在已经消除了环路情况下子节点互联的问题，需要解决不能重复访问的问题
	assert(branch_arrary.empty());
	cpb_dfa = s0;
	branch_recursion(s0,s1);
	for (auto& r : branch_arrary)
	{
			dfa_state* temp = new dfa_state();
			beparent_1(s0, temp, r.str);
			dfa2nfa_index[temp->state_num] = r.company_nfa;
	}
	cpb_dfa = nullptr;
	branch_arrary.clear();
}

void traversal_RFA(dfa_state* s1,state* s2){
	
#if 0
	if (s2->traversal_before)/*接下来需要考虑的是，重新空转绕回某个点以后怎么办*/
	{
		cout << "route!!!!" << endl;
		recursion(s1, s2->state_num); //推测会多次调用
		return;
	}
#endif
	if (s2->accept_state)
		s1->accept_state = true;
	s2->traversal_before = true;
	if (s2->edges.empty())
		return;
	
	for (auto&r : s2->edges)
	{
#if 0
		if (strcmp(r.transc,cmp)) {
			dfa_state *temp = new dfa_state();
			beparent_1(s1, temp, r.transc);
#if 0			
index[r.transto->state_num] = temp; 
#endif	        
//会将$20放在里面，但是查找的时候只能找到index 16==nullptr，所以需要改进
      cout <<"_s1_from: "<<s1->state_num<< "t_temp:" << temp->state_num << "sto:" << r.transto->state_num <<"  "<<r.transc<< endl;
	  realindex[s2->state_num].push(_son_of_state_to(r.transto->state_num,r.transc));
	  traversal_RFA(temp, r.transto);
		}
		else {
				cout << "t_s1:" << s1->state_num << "sto:" << r.transto->state_num << endl;
				realindex[s2->state_num].push(_son_of_state_to(r.transto->state_num));
				traversal_RFA(s1,r.transto);
		}
#endif
		edgeto& temp = r;
		state* temp_state = temp.transto;
		while (true) {
			
			if (strcmp(temp.transc, cmp)) 
			{
				dfa_state* tmp = new dfa_state();
				beparent_1(s1, tmp, r.transc);
				realindex[s2->state_num].push(_son_of_state_to(temp.transto->state_num, temp.transc));
				dfa2nfa_index[tmp->state_num] = temp_state;
				break;
			}
			else {
				if(temp_state->edges.size()>1)
				{
					ctrl_branch_arrary(s1,temp_state);
					
					break;
				}
				if(temp_state->edges.size()==1){
					temp = temp_state->edges[0];
					temp_state = temp_state->edges[0].transto;
					if(!temp_state->edges.empty()){
						realindex[s2->state_num].push(_son_of_state_to(temp_state->edges[0].transto->state_num));
					}
					continue;
				}
				if (temp_state->edges.size() == 0) {
					s1->accept_state = true;
					break;
				}
			}
		}

	}
	state_call_stack* this_call_stack = nullptr;
	if (route_return == true) {
		//考虑子节点全是环路的时候，是否需要改动state_call_stack visit_time
		bool con2 = false;
		for (auto&r : s1->edges) {
			for (auto&p : s1->parent)
			{
				for (auto&q : p->edges)
				{
					if (r.transto->state_num == q.transto->state_num)
					{
						con2 = true;
						break;
					}
				}
				if (con2 == true)
					break;
				continue;
			}
			if (con2 == true)
				continue;
			else {
				now_pointer->next = new state_call_stack;
				this_call_stack = now_pointer;
				now_pointer = now_pointer->next;
				traversal_RFA(r.transto, dfa2nfa_index[r.transto->state_num]);
				//jo jo 's Algorithm
				if (!this_call_stack->next->state_n.empty()) {
					for (auto&r : this_call_stack->next->state_n)
						visit_times[r] = 0;
				}

				delete this_call_stack->next;
				now_pointer = this_call_stack;
			}
		}
	}
	else {
		
		for (auto&r : s1->edges) {
			now_pointer->next = new state_call_stack;
			this_call_stack = now_pointer;
			now_pointer = now_pointer->next;
			traversal_RFA(r.transto, dfa2nfa_index[r.transto->state_num]);

			if (!this_call_stack->next->state_n.empty()) {
				for (auto&r : this_call_stack->next->state_n)
					visit_times[r] = 0;
			}

			delete this_call_stack->next;
			now_pointer = this_call_stack;
		}
	}
	
	route_return = false;
	
	return;
	
}

void divided_accept(dfa_state* sq) {
	
	if (sq->test_accept_t_b == true)
		return;
	
	sq->self_edge_cursor = 0;
	sq->test_accept_t_b = true;
	
	if (sq->accept_state == true)
		accept_states.push_back(sq);
	else 
	{
		not_accept_states.push_back(sq);
	}

	if (sq->edges.empty())
		return;
	for (auto&r : sq->edges) {
		divided_accept(r.transto);
	}
}

void RFA2DFA(int next_num, state* s0,bool if_sy,state* &dfa_s0) {

	memset(realindex, 0, 2000);
	memset(visit_times, 0, 2000);
	memset(dfa2nfa_index, 0, 2000);
	
	traversal_RFA(dfa_s0, s0);
	
	
	if (if_sy) {
		printf_nfa_table(*dfa_s0, next_state_num);
		divided_accept(dfa_s0);

		least_DFA(dfa_s0);
	}//关于opera的least之前的都对了
	else {
		printf_nfa_table_op(*dfa_s0, next_state_num);
	}
}

/*最小化DFA*/

#if 0
void deal_not_accept_state(dfa_vector& not_a_s) {
	
}

void deal_accept_state(dfa_vector& a_s) {

}
#endif;

vector<edgeto>::iterator find_e(vector<edgeto>::iterator&beg, vector<edgeto>::iterator&end,state* value){
	for (auto r = beg; r != end; r++)
		if (r->transto == value)
			return r;
	return end;
}

vector<edgeto>::iterator find_e(state* q,vector<edgeto>::iterator&beg, vector<edgeto>::iterator&end, state* value,int skip_time) {
	int count = 0;
	for (auto&r = beg; r != end; r++) {
	
		bool before = false;
		if (r->transto == value)
		{
			for (auto&d : q->find_e_times)
			{
				
				if (d == count)
				{
					before = true;
					break;
				}
			}
			if (before == false) {
				q->find_e_times.push_back(count);
				return r;
			}
		}
		count++;
	}
	return end;
	
}
void find_parent_not_self_and_son(std::list<state_group*>& sg) {
	for (auto&r : sg) {
		for (auto&q : r->group) {
			for (auto&a1:q->parent) {
				if (a1 == q) {
					q->p_in_edges.push_back(true);
					continue;
				}
				bool in = false;
				for (auto&a2 : q->edges) {
					if (a1 == a2.transto)
					{
						q->p_in_edges.push_back(true);
						in = true;
						break;
					}
				}
				if (!in) {
					q->p_in_edges.push_back(false);
				}
			}
		}
	}
}
void deal_with_every_group() {
	//需要先从acceopt_state开始分裂 
	vector<state_group*> new_group;
	std::list<state_group*>::iterator temp = group_queue.end();
	temp--;
	int count_times = 0;
	bool change = false;
	bool queue_change = false;
	std::deque<state*> next_exec;
	//问题是，第一次找到为最根部的点，而他的父亲，都不在一个和他本身，都不在一个集合
	//里，所以需要解决下次nexc_exec集合为空的问题
#if 0
	while (true)
	{
		next_exec.clear();
		cout << "fuck" << endl;
		vector<state*> needed_erease;
		vector<state*>temp_exec;
		auto&r = (*temp)->group.begin();

#if 0
		if (temp == (--group_queue.end())) {
			for (auto&c : (*temp)->group)
			{
				if (c->edges.empty() && c->accept_state) {
					for (auto&e : c->parent)
						next_exec.push_back(e);
					continue;
				}
				if (c->accept_state) {
					bool all_self = true;
					for (auto&q : c->edges) {
						if (q.transto != c)
							all_self = false;
					}
					if (all_self) {
						for (auto&e : c->parent)
							next_exec.push_back(e);
						continue;
					}
					else continue;
				}

			}
		}
#endif

		if (temp == (--group_queue.end())) {
			for (auto&c : (*temp)->group)
			{
				if (c->edges.empty() && c->accept_state) {
					next_exec.push_back(c);
					continue;
				}
				if (c->accept_state) {
					bool all_self = true;
					for (auto&q : c->edges) {
						if (q.transto != c)
							all_self = false;
					}
					if (all_self) {

						next_exec.push_back(c);
						continue;
					}
					else continue;
				}

			}
			cout << "next_exec: " << next_exec.size() << endl;
		}



		while (!next_exec.empty())// need change ,从链接accept_state的group一步一步查找回来，倒着
		{
			cout << "newwwwwwwwwwwwwww" << endl;
			for (auto&em : next_exec)
				cout << em->state_num << "  ";
			cout << endl;
			if (!next_exec.empty()) {
				bool not_f = true;
				for (auto a = (*temp)->group.begin(); a != (*temp)->group.end(); a++)
					if ((*a)->state_num == next_exec[0]->state_num)
					{
						r = a;
						not_f = false;
						next_exec.pop_front();
						break;
					}
				if (not_f)
					break;
			}

			for (auto&q : (*r)->edges) {
				//当r链接的state不指向r属于的group的state ，那么r该改变，而不是q
				bool same_all = true;
				for (auto& op : (*temp)->connectgroup) {
					if (q.transto->belong2g == op)
						continue;
					else {
						same_all = false;
						break;
					}
				}
				if (same_all)
				{
					for (int i = 0; i != (*r)->p_in_edges.size(); i++) {
						if (!(*r)->p_in_edges[i]) {
							next_exec.push_back((*r)->parent[i]);
						}
					}
					continue;
				}
				//还是应该利用connectgroup，将指向不同grop的state分裂除去
				else {
					cout << "fuck1" << endl;
					bool return_ctrl = false;
					for (auto&e : new_group)//如果能找到相同的，则不用创建新的 state_group
					{//合并算法有问题
						bool same1 = true;

						for (auto&op : e->connectgroup)
						{
							bool same2 = false;
							for (auto&cp : (*r)->edges)
							{
								if (cp.transto->belong2g == op)
									same2 = true;
								else continue;
							}
							if (same2)
								continue;
							else {
								same1 = false;
								break;
							}
						}
						if (same1)
						{//当e的connect和r的connect一样时，再把r塞入e的集合
							(*r)->belong2g = e;
							e->group.push_back(*r);
							needed_erease.push_back(*r);
							return_ctrl = true;
							for (int i = 0; i != (*r)->p_in_edges.size(); i++) {
								if (!(*r)->p_in_edges[i]) {
									next_exec.push_back((*r)->parent[i]);
								}
							}
							goto exec_when_find;
						}//误认为state 01234在一个集合里了
					}
					//在新集合里找不到
					for (auto&e : group_queue) {
						bool same1 = true;
						for (auto&op : e->connectgroup)
						{
							bool same2 = false;
							for (auto&cp : (*r)->edges)
							{
								if (cp.transto->belong2g == op)
									same2 = true;
								else continue;
							}
							if (same2)
								continue;
							else {
								same1 = false;
								break;
							}
						}
						if (same1)
						{
							(*r)->belong2g = e;
							e->group.push_back(*r);
							needed_erease.push_back(*r);
							return_ctrl = true;
							for (int i = 0; i != (*r)->p_in_edges.size(); i++) {
								if (!(*r)->p_in_edges[i]) {
									next_exec.push_back((*r)->parent[i]);
								}
							}
							goto exec_when_find;

						}//误认为s
					}
				exec_when_find:
					if (return_ctrl)
						continue;
					state_group* new_g = new state_group(q.transc);
					for (auto&cp : (*r)->edges) {
						bool find_in = false;
						for (auto&co : new_g->connectgroup) {
							if (co == cp.transto->belong2g)
								find_in = true;
						}
						if (find_in == false)
							new_g->connectgroup.push_back(cp.transto->belong2g);
					}
					(*r)->belong2g = new_g;
					new_g->group.push_back(*r);
					new_group.push_back(new_g);
					needed_erease.push_back(*r);
					for (int i = 0; i != (*r)->p_in_edges.size(); i++) {
						if (!(*r)->p_in_edges[i]) {
							next_exec.push_back((*r)->parent[i]);
						}
					}
					continue;
				}


			}
			r++;
		}
		//找出指向的不同的accept_state

		for (auto&r : needed_erease)
		{
			cout << "exec erease" << endl;
			(*temp)->erease(r);//？？？+
		}
		for (auto&r : new_group)
		{
			cout << "fesssss" << endl;
			auto t1 = group_queue.end();
			t1--;
			group_queue.insert(t1, r); //insert before t1
			cout << "insert" << endl;
			queue_change = true;
		}
		if (temp == (--group_queue.end()) && change == false);
		temp = group_queue.begin();
		if (queue_change == false && temp != (--group_queue.end()) && change)
			break;
		queue_change = false;
		new_group.clear();
		change = true;




	}// 关于accept group的还需要再考虑，应该顺序查找过去？？？
#endif


	
	vector<group_need_er> needed_erease;

	state* r = nullptr;

#if 0
	if (temp == (--group_queue.end())) {
		for (auto&c : (*temp)->group)
		{
			if (c->edges.empty() && c->accept_state) {
				for (auto&e : c->parent)
					next_exec.push_back(e);
				continue;
			}
			if (c->accept_state) {
				bool all_self = true;
				for (auto&q : c->edges) {
					if (q.transto != c)
						all_self = false;
				}
				if (all_self) {
					for (auto&e : c->parent)
						next_exec.push_back(e);
					continue;
				}
				else continue;
			}

		}
	}
#endif
	if (temp == (--group_queue.end())) {
		for (auto&c : (*temp)->group)
		{
			if (c->edges.empty() && c->accept_state) {
				next_exec.push_back(c);
				continue;
			}
			if (c->accept_state) {
				bool all_self = true;
				for (auto&q : c->edges) {
					if (q.transto != c)
						all_self = false;
				}
				if (all_self) {

					next_exec.push_back(c);
					continue;
				}
				else continue;
			}

		}
	
	}
	while (!next_exec.empty())// need change ,从链接accept_state的group一步一步查找回来，倒着
	{
		
		
		if (!next_exec.empty()) {
			r = next_exec[0];
			next_exec.pop_front();
		}
		
		for (auto&q : r->edges) {
			//当r链接的state不指向r属于的group的state ，那么r该改变，而不是q
			bool same_all = true;
			for (auto& op : r->belong2g->connectgroup) {
				if (q.transto->belong2g == op)
					continue;
				else {
					same_all = false;
					break;
				}
			}
			if (same_all)
			{
				for (int i = 0; i != r->p_in_edges.size(); i++) {
					if (!r->p_in_edges[i]) {
						bool have = false;
						for (auto& qe : next_exec) {
							if (qe == r->parent[i])
							{
								have = true;
								break;
							}
						}
						if (!have)
							next_exec.push_back(r->parent[i]);
					}
				}
				continue;
			}
			//还是应该利用connectgroup，将指向不同grop的state分裂除去
			else {
				
				bool return_ctrl = false;
				for (auto&e : new_group)//如果能找到相同的，则不用创建新的 state_group
				{//合并算法有问题
					bool same1 = true;

					for (auto&op : e->connectgroup)
					{
						bool same2 = false;
						for (auto&cp : r->edges)
						{
							if (cp.transto->belong2g == op)
								same2 = true;
							else continue;
						}
						if (same2)
							continue;
						else {
							same1 = false;
							break;
						}
					}
					if (same1)
					{//当e的connect和r的connect一样时，再把r塞入e的集合
						group_need_er x(r->belong2g);
						x.n_e.push_back(r);
						r->belong2g = e;
						e->group.push_back(r);
						needed_erease.push_back(x);
						return_ctrl = true;
						for (int i = 0; i != r->p_in_edges.size(); i++) {
							if (!r->p_in_edges[i]) {
								bool have = false;
								for (auto& qe : next_exec) {
									if (qe == r->parent[i])
									{
										have = true;
										break;
									}
								}
								if (!have)
									next_exec.push_back(r->parent[i]);
							}
						}
						goto exec_when_find;
					}//误认为state 01234在一个集合里了
				}
				//在新集合里找不到
				for (auto&e : group_queue) {
					bool same1 = true;
					for (auto&op : e->connectgroup)
					{
						bool same2 = false;
						for (auto&cp : r->edges)
						{
							if (cp.transto->belong2g == op)
								same2 = true;
							else continue;
						}
						if (same2)
							continue;
						else {
							same1 = false;
							break;
						}
					}
					if (same1)
					{
						group_need_er x(r->belong2g);
						x.n_e.push_back(r);
						r->belong2g = e;
						e->group.push_back(r);
						needed_erease.push_back(x);
						return_ctrl = true;
						for (int i = 0; i != r->p_in_edges.size(); i++) {
							if (!r->p_in_edges[i]) {
								bool have = false;
								for (auto& qe : next_exec) {
									if (qe == r->parent[i])
									{
										have = true;
										break;
									}
								}
								if (!have)
									next_exec.push_back(r->parent[i]);
							}
						}
						goto exec_when_find;

					}//误认为s
				}
			exec_when_find:
				if (return_ctrl)
					continue;
				state_group* new_g = new state_group(q.transc);
				for (auto&cp : r->edges) {
					bool find_in = false;
					for (auto&co : new_g->connectgroup) {
						if (co == cp.transto->belong2g)
							find_in = true;
					}
					if (find_in == false)
						new_g->connectgroup.push_back(cp.transto->belong2g);
				}
				group_need_er x(r->belong2g);
				x.n_e.push_back(r);
				r->belong2g = new_g;
				new_g->group.push_back(r);
				new_group.push_back(new_g);
				needed_erease.push_back(x);
				for (int i = 0; i != r->p_in_edges.size(); i++) {
					if (!r->p_in_edges[i]) {
						bool have = false;
						for (auto& qe : next_exec) {
							if (qe == r->parent[i])
							{
								have = true;
								break;
							}
						}
						if (!have)
							next_exec.push_back(r->parent[i]);
					}
				}
				continue;
			}


		}
		if (r->edges.empty()) {
			for (int i = 0; i != r->p_in_edges.size(); i++) {
				bool have = false;
				for (auto& qe : next_exec) {
					if (qe == r->parent[i])
					{
						have = true;
						break;
					}
				}
				if(!have)
					next_exec.push_back(r->parent[i]);
				
			} //解决一下父节点重复塞入的问题！！！！！！！！！！！
		}
		r++;
	}
	//找出指向的不同的accept_state

	for (auto&r : needed_erease)
	{
#ifdef debugmsg
		cout << "exec erease" << endl;
#endif
		for (auto&eq : r.n_e)//？？？+
		{
			r.sg->erease(eq);
		}
	}
	for (auto&r : new_group)
	{
#ifdef debugmsg
		cout << "fesssss" << endl;
#endif
		auto t1 = group_queue.end();
		t1--;
		group_queue.insert(t1, r); //insert before t1
#ifdef debugmsg
		cout << "insert" << endl;
#endif
		queue_change = true;
	}
	if (temp == (--group_queue.end()) && change == false);
	temp = group_queue.begin();
	
	queue_change = false;
	new_group.clear();
	change = true;




}// 关于accept group的还需要再考虑，应该顺序查找过去？？？
	

//因该只会合并成S0->S1
state* reduce_2_states(dfa_state* s1, dfa_state* s2, state_group* reduce, list<state*>::iterator& back) {
#ifdef debugmsg
	std::cout << "r2s: " << s1->state_num << " " << s2->state_num << endl;
#endif
	memset(skip_times, 0, 2000);

	


	for (auto&r : s2->parent) {
		r->find_e_times.clear();		
	}
	

	for (auto&r : s2->parent) {
		
		if (r != s2 && r != s1) {
			bool or_1_2 = false;
			bool or_1_3 = false;
#ifdef debugmsg
			for (auto&eo : r->edges) {
				cout << eo.transc << " " << eo.transto <<" "<<eo.transto->state_num<< endl;
			}
			cout << "s2: " << s2 <<" "<<s2->state_num<<  endl;
#endif
			auto result = find_e(r,r->edges.begin(), r->edges.end(),s2, 0);
			assert(result != r->edges.end());
#ifdef debugmsg
			cout << "Result  " << result->transc << endl;
#endif
			
			for (auto&c : r->edges) {
				or_1_3 = false;
				if (c.transto == s1 && !strncmp(c.transc, result->transc, 4))
					or_1_3 = true;;
			}
			if (!or_1_3)
			{
#if 0
				for (auto&q : s1->parent) {
					if (q == r)
						or_1_2 = true;   //考虑同样的边重复的问题
				}

				if (or_1_2)
				{

					cout << r->state_num << s1->state_num << " " << result->transc << endl;
					beparent_2(r, s1, result->transc);
				}
#endif
				cout << r->state_num << s1->state_num << "sp:" << result->transc << endl;
				beparent_1(r, s1, result->transc);
				

			}
		}
	}

	for (auto&q : s2->edges) {

		if (q.transto == s2) {
			beparent_1(s1, s1, q.transc);
			
		}
		if (q.transto == s1)
			continue;
		else if(q.transto!= s2&& q.transto == s1) {
			bool have_bofore22 = false;
			for (auto&c : s1->edges) {
				if (c.transto == q.transto && (!strncmp(c.transc, q.transc, 4)))
					have_bofore22 = true;
			}
			if (have_bofore22)
				continue;
			else {
				beparent_1(s1, q.transto, q.transc);
				
			}
		}
	}
	//是不是改用beparent而不是单纯的push_back
	for (auto&r : s2->parent) {
		if (r != s2) {
			for (auto&q = r->edges.begin(); q != r->edges.end(); q++)
				if (q->transto == s2)
				{
					r->edges.erase(q);
					break;
				}
		}
		else continue;
	}
	for (auto&r : s2->edges) {
		if (r.transto != s2) {
			for (auto&q = r.transto->parent.begin(); q != r.transto->parent.end(); q++) {
				if (*q == s2) {
					r.transto->parent.erase(q);
					break;
				}
			}
		}
		else continue;
	}
	for (auto r = reduce->group.begin(); r != reduce->group.end(); r++) {

		if ((*r) == s2) {
			r = reduce->group.erase(r);
			back = r;
			break;
		}

	}
	return s1;

}

void if_s_c_s_in_g(std::list<state*>& g) {
	for (auto q = g.begin(); q != g.end(); q++) {
		if ((*q)->s_or_p_in_g)
			continue;
		else {
			bool fin = false;
			for (auto&m : (*q)->parent) {
				for (auto&e : g) {
					if (e == m)
					{
						e->s_or_p_in_g = true;
						(*q)->s_or_p_in_g = true;
						fin = true;
						break;
					}
				}
				if (fin)
					break;
			}
			for (auto&m : (*q)->edges) {
				for (auto&e : g) {
					if (e == m.transto)
					{
						e->s_or_p_in_g = true;
						(*q)->s_or_p_in_g = true;
						fin = true;
						break;
					}
				}
				if (fin)
					break;
			}
		}
	}
}
bool if_merge_2(state* ep, state*qp, list<state*>::iterator& back) {
#ifdef debugmsg
	cout << "test" << " state " << ep->state_num << " " << qp->state_num << endl;
#endif
	//出现了test 5 5  test  5 6才对
	vector<vector<edgeto>::iterator> qp2ep;
	vector<vector<edgeto>::iterator> ep2qp;
	vector<vector<edgeto>::iterator> ep2ep;
	vector<vector<edgeto>::iterator> qp2qp;
	int time = 0;
	for (auto a = ebeg(ep); a != eend(ep); a++) {
		if (a->transto == ep) {
			ep2ep.push_back(a);
			cout << "ep2ep: " << a->transc << " " << ep->state_num << endl;
			continue;
		}
		if (a->transto == qp) {
			ep2qp.push_back(a);
			cout << "ep:" << ep->state_num << " " << a->transc << " " << "qp:" << qp->state_num << endl;
		}
		for (auto b = ebeg(qp); b != eend(qp); b++) {
			if (time == 0) {
				if (b->transto == qp) {
					qp2qp.push_back(b);
					cout << "qp2qp: " << b->transc << " " << qp->state_num << endl;
				}
				if (b->transto == ep)
				{
					qp2ep.push_back(b);
					cout << "qp:" << qp->state_num << " " << b->transc << " " << "ep:" << ep->state_num << endl;
				}
			}


		}
		time++;

	}
	bool size4 = qp2ep.size() && ep2ep.size() && qp2qp.size() && ep2qp.size();
	if (size4&& qp2ep.size() == ep2ep.size() && qp2qp.size() == ep2qp.size()) {
		for (auto& a : ep2qp) {
			bool find = false;
			for (auto&b : qp2qp) {
				if (!strncmp(a->transc, b->transc, 4)) {
					find = true;
					break;
				}
			}
			if (find == false)
				return false;
		}
		for (auto& a : qp2ep) {
			bool find = false;
			for (auto&b : ep2ep) {
				if (!strncmp(a->transc, b->transc, 4)) {
					find = true;
					break;
				}
			}
			if (find == false)
				return false;
		}
		reduce_2_states(ep, qp, ep->belong2g, back);
	
		return true;
	}
	bool size3 = ep2qp.size();
	if (size3&&ep2qp.size() == qp2qp.size()&&qp->edges.size()==ep->edges.size() && qp2ep.size() == 0&&ep2ep.size()==0) {
		bool same = false;
		for (auto&ee : ep2qp) {
			for (auto&qq : qp2qp) {
				if (!strncmp(ee->transc, qq->transc, 4)) {
					same = true;
				}
			}
			if (same == false)
				return false;
		}
		if (same) {
			reduce_2_states(ep, qp, ep->belong2g, back);
			return true;
		}
		else return false;
	}

	else return false;
	//parent son merge
}
# if 0
void if_merge(state* r,vector<edgeto>::iterator&e,vector<edgeto>::iterator&q, bool& not_find, merge_states&new_merge) {
	
	auto result1 = find_e(r->edges.begin(), r->edges.end(), q->transto);
			//刚开始的时候最多只可能有一条循环自身，每个分路只有一种情况，所以find_e找到的就是全部
			/*   />  e
			r    | ^
			\  v |
			\> q

			*/
	if (result1 != r->edges.end()) { //考虑有多条环路的情况
				auto result2 = find_e(result1->transto->edges.begin(), result1->transto->edges.end(), e->transto);
				if (result2 != result1->transto->edges.end()) {
					auto result3 = find_e(e->transto->edges.begin(), e->transto->edges.end(), e->transto);
					if (result3 != e->transto->edges.end()) {
						auto result4 = find_e(result1->transto->edges.begin(), result1->transto->edges.end(), result1->transto);
						if (result4 != result1->transto->edges.end()) {
							if ((!strncmp(q->transc, result1->transc, 4)) && (!strncmp(result2->transc, result3->transc, 4))) {
								reduce_2_states(e->transto, q->transto, e->transto->belong2g);
							}
						}
						else goto next_exec;
					}
					else goto next_exec;
				}
				/*以上用来找出两个互联的点是否能够合并*/
				/*目前看来需要找到马上就合并，否则不好递归合并后面的内容*/
			next_exec:
				not_find = false;
				
			}
	
	
}
// son son merge
#endif
void reduce_a_group(std::list<state*>& g) {
	//accept state 和not 不可能在一个集合里
	//当两个节点互联且自身循环的时候，可以合并
	//如果没有环路，两个节点的高度一样的话，一样可以合并
	//如果在同一高度，但是互联，但不自身循环，则不能合并，所以目前看来找出高度才是关键
	//尝试如何找出高度最快
	std::list<state*>& reduce = g;
#if 0
	vector<state*> reduce;
	for (auto&r : g) {
		reduce.push_back(r);
	}
#endif
	if_s_c_s_in_g(reduce);	
	int *visit_times = new int[g.size()];
	memset(visit_times, 0, g.size() * 4);
	if ((*g.begin())->accept_state == true) {
		
#if 0
		for (auto&r : accept_reduce) {
			for (auto &q : (*r).parent) {
				for (auto& e = q->edges.begin(); e != q->edges.end(); e++) {
					if ((*e).transto == r) {
						reduce_state->parent.push_back(q);
						q->edges.push_back(edgeto(reduce_state, e->transc));
						q->edges.erase(e); // save e? in reduce_state

					}
				}
			}
		}
#endif
		if (reduce.size() >= 2) {
			for (auto r = reduce.begin(); r != reduce.end(); r++)
			{
# if 0
				for (auto&q = r->edges.begin(); q != r->edges.end(); q++) {

					bool exist = false;
					for (auto& e : reduce_state->edges) { //看这条边是否存在在reduce_state
						if (e.transto != q->transto&&strncmp(e.transc, q->transc, 4))
							continue;
						else {
							need_erease.push_back(q); //是否需要删除q里得parent 指向r得部分
							exist = true;
							break;
						}
					}
					if (exist == false) {
						beparent_1(reduce_state, q->transto, q->transc);
					}
				}
				for (auto&q = r->parent.begin(); q != r->parent.end(); q++) {
					parent_e.push_back(parent_erease_index(*q));
					for (auto&e = (*q)->edges.begin(); e != (*q)->edges.end(); e++) {
						if (e->transto == r) {
							(parent_e.end() - 1)->son_erease.push_back(e);
						}
					}
				}
				for (auto&q : need_erease) {
					r->edges.erase(q);
				}
				for (auto&q : parent_e) {
					for (auto&e : q.son_erease)
						(q.parent_need_e)->edges.erase(e);
				}
#endif;
				auto realtemp = *r;
				if (visit_times[(*r)->state_num] > 0)
					continue;
				else {
#if 0
					auto self = r->edges.end();
					for (auto&q = r->edges.begin; q != r->edges.end(); q++) {
						if (q.transto = r)
							self = q;
						else continue;
					}
					visit_times[r->state_num]++;
					for (auto e = r->edges.begin(); e != r->edges.end(); r++) {
						if (e->transto->belong2g == r->belong2g) {
							if (e->transto == r) {
								bool son_recycle = false;
								for (auto &sonself : e->transto->edges) {
									if (sonself.transto = e->transto)
										son_recycle = true;
									else continue;
								}
								if (self != r->edges.end() && son_recycle) {
									merge_states new_merge;
									new_merge.push_back(e->transto);
									new_merge.push_back(r);
									all_merge.push_back(new_merge);
								}
							}
						}
					}
#endif;
					merge_states new_merge;
					int merge1 = 0;
					int merge2 = 0;
					list<state*>::iterator back;

					cout << endl;
					if (!(*r)->edges.empty()) {
						for (auto e = (*r)->edges.begin(); e != (*r)->edges.end(); e++) {

							state* fuck = (*r)->edges.begin()->transto;
							bool fuck1 = false;
							state* tempptr = e->transto;
#if 0
							for (auto&cp : e->transto->edges) { //原来是(*r)->edge
								if (fuck == cp.transto)
								{
									continue;
									fuck = cp.transto;
								}
								else if (fuck != cp.transto)
								{
									fuck1 = true;
									break;
								}
							}
							if (fuck1 == false)
							{

								if_merge_2((*r), e->transto, back);

								break;
							}
#endif
							char* confirm_str = e->transc;
#ifdef debugmsg
							for (auto&op : (*r)->edges) {
								cout << "redges: " << op.transto->state_num << " ";
							}
#endif
							bool find1 = false;
							bool find2 = false;
							//需要检查e是否和r互联吗？如果互联，需要合并吗？
							//想法：不管，先把子子节点能合并的合并了，再来考虑父子节点
							//目前没有问题，但是在子子合并后考虑父子，子有两个自身环路，那么需要多检测一次，参照图
							find2 = if_merge_2((*r), e->transto, back);
							if (find2 == true)
							{
								auto ne = ebeg((*r));
								while (!strncmp(ne->transc, confirm_str, 4) && ne->transto != tempptr) {
									ne++;
								}
								e = ne;
								break;
							} //当找到re可以合并，那么直接开始下一个子节点的循环了
							for (auto q = e->transto->edges.begin(); q != e->transto->edges.end(); q++) {
								if (q->transto == e->transto)
									continue;
								find1 = if_merge_2(e->transto, q->transto, back);//检测e是否和r中的任意一条边可以合并
								if (find1 == true && (*r)->edges.size() != 1) {
									break;
								}
							}
							//当找到 EQ可以合并，那么这次循环就结束了
							if (find1) {
								auto ne = ebeg((*r));
								while (!strncmp(ne->transc, confirm_str, 4) && ne->transto != tempptr) {
									ne++;
								}
								e = ne;
								
							}
							

							if (!find1 && !find2) {
								if (e->transto->parent.size() == 1)
									new_merge.push_back(e->transto); //合并判定有问题，即使不能被if_merge也不能直接扔向量里，因为存在不能合并的情况
							}


						}
#if 0
						if (!new_merge.empty()) {
							//检查每条edge是否相同，并且不指向父亲，自身和兄弟，如果是那么可以合
							std::list<state*>::iterator back;
							int count1 = 0;
							int count2 = 0;
							for (auto&iter1 = new_merge.begin(); iter1 != new_merge.end(); iter1++)
							{

								count2 = 0;
								for (auto&iter2 = iter1 + 1; iter2 != new_merge.end(); iter2++) {

									bool find1 = true;
									bool find2 = false;
									for (auto&edge1 : (*iter1)->edges)
									{
										for (auto&edge2 : (*iter2)->edges) {
											if (edge1.transto == edge2.transto && !strncmp(edge1.transc, edge2.transc, 4))
												find2 = true;
										}
										if (find2)
										{
											reduce_2_states(*iter1, *iter2, (*iter1)->belong2g, back);
											new_merge.erase(iter2);
										}
										else
										{
											find1 = false;
											break;
										}
									}
									if (find1) {
										iter2 = new_merge.begin() + count1 + 1 + count2;
										continue;
									}
									count2++;
								}
								iter1 = new_merge.begin() + count1;
								count1++;
							}
						}
#endif
					}
					else {

					}
					//delete need_erease && parent_e
				}
			}
		}
	}

    else {
		
		if (reduce.size() >= 2) {
			list<state*>::iterator back;
			list<state*>::iterator r;
			auto temp = reduce.begin();
			while (temp!=reduce.end()) {
				if ((*temp)->s_or_p_in_g) {
					temp++;
					continue;
				}
				else {
					r = temp;
					break;
				}
			}
			if (temp == reduce.end()) {
#ifdef debugmsg
				cout << "ao can no" << endl;
#endif
				return;
			}
			auto&r1 = reduce.begin();
			
			while (r1 != reduce.end()) {
				if ((*r1)->state_num != (*r)->state_num && !(*r1)->s_or_p_in_g) {
					
					reduce_2_states((*r), (*r1), (*r)->belong2g, back);
					r = reduce.begin();
					r1 = reduce.begin();
				}
				r1++;
			}
		}
	}

}

void reduce_states() {
	for (auto&r : group_queue) {
		reduce_a_group(r->group);
	}
	
	for (auto&r : group_queue) {
		bool real = false;
		if ((*r->group.begin())->accept_state == true)
		{
			for (auto&cq : r->group) {
				for (auto&ep : cq->parent) {
					ep->find_e_times.clear();
				}
				cq->find_e_times.clear();
			}
			if (r->group.size() >= 2) {
				merge_states new_m;
				list<state*>::iterator back;

				auto&r1 = r->group.begin();

				while (r1 != r->group.end()) {
					bool f = true;
					for (auto&c : (*r1)->edges)
					{
						if (c.transto->belong2g == (*r1)->belong2g&&c.transto!=*r1)
							f = false;
					}
					for (auto&c : (*r1)->parent)
					{
						if (c->belong2g == (*r1)->belong2g&&c!=*r1)
							f = false;
					}
					if (f) {
						new_m.push_back(*r1);
					}
					r1++;
				}
				if (!new_m.empty()) {
					
					
					list<state*>::iterator back;
					
					int num = 0;
#if 0
					while (b1 != new_m.end()) {
						auto b2 = b1;
						b2++;
						if (b2 != new_m.end()) {
							num = (*b2)->state_num;
						}
						
						auto b3 = b;
						while ((*b3)->state_num != num&&b3!= new_m.end())
							b3++;
						if (b3 != new_m.end())
							b1 = b3;
						else break;
					}
#endif
					
					while (new_m.size() > 1) {
						auto b = new_m.begin();
						auto b1 = new_m.begin();
						b1++;
						reduce_2_states(*b, *b1, (*b)->belong2g, back);
						new_m.erase(b1);
						
					}
				}
				
			}
		}

	}

}
void reduce_redundant_edges_and_parent()	{
	for (auto&g : group_queue) {
		for (auto&p: g->group) {

			int time = 0;
			if (p->parent.size() > 1)
			{
				vector<state*> new_p;
				for (auto e = p->parent.begin(); e != p->parent.end() ; e++) {
					bool in = false;
					for (auto&q : new_p) {
						if (q == *e) {
							in = true;
							break;
						}
					}
					if (!in)
						new_p.push_back(*e);
				}
				p->parent.clear();
				for (auto&q : new_p)
					p->parent.push_back(q);

			}
			time = 0;
			if (p->edges.size() > 1) {
				vector<edgeto> new_p;
				for (auto e = p->edges.begin(); e != p->edges.end(); e++) {
					bool in = false;
					for (auto&q : new_p) {
						if (q.transto==e->transto&&!strncmp(q.transc,e->transc,4)) {
							in = true;
							break;
						}
					}
					if (!in)
					{
						new_p.push_back(*e);

				
					}
				}
				p->edges.clear();
				for (auto&q : new_p)
					p->edges.push_back(q);
			}
		}
	}
}

void test_deal_with() {
	int i = 0;
	for (auto&r : group_queue) {
		cout << "group: " << i++ << endl;
		for (auto &q : r->group)
		{
			cout << q->state_num <<"  ";

		}
		cout << endl;
	}
}
void test_deal_with2() {
	int i = 0;
	for (auto&r : group_queue) {
		cout << "group: " << i++ << endl;
		for (auto &q : r->group)
		{
			cout << q->state_num << endl;
			for (auto&c : q->edges)
				cout << c.transto->state_num << " " << c.transc << endl;

			for (auto&c : q->parent)
				cout << "p : " << c->state_num << endl;
		}
		cout << endl;
	}
}
//完成规约
//完成对每个state内的state归并
void least_DFA(dfa_state* s ) { //猜测需要统计DFA tree里每个节点的高度？？？
	state_group* n_a_s = new state_group;
	for (auto& r : not_accept_states) {
		n_a_s->group.push_back(r);
		n_a_s->num.push_back(r->state_num);
		r->belong2g = n_a_s;
	}
	memcpy(n_a_s->transc, (*(n_a_s->group.begin()))->edges[0].transc, 3);
	state_group* a_s = new state_group();
	n_a_s->connectgroup.push_back(a_s);
	for (auto&r : accept_states) {
		a_s->group.push_back(r);
		n_a_s->num.push_back(r->state_num);
		r->belong2g = a_s;
	}
	
	a_s->connectgroup.push_back(a_s);
	group_queue.push_back(n_a_s);
	group_queue.push_back(a_s);
	
	find_parent_not_self_and_son(group_queue);
	
	memset(skip_times, 0, 2000);
	reduce_states();
	reduce_redundant_edges_and_parent();
	test_deal_with2();
	//合并这里有些问题

	

}


