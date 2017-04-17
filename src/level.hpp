/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-17 14:54:51
**/

#ifndef NOLSM

#ifndef _LEVEL_HPP_
#define _LEVEL_HPP_

#include <vector>

#include "utility.hpp"

namespace Mushroom {

class Level
{
	public:
		friend class LevelTree;

		Level(uint32_t level);

		uint32_t SSTableNumber() const;

		void FindOverlapSSTable(std::vector<SSTable *> *tables, const Key &smallest,
			const Key &largest) const;

		SSTable* NextSSTable(const Key &offset, uint32_t *index) const;

	private:
		uint32_t               level_;
		std::vector<SSTable *> sstables_;
};

} // namespace Mushroom

#endif /* _LEVEL_HPP_ */

#endif /* NOLSM */