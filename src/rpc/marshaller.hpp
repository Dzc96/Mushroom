/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-30 09:17:51
**/

#ifndef _MARSHALLER_HPP_
#define _MARSHALLER_HPP_

#include <vector>

#include "utility.hpp"
#include "../network/buffer.hpp"

namespace Mushroom {

class Marshaller
{
	public:
		Marshaller(Buffer &input, Buffer &output);

		~Marshaller();

		template<typename T>
		inline uint32_t Marshal(rpc_t id, const T *args);

		template<typename T>
		uint32_t Unmarshal(rpc_t *id, T *reply);

		void Read(const void *str, uint32_t len);

		void Write(void *str, uint32_t len);

	private:
		Buffer &input_;
		Buffer &output_;
};

inline Marshaller& operator<<(Marshaller &marshaller, const int32_t &v) {
	marshaller.Read(&v, 4);
	return marshaller;
}

inline Marshaller& operator<<(Marshaller &marshaller, const uint32_t &v) {
	marshaller.Read(&v, 4);
	return marshaller;
}

template<typename T>
inline Marshaller& operator<<(Marshaller &marshaller, const std::vector<T> &v) {
	uint32_t e = v.size();
	marshaller << e;
	for (uint32_t i = 0; i < e; ++i)
		marshaller << v[i];
	return marshaller;
}

inline Marshaller& operator>>(Marshaller &marshaller, int32_t &v) {
	marshaller.Write(&v, 4);
	return marshaller;
}

inline Marshaller& operator>>(Marshaller &marshaller, uint32_t &v) {
	marshaller.Write(&v, 4);
	return marshaller;
}

template<typename T>
inline Marshaller& operator>>(Marshaller &marshaller, std::vector<T> &v) {
	uint32_t e;
	marshaller >> e;
	v.reserve(e);
	for (uint32_t i = 0; i < e; ++i) {
		T t;
		marshaller >> t;
		v.push_back(t);
	}
	return marshaller;
}

template<typename T>
inline uint32_t Marshaller::Marshal(rpc_t id, const T *args)
{
	output_.Clear();
	uint32_t *len = (uint32_t *)output_.data();
	output_.AdvanceTail(4);
	*this <<  id;
	*this << *args;
	*len = output_.size() - 4;
	return *len;
}

template<typename T>
inline uint32_t Marshaller::Unmarshal(rpc_t *id, T *reply)
{
	uint32_t len;
	*this >> len;
	*this >> *id;
	*this >> *reply;
	return len;
}

} // namespace Mushroom

#endif /* _MARSHALLER_HPP_ */