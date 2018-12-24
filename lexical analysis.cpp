// lexical analysis.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include"RE2DFA.h"
#include"analysis.h"
#include "testparse.h"
#include<iostream>
/*C语言分为标识符和运算符和关键字
标识符又分为 常量 变量 函数名 类名等等
分析的时候应该区别,标志符必须以字母开头,字母和数字的组合，除了_,$
关键字是C标准规定的保留标志符，如int class struct等
运算符就是各种运算符号
, ;  空格之类的单独分析
*/

/*转移表的设计
      character   digit   operator  ;,space  $_  other
s0      
s1
s2
.
.
.
sn

return:  sa          sb           sc         sd  ....
       keywords    identifier    operator   ......
*/
/*思路：
   先人工写出所有标识符，关键字，运算符 等的RE，
   然后将RE转化为最小DFA，然后用算法填写转移表
   因为看的别人的项目是手工编码实现的，所以我这里
   改为表词驱动实现
*/

extern int next_state_num;

int main()
{
#if 0 
	state sy_s0;
	state* dfa_s0_sy = nullptr;
	state* dfa_s0_op = nullptr;
	state* dfa_s0_fn = nullptr;
	achieve_nfa_sy(sy_s0, dfa_s0_sy);
	
	next_state_num = 0;
	state op_s0;
	achieve_nfa_op(op_s0, dfa_s0_op);
	
	

	read_file_and_analysis(dfa_s0_sy, dfa_s0_op);
	//解决重复边的问题，或者干脆不最小化了，我觉得没毛病
#endif
	test_FIRST();
}

/*
整理一下思路：从RE构造到NFA，如果字节不在一个括号里，那么直接连
起来用空转移就行了，如果在一个括号里，就开始分析是否有brank，注意
一个括号内部最多只能看到下一级的括号，如(1(2)) (1(2(3)))1能
看到2，却不能看到3，每次遇到|都会将原来的字符串流分为两部分,
根据架构*?+前面最多只能有一个括号，只用看前面一位就行了*/