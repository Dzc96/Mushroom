/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-10 15:32:20
**/

#ifndef _MUSHROOM_DB_HPP_
#define _MUSHROOM_DB_HPP_

#include "utility.hpp"

namespace Mushroom {

class KeySlice;
class BLinkTree;
class LSMTree;

class MushroomDB
{
	public:
		MushroomDB(uint32_t key_len, uint32_t page_size, uint32_t pool_size,
			uint32_t hash_bits, uint32_t seg_bits);

		bool Put(KeySlice *key);

		bool Get(KeySlice *key);

		bool Close();

		MushroomDB(const MushroomDB &) = delete;
		MushroomDB& operator=(const MushroomDB &) = delete;

		~MushroomDB();

	private:
		BLinkTree *tree_;
};

} // namespace Mushroom

#endif /* _MUSHROOM_DB_HPP_ */