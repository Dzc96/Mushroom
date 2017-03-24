/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-21 16:50:18
**/

#ifndef _LATCH_MANAGER_HPP_
#define _LATCH_MANAGER_HPP_

#include "latch.hpp"

namespace Mushroom {

class LatchManager
{
	public:
		LatchManager();

		Latch* GetLatch(page_id page_no);

	private:
		void Link(uint16_t hashidx, uint16_t victim, page_id page_no);

		static const uint16_t total = 208;
		static const uint16_t mask  = 209;

		uint16_t  deployed_;
		uint16_t  victim_;
		HashEntry entries_[mask-1];
		Latch     latches_[total];
};

} // namespace Mushroom

#endif /* _LATCH_MANAGER_HPP_ */