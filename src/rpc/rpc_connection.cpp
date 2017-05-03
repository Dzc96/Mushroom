/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-02 21:22:15
**/

#include "rpc_connection.hpp"

namespace Mushroom {

RpcConnection::RpcConnection(const EndPoint &server)
:Connection(server), marshaller_(input_, output_) { }

RpcConnection::~RpcConnection() { }

} // namespace Mushroom
