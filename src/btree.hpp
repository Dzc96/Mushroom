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

#include <fstream>
#include <string>
#include <mutex>
#include <map>

#include "status.hpp"
#include "slice.hpp"
#include "btree_pager.hpp"
#include "latch_manager.hpp"

namespace Mushroom {

class BTree
{
	public:
		static const int MAX_KEY_LENGTH = 256;

		BTree(const int fd, const int key_len);

		Status Close();

		uint8_t KeyLen() const { return key_len_; }

		Status Put(const KeySlice *key);

		Status Delete(const KeySlice *key);

		Status Get(KeySlice *key) const;

		BTreePage* First(page_id *page_no, int level) const;

		bool Next(KeySlice *key, page_id *page_no, uint16_t *index) const;

		void Traverse(int level) const;

		bool KeyCheck(std::ifstream &in, int total) const;

		std::string ToString() const;

		BTree& operator=(const BTree &) = delete;
		BTree(const BTree &) = delete;

		~BTree() {
			delete [] root_;
			delete btree_pager_;
			delete latch_manager_;
		}

	private:

		std::pair<BTreePage*, Latch*> DescendToLeaf(const KeySlice *key, page_id *stack,
			uint8_t *depth) const;
		Status Split(BTreePage *leaf, Latch *latch, page_id *stack, uint8_t depth);
		Status SplitRoot();
		Status InsertKey(BTreePage *page, Latch *latch, const KeySlice *key);

		LatchManager *latch_manager_;

		BTreePager  *btree_pager_;

		BTreePage   *root_;

		uint16_t 	  degree_;
		uint8_t     key_len_;
};

} // namespace Mushroom

#endif /* _BTREE_HPP_ */