#include"stdafx.h"
#include"RE2DFA.h"
#include<string>
#include<iostream>
#include<cstdio>
#include<vector>
#include"analysis.h"
#include<cstdio>

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::cin;
// k return 0  o return 1 p return 2 t return 3  s 4  e5
#if 0
#define str s.c_str()


bool compare_c_transc(char& c, char* transc) {
	if(65<=c&&c<=90){
		if (!strcmp(transc, "A-Z"))
			return true;
		else return false;
	}
	if(97<=c&&c<=122){
		if (!strcmp(transc, "a-z"))
			return true;
		else return false;
	}
	if(48<=c&&c<=57){
		if (!strcmp(transc, "19"))
			return true;
		else return false;
	}
	if (c == '$' && !strcmp(transc, "$"))
		return true;
	if (c == '_' && !strcmp(transc, "_"))
		return true;
	else return false;

}
int analysis_lex(string& s) {
	if (s == "{" || s == "}")
		return 3;
	if (!strcmp(str, "sizeof"))
		return 1;
	if (s == "#")
		return 2;
	for (int i = 0; i != 35; i++) {
		if (!strcmp(str, keywords[i])) {
			return 0;
		}
	}
	for (int i = 0; i != 47; i++) {
		if (!strcmp(str, operatorsymbol[i]))
			return 1;
	}
	
	for (int i = 0; i != s.size(); i++) {
		bool same = false;
		for (auto&r : now->edges)
		{
			same=compare_c_transc(s[i], r.transc);
			if (same) {
				now = r.transto;
				break;
			}
		}
		if (same == false)
			return 5;
	}
	if (now->accept_state)
		return 4;
	else return 5;
}

void ana_header(char* s, int pos) {
// 1-9 a-z _ $ =type 1 ;space tab	type 2; other type 3 
	while (true) {

	}
}

void read_code_and_ana(string& codepath) {
	FILE* code=fopen(codepath.c_str(), "r");
	assert(code != nullptr);
	char buf[1024];
	memset(buf, 0, 1024);
	char* result = nullptr;
	while (!feof(code)) {
		result = fgets(buf, 1024, code);
		if(result!=nullptr)
		{ 
			
		}
		else {
			std::cerr << "error read" << std::endl;
			break;
		}
	}
}
#endif



bool compare_sy(char c_str[], char c) {
	if (!strncmp(c_str, "a-z", 4)) {
		if (97 <= c && c <= 122)
			return true;
		else return false;
	}
	if (!strncmp(c_str, "A-Z", 4)) {
		if (65 <= c && c <= 90)
			return true;
		else return false;
	}
	if (!strncmp(c_str, "1-9", 4)) {
		if (48 <= c && c <= 57)
			return true;
		else return false;
	}
	else return !strncmp(c_str, &c, 1);

	
}
/* keywords 0 
   symbol  1
   operator 3
   #      4
   { }  5
   char   6
   str    7
   ;     8
   fn 9
   10  number
   err 11*/
int analysis_quote(char* c_s, vector<int>&quote2_pos, vector<int>& quote1_pos) {
	string str(c_s);
	int str_size = str.size();
	
	 //c处理引号中得引号，里面得引号得之前必定有一个\字符，这样chul
	bool escape = false;
	for (int i = 0; i != str_size; i++) {
		if (str[i] == '\\') {
			escape = true;
			continue;
		}
		if (str[i] == '\"'&&!escape) {
			quote2_pos.push_back(i);
		}
		if (str[i] == '\''&&!escape) {
			quote1_pos.push_back(i);
		}
		
		escape = false;
		
	}
	if ((quote1_pos.size()) % 2 != 0 || (quote2_pos.size() % 2) != 0)
		return 0;  //means err
	else return 1;
} //返回所有非转义" 和'得位置

int analysis_if_include_filename(char* c_s,int& fn_beg,int& fn_end) {
	string str(c_s);
	int i = 0;
	while (str[i] == ' ')
		i++;
	if (str[i] == '#') {
		i++;
		string temp(str.begin() + i, str.begin() + i + 7);
		if (temp == "include") {
			i = i + 7;
			while (str[i] == ' ')
				i++;
			if(str[i]=='\"'||str[i]=='<'){
				fn_beg = (++i);
				while (i!=str.size()&&str[i] != '\"' &&str[i] != '>')
					i++;
				if (i == str.size())
					return -1;
				else {
					fn_end = i;
					return 1;
				}
			}
			else return -1;
		}
		
		else return 0;
	}
	else return 0;
}

