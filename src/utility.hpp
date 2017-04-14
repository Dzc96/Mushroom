/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-15 10:37:54
**/

#ifndef _UTILITY_HPP_
#define _UTILITY_HPP_

#include <cstdint>

namespace Mushroom {

typedef uint32_t valptr;
typedef uint32_t page_t;
typedef uint32_t table_t;

class KeySlice;
class SpinLatch;
class Latch;
class HashEntry;
class Page;
class BLinkTree;
class LSMTree;
class LatchManager;
class PagePool;
class PoolManager;
class MushroomDB;
class Task;
class ThreadPool;
class Block;
class BlockManager;
class SSTable;
class SSTableManager;
class Merger;

} // namespace Mushroom

#endif /* _UTILITY_HPP_ */