/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-10 15:34:43
**/

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <fstream>
#include <cassert>
#include <iostream>

#include "db.hpp"
#include "page_manager.hpp"
#include "latch_manager.hpp"

namespace Mushroom {

MushroomDB::MushroomDB(const char *name, const int key_len)
{
	// if (!access(name, F_OK)) assert(!remove(name));

	assert((sizeof(LatchManager) + 2 * sizeof(page_id)) ==
		BTreePage::PageSize * PageManager::LatchPages);

	assert((fd_ = open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) > 0);

	struct flock lock;
	memset(&lock, 0, sizeof(lock));
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_len = BTreePage::PageSize * PageManager::LatchPages;

	assert(fcntl(fd_, F_SETLKW, &lock) != -1);
	if (!lseek(fd_, 0, SEEK_END)) {
		page_id page_no[2] = {0, 0};
		assert(write(fd_, (void *)page_no, 2 * sizeof(page_id)) == 2 * sizeof(page_id));
		LatchManager tmp;
		assert(write(fd_, (void *)&tmp, sizeof(LatchManager)) == sizeof(LatchManager));
	}

	mapped_ = (char *)mmap(0, BTreePage::PageSize * PageManager::LatchPages,
		PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
	assert(mapped_ != MAP_FAILED);

	LatchManager *latch_manager = (LatchManager *)(mapped_ + 2*sizeof(page_id));
	PageManager *page_manager = new PageManager(fd_, (page_id *)mapped_+1);

	btree_ = new BTree(key_len, latch_manager, page_manager, (page_id *)mapped_);

	if (!*((page_id *)mapped_+1)) btree_->Initialize();

	lock.l_type = F_UNLCK;
	assert(fcntl(fd_, F_SETLKW, &lock) != -1);
}

bool MushroomDB::Put(KeySlice *key)
{
	return btree_->Put(key);
}

bool MushroomDB::Get(KeySlice *key)
{
	return btree_->Get(key);
}

bool MushroomDB::FindSingle(const char *file, const int total)
{
	std::ifstream in(file);
	assert(in.is_open());
	bool flag = btree_->KeyCheck(in, total);
	in.close();
	return flag;
}

bool MushroomDB::Close()
{
	btree_->Free();
	assert(!munmap(mapped_, BTreePage::PageSize * PageManager::LatchPages));
	close(fd_);
	return true;
}

} // namespace Mushroom
