#include"stdafx.h"
#include"RE2DFA.h"

void release_nfa_memory(state& s0) {
	if (s0.edges.empty()) {
		return;
	}
	else {
		for (auto&r : s0.edges)
			release_nfa_memory(*(r.transto));
	}
	for (auto&r : s0.edges) {
		delete r.transto;
	}
}