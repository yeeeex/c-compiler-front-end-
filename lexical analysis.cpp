// lexical analysis.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include"RE2DFA.h"
#include"analysis.h"
#include "testparse.h"
#include<iostream>
/*C���Է�Ϊ��ʶ����������͹ؼ���
��ʶ���ַ�Ϊ ���� ���� ������ �����ȵ�
������ʱ��Ӧ������,��־����������ĸ��ͷ,��ĸ�����ֵ���ϣ�����_,$
�ؼ�����C��׼�涨�ı�����־������int class struct��
��������Ǹ����������
, ;  �ո�֮��ĵ�������
*/

/*ת�Ʊ�����
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
/*˼·��
   ���˹�д�����б�ʶ�����ؼ��֣������ �ȵ�RE��
   Ȼ��REת��Ϊ��СDFA��Ȼ�����㷨��дת�Ʊ�
   ��Ϊ���ı��˵���Ŀ���ֹ�����ʵ�ֵģ�����������
   ��Ϊ�������ʵ��
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
	//����ظ��ߵ����⣬���߸ɴ಻��С���ˣ��Ҿ���ûë��
#endif
	test_FIRST();
}

/*
����һ��˼·����RE���쵽NFA������ֽڲ���һ���������ôֱ����
�����ÿ�ת�ƾ����ˣ������һ��������Ϳ�ʼ�����Ƿ���brank��ע��
һ�������ڲ����ֻ�ܿ�����һ�������ţ���(1(2)) (1(2(3)))1��
����2��ȴ���ܿ���3��ÿ������|���Ὣԭ�����ַ�������Ϊ������,
���ݼܹ�*?+ǰ�����ֻ����һ�����ţ�ֻ�ÿ�ǰ��һλ������*/