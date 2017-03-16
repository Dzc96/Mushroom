/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:   2017-03-16 17:04:27
**/

#include <iostream>
#include <chrono>
#include <iomanip>
#include <string>
#include <vector>

#include "../src/db.hpp"
#include "../src/iterator.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;

	const int total = (argc == 2) ? atoi(argv[1]) : 1;
	const int key_len = 16;
	const std::vector<std::string> files = {
		std::string("../data/16_2500000_0_random"),
		std::string("../data/16_2500000_1_random"),
		std::string("../data/16_2500000_2_random"),
		std::string("../data/16_2500000_3_random")
	};

	MushroomDB db("mushroom", key_len);

	auto beg = std::chrono::high_resolution_clock::now();
	db.IndexMultiple(files, total);
	auto end  = std::chrono::high_resolution_clock::now();
	auto Time = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	std::cerr << "\ntime: " << std::setw(8) << Time << "  s\n";

	if (db.FindMultiple(files, total) == Fail) {
		std::cout << "\033[31mError :(\033[0m\n";
	} else {
		Iterator it(db.Btree());
		assert(it.CheckBtree());
		std::cout << "\033[32mSuccess :)\033[0m\n";
	}

	db.Close();
	return 0;
}
