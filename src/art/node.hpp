/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-21 09:42:33
**/

#ifndef _NODE_HPP_
#define _NODE_HPP_

#include <emmintrin.h>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <new>

namespace Mushroom {

enum NodeType { NODE4 = 0x0, NODE16, NODE48, NODE256, LEAF };

class Node
{
	public:
		Node(NodeType type):type_(type), count_(0), len_(0) { }

		inline NodeType Type() const { return (NodeType)type_; }

		inline uint8_t Count() const { return count_; }

		inline void SetPrefix(const uint8_t *prefix, uint8_t len) {
			memcpy(prefix_, prefix, std::min(len_, len));
		}

		inline uint8_t PrefixLen() const { return len_; }

		Node(const Node &) = delete;
		Node& operator=(const Node &) = delete;

		static const uint32_t MAX_PRE_LEN = 8;

	protected:
		inline void IncrCount() { ++count_; }

	private:
		uint8_t type_;
		uint8_t count_;
		uint8_t len_;
		uint8_t prefix_[MAX_PRE_LEN];
};

class Node4 : public Node
{
	public:
		Node4():Node(NODE4) { memset(key_, 0, 4); }

		Node** Descend(uint8_t byte) {
			for (int i = 0; i < Count(); ++i)
				if (key_[i] == byte)
					return &child_[i];
			return 0;
		}

		void Add(uint8_t byte, void *child) {
			int idx;
			for (idx = 0; idx < Count(); ++idx)
				if (key_[idx] > byte)
					break;
			memmove(key_ + idx + 1, key_ + idx, Count() - idx);
			memmove(child_ + idx + 1, child_ + idx, sizeof(Node *) * (Count() - idx));
			key_[idx] = byte;
			child_[idx] = (Node *)child;
			IncrCount();
		}

	private:
		uint8_t key_[4];
		Node   *child_[4];
};

class Node16 : public Node
{
	public:
		Node16():Node(NODE16) { memset(key_, 0, 16); }

		Node** Descend(uint8_t byte) {
			__m128i cmp = _mm_cmpeq_epi8(_mm_set1_epi8(byte), _mm_loadu_si128((__m128i *)key_));
			int bit = _mm_movemask_epi8(cmp) & ((1 << Count()) - 1);
			if (bit) return &child_[__builtin_ctz(bit)];
			else return 0;
		}

		void Add(uint8_t byte, void *child) {
			__m128i cmp = _mm_cmplt_epi8(_mm_set1_epi8(byte), _mm_loadu_si128((__m128i *)key_));
			int bit = _mm_movemask_epi8(cmp) & ((1 << Count()) - 1);
			int idx;
			if (bit) {
				idx = __builtin_ctz(bit);
				memmove(key_ + idx + 1, key_ + idx, Count() - idx);
				memmove(child_ + idx + 1, child_ + idx, sizeof(Node *) * (Count() - idx));
			} else {
				idx = Count();
			}
			key_[idx] = byte;
			child_[idx] = (Node *)child;
			IncrCount();
		}

	private:
		uint8_t key_[16];
		Node   *child_[16];
};

class Node48 : public Node
{
	public:
		Node48():Node(NODE48) { memset(index_, 0, 256); memset(child_, 0, sizeof(Node *) * 48); }

		Node** Descend(uint8_t byte) {
			uint8_t idx = index_[byte];
			if (idx) return &child_[idx];
			else return 0;
		}

		void Add(uint8_t byte, void *child) {
			int idx = 0;
			while (child_[idx]) ++idx;
			index_[byte] = idx;
			child_[idx] = (Node *)child;
			IncrCount();
		}

	private:
		uint8_t index_[256];
		Node   *child_[48];
};

class Node256 : public Node
{
	public:
		Node256():Node(NODE256) { memset(child_, 0, sizeof(Node *) * 256); }

		Node** Descend(uint8_t byte) {
			return &child_[byte];
		}

		void Add(uint8_t byte, void *child) {
			child_[byte] = (Node *)child;
			IncrCount();
		}

	private:
		Node *child_[256];
};

class Leaf
{
	public:
		Leaf(const uint8_t *key, uint32_t len, uint32_t val):val_(val), len_(len) {
			memcpy(key_, key, len_);
		}

		bool Match(const uint8_t *key, uint32_t len) const {
			if (len != len_) return false;
			return !memcmp(key_, key, len_);
		}

		int PrefixFrom(uint32_t depth, const Leaf *that) const {
			int max_cmp = std::min(this->len_, that->len_) - depth;
			int idx;
			for (idx = 0; idx < max_cmp; ++idx)
				if (this->key_[depth + idx] != that->key_[depth + idx])
					return idx;
			return idx;
		}

		uint8_t KeyAt(uint32_t idx) const {
			assert(idx < len_);
			return key_[idx];
		}

	private:
		uint32_t val_;
		uint32_t len_;
		uint8_t  key_[0];
};

inline Leaf* NewLeaf(const uint8_t *key, uint32_t len, uint32_t val) {
	char *tmp = new char[8 + len];
	Leaf *leaf = (Leaf *)tmp;
	return new (leaf) Leaf(key, len, val);
}

inline void DeleteLeaf(Leaf *leaf) {
	delete [] (char *)leaf;
}

} // namespace Mushroom

#endif /* _NODE_HPP_ */