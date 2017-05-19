/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-25 22:11:08
**/

#include <unistd.h>

#include "../log/log.hpp"
#include "connection.hpp"
#include "channel.hpp"

namespace Mushroom {

Connection::Connection(const EndPoint &server, Poller *poller)
:connected_(false), channel_(0), readcb_(0), writecb_(0)
{
	FatalIf(!socket_.Create(), "socket create failed :(", strerror(errno));
	if (!socket_.Connect(server)) {
		Error("socket connect server %s failed, %s :(", server.ToString().c_str(), strerror(errno));
		return ;
	}
	FatalIf(!socket_.SetNonBlock(), "socket set non-block failed :(", strerror(errno));
	channel_ = new Channel(socket_.fd(), poller,
		[this]() { this->HandleRead(); }, [this]() { this->HandleWrite(); });
	connected_ = true;
}

Connection::Connection(const Socket &socket, Poller *poller)
:socket_(socket), readcb_(0), writecb_(0)
{
	FatalIf(!socket_.SetNonBlock(), "socket set non-block failed :(", strerror(errno));
	channel_ = new Channel(socket_.fd(), poller,
		[this]() { this->HandleRead(); }, [this]() { this->HandleWrite(); });
	connected_ = true;
}

Connection::~Connection()
{
	Close();
}

bool Connection::Close()
{
	if (socket_.Valid()) {
		// Info("closing connection ;)");
		connected_ = false;
		delete channel_;
		channel_ = 0;
		return socket_.Close();
	}
	return true;
}

bool Connection::Success() const
{
	return connected_;
}

Buffer& Connection::GetInput()
{
	return input_;
}

Buffer& Connection::GetOutput()
{
	return output_;
}

void Connection::OnRead(const ReadCallBack &readcb)
{
	readcb_ = readcb;
}

void Connection::OnWrite(const WriteCallBack &writecb)
{
	writecb_ = writecb;
}

void Connection::HandleRead()
{
	if (!connected_) {
		Error("connection has closed :(");
		return ;
	}
	input_.Reset();
	bool blocked = false;
	uint32_t read = socket_.Read(input_.end(), input_.space(), &blocked);
	if (!read && !blocked) {
		Close();
		return ;
	}
	input_.AdvanceTail(read);
	if (readcb_ && read)
		readcb_();
}

void Connection::HandleWrite()
{
	if (!connected_) {
		Error("connection has closed :(");
		return ;
	}
	uint32_t write = socket_.Write(output_.begin(), output_.size());
	if (!write) {
		Close();
		return ;
	}
	output_.AdvanceHead(write);
	if (output_.empty()) {
		if (writecb_)
			writecb_();
		if (channel_->CanWrite())
			channel_->EnableWrite(false);
	}
}

void Connection::Send(const char *str)
{
	Send(str, strlen(str));
}

void Connection::Send(Buffer &buffer)
{
	output_.Read(buffer.begin(), buffer.size());
	buffer.Clear();
	SendOutput();
}

void Connection::Send(const char *str, uint32_t len)
{
	output_.Read(str, len);
	SendOutput();
}

void Connection::SendOutput()
{
	if (!connected_) {
		Error("connection has closed :(");
		return ;
	}
	uint32_t write = socket_.Write(output_.begin(), output_.size());
	if (!write) {
		Close();
		return ;
	}
	output_.AdvanceHead(write);
	if (output_.size()) {
		output_.Adjust();
		assert(0);
		if (!channel_->CanWrite())
			channel_->EnableWrite(true);
	}
}

} // namespace Mushroom
