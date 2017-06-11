/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2016-10-07 20:10:30
**/

#ifndef _B_LINK_TREE_HPP_
#define _B_LINK_TREE_HPP_

#include "../include/utility.hpp"
#include "../include/atomic.hpp"

namespace Mushroom {

class KeySlice;
class Page;
class PoolManager;
class Latch;
class LatchManager;

enum LockType { WriteLock, ReadLock };

class BLinkTree : private NoCopy
{
	public:
		static const uint32_t MAX_KEY_LENGTH = 255;

		BLinkTree(uint32_t key_len);

		~BLinkTree();

		bool Put(KeySlice *key);

		bool Get(KeySlice *key);

		void Free();

	private:
		struct Set {
			Set():depth_(0) { }
			page_t   page_no_;
			Latch   *latch_;
			Page    *page_;
			page_t   stack_[8];
			uint32_t depth_;
		};

		void GetParent(Set &set);

		void GetNext(Set &set);

		void DescendToLeaf(const KeySlice *key, Set &set, LockType type);

		bool SplitAndPromote(Set &set, KeySlice *key);

		Page* Split(Set &set, KeySlice *key);

		void Insert(Set &set, KeySlice *key);

		bool Update(Set &set, KeySlice *old_key, KeySlice *new_key);

		void LoadLeaf(const KeySlice *key, Set &set);

		void Audit();

		void ShowPage(page_t page_no);

		LatchManager *latch_manager_;
		PoolManager  *pool_manager_;

		Atomic<page_t> root_;

		uint8_t      key_len_;
		uint16_t     degree_;
};

} // namespace Mushroom

#endif /* _B_LINK_TREE_HPP_ */