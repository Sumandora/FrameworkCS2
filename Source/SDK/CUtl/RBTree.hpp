#pragma once

#include "Memory.hpp"

template <typename T, typename I = int>
struct UtlRBTree {
	struct TreeNode {
		I left;
		I right;
		I parent;
		// I tag;
		T data;
	};
	static constexpr I INVALID_INDEX = static_cast<I>(-1);

	bool (*comparator)(const T&, const T&);
	
	CUtlMemory<TreeNode> memory;
	int root;
	int elements_count;
	int first_free;
	TreeNode* elements;
};