int analysis_lex(char* c_s,state* dfa_sy,state* dfa_op,vector<lex_with_att>& result) {
	string str(c_s);
	cout << "now: " << str << endl;
	int pos = 0;
	int str_size = str.size(); 
	state* now_op = dfa_op;
	state* now_sy = dfa_sy;
	
	bool escape_before = false;
	int fn_beg = -1;
	int fn_end = -1;
	bool if_file = analysis_if_include_filename(c_s, fn_beg, fn_end);
	if (if_file == -1) {
		cout << "err grammar" << endl;
		return 0;
	}
	vector<int> quote2_pos;  vector<int> quote1_pos;
	bool if_err=analysis_quote(c_s, quote2_pos, quote1_pos);
	vector<beg_end_quote>quote1; vector<beg_end_quote> quote2;
	if (!if_err) {
		cout << "err grammar" << endl;
		return 0;
	}
	else {
		if (!quote2_pos.empty()) {
			for (int i = 0; i < quote2_pos.size() - 1;) {
				quote2.push_back(beg_end_quote(quote2_pos[i], quote2_pos[i + 1]));
				i = i + 2;

			}
		}
		if (!quote1_pos.empty()) {
			for (int i = 0; i < quote1_pos.size() - 1;) {
				quote1.push_back(beg_end_quote(quote1_pos[i], quote1_pos[i + 1]));
				i = i + 2;
			}
		}
	}
	int cursor1 = 0;
	int cursor2 = 0;
	int sy_start_pos = -1;//在dfa table里分析的时候，用来标记一串字符串的开始
	int op_start_pos = -1;

	while (pos < str_size) {
		
		//当POS等于以上两个点时，必定时char or str;
		
		
		bool in_table = false;
		bool ana_achieve = false;
		if (now_op == dfa_op && pos != fn_beg) {
			for (auto& edge : now_sy->edges) {


				in_table = compare_sy(edge.transc, str[pos]);
				if (in_table) {
					//当返回true 表示找到了相同得边
					if (now_sy == dfa_sy)
						sy_start_pos = pos;
					now_sy = edge.transto;
					pos++;
					break;
				}//能

			}
			if (!in_table&&sy_start_pos != -1 && now_sy != dfa_sy) {
				if (now_sy->accept_state) {

					string temp(str.begin() + sy_start_pos, str.begin() + pos);
					bool iskey = false;
					for (int i = 0; i != 35; i++) {
						if (keywords[i] == temp) {
							iskey = true;
							break;
						}
					}
					if (iskey) {
						result.push_back(lex_with_att(temp, 0));
					}
					else
					{
						result.push_back(lex_with_att(temp, 1));
					}
					now_sy = dfa_sy;
					sy_start_pos = -1;
					ana_achieve = true;

				}
				else {
					return -1;
				}
			}
			if (in_table || ana_achieve) {
				ana_achieve = false;
				continue;
			}
		}
		if (now_sy == dfa_sy) {
			for (auto& edge : now_op->edges) {
				
				if (edge.transc[0] == str[pos]) {
					//当返回true 表示找到了相同得边
					in_table = true;
					if (now_op == dfa_op)
						op_start_pos = pos;
					now_op = edge.transto;
					pos++;
					break;
				}
				//能
			}
			if (!in_table&& op_start_pos != -1 && now_op != dfa_op) {
				if (now_op->accept_state) {

					string temp(str.begin() + op_start_pos, str.begin() + pos);
					result.push_back(lex_with_att(temp, 3));
					now_op = dfa_op;
					op_start_pos = -1;
					ana_achieve = true;

				}
				else {
					return -1;
				}
			}
			if (in_table || ana_achieve) {
				ana_achieve = false;
				continue;
			}
		}
		if (now_sy == dfa_sy && str[pos] >= 48 && str[pos] <= 57) {
			result.push_back(lex_with_att(string(str.begin()+pos,str.begin()+pos+1), 10));
			pos++;
			sy_start_pos = -1;
			op_start_pos = -1;
			continue;
		}
		//是否应该将两个表的指针指到初始位置？
		if (if_file == 1) {
			if (pos == fn_beg) {
				result.push_back(lex_with_att(string(str.begin() + fn_beg, str.begin() + fn_end), 9));
				pos = fn_end;
				sy_start_pos = -1;
				op_start_pos = -1;
				continue;
			}
		}
		if(cursor1 < quote1.size()) {
			if (cursor1 < quote1_pos.size() && !quote1.empty() && pos == quote1[cursor1].beg&&str[pos] == '\'') {
				result.push_back(lex_with_att(string("\'"), 3));
				result.push_back(lex_with_att(string(str.begin() + pos + 1, str.begin() + quote1[cursor1].end), 6));
				result.push_back(lex_with_att(string("\'"), 3));
				pos = quote1[cursor1].end + 1;
				sy_start_pos = -1;
				op_start_pos = 0;
				cursor1++;
				continue;
			}
		}
		if (cursor2 < quote2.size()) {
			if (pos == quote2[cursor2].beg&&str[pos] == '\"') {
				result.push_back(lex_with_att(string("\""), 3));
				result.push_back(lex_with_att(string(str.begin() + pos + 1, str.begin() + quote2[cursor2].end), 7));
				result.push_back(lex_with_att(string("\""), 3));
				pos = quote2[cursor2].end + 1;
				sy_start_pos = -1;
				op_start_pos = -1;
				cursor2++;
				continue;
			}
		}
		
		if (str[pos] == '{') {
			result.push_back(lex_with_att(string("{"), 5));
			pos++;
			continue;
		}
		if (str[pos] == '}') {
			result.push_back(lex_with_att(string("}"), 5));
			pos++;
			continue;
		}
		if (str[pos] == '#') {
			result.push_back(lex_with_att(string("#"), 4));
			pos++;
			continue;
		}
		if (str[pos] == ';') {
			result.push_back(lex_with_att(string(";"), 8));
			pos++;
			continue;
		}  //考虑错误的情况
		else {
			pos++;
			continue;
			}//错误的情况
			
		
		

	}
	return 0;
}



void read_file_and_analysis(state* dfa_sy, state* dfa_op) {
	FILE *f;
	fopen_s(&f,"test.c", "r");
	char code_line[1024];
	memset(code_line, 0, 1024);
	
	if (f != nullptr) {
		
		char* temp = nullptr;
		int back = 0;
		while (true) {

			temp=fgets(code_line, 1024, f);
			if (temp == NULL)
				break;
			else {
				vector<lex_with_att> result;
				back=analysis_lex(temp, dfa_sy, dfa_op,result);
				memset(code_line, 0, 1024);
				if (back == -1) {
					cout << "err in ana" << endl;
					break;
				}
				else {
					for (auto& r : result) {
						cout << "<" << r.lex << "," << r.att << ">" << endl;
					}
					result.clear();
				}
			}
		}
	} //明天测试读取和分析函数
	else {
		cout << "fuck " << endl;
	}
}