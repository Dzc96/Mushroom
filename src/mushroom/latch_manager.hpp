/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-21 16:50:18
**/

#ifndef _LATCH_MANAGER_HPP_
#define _LATCH_MANAGER_HPP_

#include "../include/utility.hpp"
#include "../include/atomic.hpp"

namespace Mushroom {

class Latch;
class HashEntry;

class LatchManager : private NoCopy
{
	public:
		LatchManager();

		~LatchManager();

		Latch* GetLatch(page_t page_no);

	private:
		void Link(uint16_t hashidx, uint16_t victim, page_t page_no);

		static const uint16_t total = 256;
		static const uint16_t mask  = total - 1;

		atomic_16_t deployed_;
		atomic_16_t victim_;

		HashEntry *entries_;
		Latch     *latches_;
};

} // namespace Mushroom

#endif /* _LATCH_MANAGER_HPP_ */