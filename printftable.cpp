#include"stdafx.h"
#include<iostream>
#include"RE2DFA.h"
#define DEBUG

class transitem {
public:
	std::vector<int> alltransto;
	void operator=(int m) {
		alltransto.push_back(m);
	}
};
std::ostream& operator <<(std::ostream& os, transitem& ts) {
	if (ts.alltransto.empty()) {
		std::cout << 0;
		return os;
	}
	if (ts.alltransto.size() == 1) {
		std::cout << ts.alltransto[0];
		return os;
	}
	for (auto&r : ts.alltransto)
	{
		std::cout << r << " ";
	}
	return os;
}

int tran_to_num(char* trans) {
	
	if (trans[1] == '-') {
		if ('a'<=trans[0] && trans[0]<='z'&&trans[0] != 0) {
			
			return 0;
		}
		if ('A'<=trans[0] && trans[0]<='Z'&&trans[0] != 0) {
			
			return 1;
		}
		if ('0'<=trans[0] && trans[0]<='9'&&trans[0] != 0) {
			
			return 2;
		}
	}
	if('a'<=trans[0]&&trans[0]<='z'&&trans[0] != 0){
		
		return 0;
	}
	if('A'<=trans[0]&&trans[0]<='Z'&&trans[0]!=0){
		
		return 1;
	}
	if ('0' <= trans[0]&&trans[0] <= '9'&&trans[0] != 0) {
	
		return 2;
	}
	if(trans[0]=='_'&&trans[0] != 0){
		
		return 3;
	}
	if(trans[0]=='$'&&trans[0] != 0){
		
		return 4;
	}
	if(trans[0]==0&&trans[1]==0&&trans[2]==0){
		
		return 5;
	}
	else {
		
		return 6;
	}
}
int tran_to_num_op(char* trans) {
	if (trans[0] == 0 && trans[1] == 0 && trans[2] == 0) {

		return 21;
	}
	if (trans[0] == '\\') {
		return 0;
	}
	if (trans[0] == '(') {
		return 1;
	}
	if (trans[0] == ')') {
		return 2;
	}
	if (trans[0] == '[') {
		return 3;
	}
	if (trans[0] == ']') {
		return 4;
	}
	if (trans[0] == '+') {
		return 5;
	}
	if (trans[0] == '-') {
		return 6;
	}
	if (trans[0] == '*') {
		return 7;
	}
	if (trans[0] == '/') {
		return 8;
	}
	if (trans[0] == '>') {
		return 9;
	}
	if (trans[0] == '<') {
		return 10;
	}
	if (trans[0] == '!') {
		return 11;
	}
	if (trans[0] == '~') {
		return 12;
	}
	if (trans[0] == '&') {
		return 13;
	}
	if (trans[0] == '%') {
		return 14;
	}
	if (trans[0] == '=') {
		return 15;
	}
	if (trans[0] == '|') {
		return 16;
	}
	if (trans[0] == '^') {
		return 17;
	}
	if (trans[0] == '?') {
		return 18;
	}
	if (trans[0] == ':') {
		return 19;
	}
	if (trans[0] == ',') {
		return 20;
	}
	else return 22;
}
void traversal_state(state* s, transitem** table) {
	s->traversal_before = true;
	
	if (s->edges.empty())
		return;
	
	else {
		int temp = 0;
		
		for (int i = 0; i != s->edges.size(); i++) {
			
			temp = tran_to_num(s->edges[i].transc);
			table[s->state_num][temp] = s->edges[i].transto->state_num;
			//s2->s3 or s2->s1 如果都通空转移，那么会占掉格子，导致数据刷新
#if 0
			std::cout << table[s->state_num][temp] << std::endl;

			std::cout << "s" << s->state_num << "->" << "s" << s->edges[i].transto->state_num <<"s c"<< s->edges[i].transc<< std::endl;
#endif // DEBUG
		}
		for (int i = 0; i != s->edges.size(); i++) {
			if (!s->edges[i].transto->traversal_before)
			{
				traversal_state(s->edges[i].transto, table);
			}
		}
	}
}
void traversal_state_op(state* s, transitem** table) {
	s->traversal_before = true;

	if (s->edges.empty())
		return;

	else {
		int temp = 0;

		for (int i = 0; i != s->edges.size(); i++) {

			temp = tran_to_num_op(s->edges[i].transc);
			table[s->state_num][temp] = s->edges[i].transto->state_num;
			//s2->s3 or s2->s1 如果都通空转移，那么会占掉格子，导致数据刷新
#if 0
			std::cout << table[s->state_num][temp] << std::endl;

			std::cout << "s" << s->state_num << "->" << "s" << s->edges[i].transto->state_num << "s c" << s->edges[i].transc << std::endl;
#endif // DEBUG
		}
		for (int i = 0; i != s->edges.size(); i++) {
			if (!s->edges[i].transto->traversal_before)
			{
				traversal_state_op(s->edges[i].transto, table);
			}
		}
	}
}
void traversal_state(state* s) {
	s->traversal_before = true;

	if (s->edges.empty())
		return;

	else {
		int temp = 0;
#ifdef debugmsg
		std::cout << s->state_num << " to:" << std::endl;
#endif
		for (int i = 0; i != s->edges.size(); i++) {
			std::cout << "par: " << s->state_num << std::endl;
			std::cout << s->edges[i].transto->state_num << s->edges[i].transc << std::endl;
			temp = tran_to_num_op(s->edges[i].transc);
			
			//s2->s3 or s2->s1 如果都通空转移，那么会占掉格子，导致数据刷新
#if 0
			std::cout << table[s->state_num][temp] << std::endl;

			std::cout << "s" << s->state_num << "->" << "s" << s->edges[i].transto->state_num << "s c" << s->edges[i].transc << std::endl;
#endif // DEBUG
		}
		for (int i = 0; i != s->edges.size(); i++) {
			if (!s->edges[i].transto->traversal_before)
			{
				traversal_state(s->edges[i].transto);
			}
		}
	}
}
void printf_nfa_table(state&s0,int next_state_num) {
	/*
	       0a-z 1A-Z 21-9 3_ 4$ 5kong 6other
		s0
		s1
		sn
	*/
	transitem** trans_table = new transitem*[next_state_num];
	for (int i = 0; i!= next_state_num; i++)
		trans_table[i] = new transitem[7];
	
#if 0
	for (int i = 0; i != next_state_num; i++) {
		for (int j = 0; j != 7; j++)
		{
		   std::cout << trans_table[i][j];
		}
		std::cout << std::endl;
	}
#endif // DEBUG
	int* state_index = new int[next_state_num];
	memset(state_index, 0, next_state_num);
	traversal_state(&s0, trans_table);


	std::cout << "    a-z" << "  A-Z" << "  1-9" << " _" << "  &" << "  k" << "  o" << std::endl;
	for (int i = 0; i!= next_state_num; i++) {
		std::cout << "s" << i
			<< "   " << trans_table[i][0]
			<< "    " << trans_table[i][1]
			<< "    " << trans_table[i][2]
			<< "  " << trans_table[i][3]
			<< "  " << trans_table[i][4]
			<< "  " << trans_table[i][5]
			<< "  " << trans_table[i][6] << std::endl;
	}

}

