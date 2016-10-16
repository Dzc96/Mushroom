/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-07 20:10:30
**/

#ifndef _BTREE_HPP_
#define _BTREE_HPP_

#include <cstdint>
#include <string>

#include "status.hpp"
#include "slice.hpp"
#include "btree_page.hpp"
#include "data_page.hpp"

namespace Mushroom {

class BTree
{
	public:
		static const int MAX_KEY_LENGTH = 256;

		BTree():pager_(nullptr), root_(nullptr) { }

		Status Init(const int fd, const int key_len);

		Status Close();

		uint8_t KeyLen() const { return key_len_; }

		Status Put(const KeySlice *key);
		Status Delete(const KeySlice *key);
		bool Get(KeySlice *key) const;

		BTreePage* First(page_id *page_no, int level) const;
		bool Next(KeySlice *key, page_id *page_no, uint16_t *index) const;

		void Traverse(int level) const;

		std::string ToString() const;

		BTree& operator=(const BTree &) = delete;
		BTree(const BTree &) = delete;

		~BTree() {
			if (pager_) delete pager_;
			pager_ = nullptr;
			if (root_) delete [] root_;
			root_ = nullptr;
		}

	private:
		BTreePage* DescendToLeaf(const KeySlice *key, BTreePage **stack, uint8_t *depth) const;
		Status Split(BTreePage *leaf, BTreePage **stack, uint8_t depth);
		Status SplitRoot();

		BTreePager *pager_;

		BTreePage  *root_;

		uint16_t 	  degree_;

		uint8_t     key_len_;

		int num = 0;
};

} // namespace Mushroom

#endif /* _BTREE_HPP_ */