/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-11-20 12:37:41
**/

#include <cassert>
#include <chrono>
#include <fcntl.h>
#include <unistd.h>

#include "../src/slice.hpp"
#include "../src/db.hpp"

#ifndef NOLATCH
#include "../src/thread_pool.hpp"
#endif

using namespace Mushroom;

static const int key_len = 16;
static int total;

double Do(const char *file, MushroomDB *db, bool (MushroomDB::*(fun))(KeySlice *))
{
	#ifndef NOLATCH
	ThreadPool *pool = new ThreadPool(new Queue(1024, key_len));
	#else
	printf("NOLATCH defined, using single thread ;)\n");
	#endif

	char tmp[sizeof(page_id) + key_len] = {0};
	KeySlice *key = (KeySlice *)tmp;
	int fd = open(file, O_RDONLY);
	assert(fd > 0);
	char buf[8192];
	int curr = 0, ptr = 0, count = 0;
	bool flag = true;

	auto beg = std::chrono::high_resolution_clock::now();
	for (; (ptr = pread(fd, buf, 8192, curr)) > 0 && flag; curr += ptr) {
		while (--ptr && buf[ptr] != '\n' && buf[ptr] != '\0') buf[ptr] = '\0';
		if (ptr) buf[ptr++] = '\0';
		else break;
		for (int i = 0; i < ptr;) {
			int j = 0;
			char *tmp = buf + i;
			for (; buf[i] != '\n' && buf[i] != '\0'; ++i, ++j) ;
			tmp[j] = '\0';
			memcpy(key->Data(), tmp, key_len);
			#ifndef NOLATCH
			pool->AddTask(fun, db, key);
			#else
			(db->*fun)(key);
			#endif
			if (++count == total) {
				flag = false;
				break;
			}
			++i;
		}
	}
	close(fd);
	#ifndef NOLATCH
	pool->Clear();
	#endif
	auto end = std::chrono::high_resolution_clock::now();

	#ifndef NOLATCH
	delete pool;
	#endif
	auto t = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	return t;
}

int main(int argc, char **argv)
{
	const char *file = "../data/10000000";

	assert(argc > 4);
	uint32_t page_size = atoi(argv[1]) ? atoi(argv[1]) : 4096;
	uint32_t pool_size = atoi(argv[2]) ? atoi(argv[2]) : 4800;
	uint32_t hash_bits = atoi(argv[3]) ? atoi(argv[3]) : 1024;
	uint32_t seg_bits  = atoi(argv[4]) ? atoi(argv[4]) : 4;

	total = (argc == 6) ? atoi(argv[5]) : 1;

	MushroomDB db(key_len, page_size, pool_size, hash_bits, seg_bits);
	printf("\n");

	double t1 = Do(file, &db, &MushroomDB::Put);
	printf("total: %d\nput time: %f  s\n", total, t1);

	// double t2 = Do(file, &db, &MushroomDB::Get);
	// printf("get time: %f  s\n", t2);

	db.Close();

	return 0;
}
