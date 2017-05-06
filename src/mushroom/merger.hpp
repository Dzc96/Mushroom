/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-14 11:37:32
**/

#ifndef _MERGER_HPP_
#define _MERGER_HPP_

#include "utility.hpp"

#include <vector>

namespace Mushroom {

class Key;
class SSTable;
class BlockManager;
class SSTableManager;

class Merger
{
	public:
		Merger();

		~Merger();

		void AppendMergePointer(uint32_t key_len);

		void Merge(const std::vector<SSTable *> &tables, SSTableManager *sstable_manager,
			BlockManager *block_manager, uint32_t level, uint32_t key_len,
			std::vector<SSTable *> *result);

		Key& GetOffsetInLevel(uint32_t level);

	private:
		uint32_t           time_;
		std::vector<Key *> merge_ptrs_;
};

} // namespace Mushroom

#endif /* _MERGER_HPP_ */