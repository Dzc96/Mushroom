/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-17 14:48:06
**/

#ifndef NOLSM

#ifndef _LEVEL_TREE_HPP_
#define _LEVEL_TREE_HPP_

#include <vector>

#include "utility.hpp"

namespace Mushroom {

class LevelTree
{
	public:
		LevelTree(uint32_t key_len);

		void AppendLevel0SSTable(const BLinkTree *b_link_tree);

		void MergeLevel(uint32_t level);

	private:
		void AppendNewLevel(uint32_t level);

		void FindOverlapInLevel(uint32_t level, std::vector<SSTable *> &sstabels,
			const Key &smallest, const Key &largest);

		static const uint32_t MaxLevel0SSTable = 4;

		uint32_t            key_len_;
		std::vector<Level>  levels_;
		SSTableManager     *sstable_manager_;
		Merger             *merger_;
};

} // namespace Mushroom

#endif /* _LEVEL_TREE_HPP_ */

#endif /* NOLSM */