void traversal_set_0(state* s) {
	if (s->traversal_before1 == true)
		return;
	s->traversal_before1 = true;
	s->traversal_before = false;
	if (s->edges.empty())
		return;
	for (auto&r : s->edges) {
		if (r.transto->traversal_before1)
			continue;
		else
		{
			
			traversal_set_0(r.transto);
		}
		
	}
}

void printf_nfa_table_op(state&s0, int next_state_num) {
	transitem** trans_table = new transitem*[next_state_num];
	for (int i = 0; i != next_state_num; i++)
		trans_table[i] = new transitem[23];
	int* state_index = new int[next_state_num];
	memset(state_index, 0, next_state_num);
	traversal_state_op(&s0, trans_table);
	std::cout << std::endl;
	std::cout << "     \\" << "  (" << "  )" << "  [" << "  ]" << "  +" << "  -" << "  *" << "  /" << "  >" << "  <" << "  !" << "  ~" << "  &" << "  %" << "  =" << "  |" << "  ^" << "  ?" << "  :" << "  ," <<"  k"<< "  o"<<std::endl; 
	//21
	for (int i = 0; i != next_state_num; i++) {
		std::cout << "s" << i
			<< "   " << trans_table[i][0]
			<< "  " << trans_table[i][1]
			<< "  " << trans_table[i][2]
			<< "  " << trans_table[i][3]
			<< "  " << trans_table[i][4]
			<< "  " << trans_table[i][5]
			<< "  " << trans_table[i][6]
			<< "  " << trans_table[i][7]
			<< "  " << trans_table[i][8]
			<< "  " << trans_table[i][9]
			<< "  " << trans_table[i][10]
			<< "  " << trans_table[i][11]
			<< "  " << trans_table[i][12]
			<< "  " << trans_table[i][13]
			<< "  " << trans_table[i][14]
			<< "  " << trans_table[i][15]
			<< "  " << trans_table[i][16]
			<< "  " << trans_table[i][17]
			<< "  " << trans_table[i][18]
			<< "  " << trans_table[i][19]
			<< "  " << trans_table[i][20]
			<< "  " << trans_table[i][21]
			<< "  " << trans_table[i][22] << std::endl;
		    
	}

}