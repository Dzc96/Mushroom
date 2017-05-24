/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-04-23 10:50:45
**/

#include "../log/log.hpp"
#include "eventbase.hpp"
#include "server.hpp"
#include "channel.hpp"
#include "connection.hpp"

namespace Mushroom {

Server::Server(EventBase *event_base, uint16_t port)
:port_(port), event_base_(event_base), listen_(0), connectcb_(0) { }

Server::~Server()
{
	if (listen_)
		delete listen_;

	socket_.Close();

	for (auto e : connections_)
		delete e;
}

void Server::Close()
{
	delete listen_;
	listen_ = 0;
	socket_.Close();
}

void Server::Start()
{
	FatalIf(!socket_.Create(), "socket create failed :(", strerror(errno));
	FatalIf(!socket_.SetResuseAddress(), "socket option set failed :(", strerror(errno));
	FatalIf(!socket_.Bind(port_), "socket bind port %u failed, %s :(", port_, strerror(errno));
	FatalIf(!socket_.Listen(), "socket listen failed, %s :(", strerror(errno));
	listen_ = new Channel(socket_.fd(), event_base_->GetPoller(), [this]() { HandleAccept(); }, 0);
}

void Server::OnConnect(const ConnectCallBack &connectcb)
{
	connectcb_ = connectcb;
}

void Server::HandleAccept()
{
	int fd = socket_.Accept();
	if (fd < 0) {
		Error("socket accept failed, %s :(", strerror(errno));
		return ;
	}
	Connection *con = new Connection(Socket(fd), event_base_->GetPoller());
	if (connectcb_)
		connectcb_(con);
	connections_.push_back(con);
}

} // namespaceMushroom
