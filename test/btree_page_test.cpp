/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-08 20:29:20
**/

#include <iostream>

#include "../src/bt_page.hpp"

int main(int argc, char **argv)
{
	using namespace Mushroom;
	BTreePage *leaf;
	std::cout << sizeof(BTreePage) << std::endl;
	char a[16] = {0, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 6, 0, 7, 0};
	leaf = static_cast<BTreePage *>(static_cast<void *>(a));
	std::cout << leaf;
	return 0;
}
