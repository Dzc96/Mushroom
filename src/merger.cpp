/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-14 11:37:39
**/

#ifndef NOLSM

#include <queue>
#include <vector>

#include "merger.hpp"
#include "sstable.hpp"
#include "sstable_manager.hpp"

namespace Mushroom {

Merger::Merger():time_(0) { }

Merger::~Merger()
{
	printf("\033[36mmerge time: %u\033[0m\n", time_);
}

void Merger::AppendMergePointer(uint32_t key_len)
{
	merge_ptrs_.push_back(new Key(key_len));
}

const Key& Merger::GetOffsetInLevel(uint32_t level)
{
	assert(level);
	return *merge_ptrs_[level-1];
}

struct Tuple
{
	Tuple():iter(0), idx(0) { }
	Tuple(SSTable::Iterator *iter, uint32_t idx):iter(iter), idx(idx) { }

	SSTable::Iterator *iter;
	uint32_t  idx;
};

static auto greater = [](const Tuple &lhs, const Tuple &rhs) {
	return lhs.iter->key() > rhs.iter->key();
};

void Merger::Merge(const std::vector<SSTable *> &tables, SSTableManager *sstable_manager,
	BlockManager *block_manager, uint32_t level, uint32_t key_len, std::vector<SSTable *> *result)
{
	uint32_t size = tables.size();
	SSTable::Iterator *iters[size];
	std::vector<Tuple> tuples(size);
	for (uint32_t i = 0; i != size; ++i) {
		iters[i] = new SSTable::Iterator(tables[i], key_len);
		tuples[i] = Tuple(iters[i], i);
	}

	std::priority_queue<Tuple, std::vector<Tuple>, decltype(greater)> queue(greater, tuples);

	SSTable *sstable = sstable_manager->NewSSTable(level);

	for (; !queue.empty();) {
		Tuple tuple = queue.top();
		if (!sstable->Append(tuple.iter->key(), block_manager)) {
			sstable->AppendFinalBlockRange(key_len);
			result->push_back(sstable);
			sstable = sstable_manager->NewSSTable(level);
			assert(sstable->Append(tuple.iter->key(), block_manager));
		}
		queue.pop();
		if (iters[tuple.idx]->Next())
			queue.push(tuple);
	}
	sstable->AppendFinalBlockRange(key_len);
	result->push_back(sstable);

	SSTable::Iterator *it = new SSTable::Iterator(sstable, key_len);
	it->Last();
	*merge_ptrs_[level] = it->key();

	for (size_t i = 0; i != size; ++i)
		delete iters[i];

	delete it;
	++time_;
}

} // namespace Mushroom

#endif /* NOLSM */