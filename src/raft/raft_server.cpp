/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-22 21:21:08
**/

#include <cassert>
#include <ctime>
#include <random>
#include <unistd.h>

#include "raft_server.hpp"
#include "../include/thread.hpp"
#include "../include/bounded_queue.hpp"
#include "log.hpp"
#include "arg.hpp"

namespace Mushroom {

uint32_t RaftServer::ElectionTimeoutBase  = 150;
uint32_t RaftServer::ElectionTimeoutLimit = 300;
uint32_t RaftServer::HeartbeatInterval    = 15;

class TimeUtil
{
	public:
		static int64_t Now() const {
			struct timeval tv;
			gettimeofday(&tv, 0);
			return int64_t(tv.tv_sec) * 1000000 + tv.tv_usec;
		}

		static SleepFor(int ms) const {
			usleep(ms * 1000);
		}

		static int GetTimeOut() {
			return distribution(engine);
		}

	private:
		static std::default_random_engine engine;
		static std::uniform_int_distribution<int> distribution;
};

std::default_random_engine TimeUtil::engine(time(0));
std::uniform_int_distribution<int> TimeUtil::distribution(RaftServer::ElectionTimeoutBase,
	RaftServer::ElectionTimeoutLimit);

RaftServer::RaftServer(int32_t id, const std::vector<RpcConnection *> &peers,
	Queue<Task> *queue)
:id_(id), state_(Follower), running_(true), in_election_(false), election_time_out_(false),
reset_timer_(false), term_(0), vote_for_(-1), commit_(-1), applied_(-1), peers_(peers),
queue_(queue)
{
	next_ .resize(peers_.size());
	match_.resize(peers_.size());

	thread_ = new Thread([this]() { this->Run(); });
	thread_->Start();
}

void RaftServer::Close()
{
	mutex_.Lock();
	if (!running_) {
		mutex_.Unlock();
		return ;
	}

	running_ = false;
	mutex_.Unlock();

	cond_.Signal();
	thread_->Stop();

	for (auto e : peers_)
		e->Close();
}

RaftServer::~RaftServer()
{
	Close();
	delete thread_;
}

void RaftServer::Vote(const RequestVoteArgs *args, RequestVoteReply *reply)
{
	mutex_.Lock();
	*reply = RequestVoteReply(term_, false);
	if (args->term_ < term_)
		goto end;
	if (vote_for_ != -1 && vote_for_ != args->id_)
		goto end;
	if (args->last_index_ < commit_)
		goto end;
	if (commit_ >= 0 && args->last_term_ < logs_[commit_].term_)
		goto end;

	reply->granted_ = true;

	state_    = Follower;
	vote_for_ = args->id_;

	reset_timer_ = true;
	cond_.Signal();

end:
	mutex_.Unlock();
}

void RaftServer::AppendEntry(const AppendEntryArgs *args, AppendEntryReply *reply)
{
	mutex_.Lock();
	*reply = AppendEntryReply(term_, -1);
	if (args->term_ < term_)
		goto end;
	if (args->prev_index_ >= int32_t(logs_.size()))
		goto end;
	if (args->prev_index_ >= 0 && logs_[args->prev_index_].term_ != args->prev_term_)
		goto end;

	reply->success_ = 0;
	if (args->entries_.size()) {
		int32_t start = 0, prev_index = args->prev_index_;
		if (prev_index >= 0) {
			// find index in entries that does not overlap with current raft server
			for (int32_t i = int32_t(args->entries_.size()) - 1; i >= 0; --i) {
				if (logs_[prev_index].number_ == args->entries_[i].number_) {
					start = i + 1;
					break;
				}
			}
			// erase any entry that has same log but different term
			for (int32_t i = start - 1; i >= 0 && prev_index >= 0; --i) {
				if (logs_[prev_index].term_ != args->entries_[i].term_)
					--prev_index;
			}
		}
		logs_.erase(logs_.begin() + prev_index + 1, logs_.end());
		// prev_index should be exactly logs_.size()-1
		if ((args->entries_.begin() + start) < args->entries_.end()) {
			logs_.insert(logs_.end(), args->entries_.begin() + start, args->entries_.end());
			reply->success_ = args->entries_.end() - (args->entries_.begin() + start);
		}
	}

	if (args->leader_commit_ > commit_)
		commit_ = std::min(args->leader_commit_, int32_t(logs_.size()) - 1);
	reset_timer_ = true;
	cond_.Signal();

end:
	mutex_.Unlock();
}

ElectionStatus RaftServer::Election(const RequestVoteArgs *args)
{
	int time_out = TimeUtil::GetTimeOut();
	int64_t before = TimeUtil::Now();
	uint32_t size = peers.size();
	RequestVoteReply replys[size];
	Future *futures[size];
	for (uint32_t i = 0; i < size; ++i) {
		queue_->Push([this, i, futures, args, &replys]() {
			peers_[i]->Call("RaftServer::Vote", args, &replys[i]);
		});
	}
	int left = time_out - int((TimeUtil::Now() - before) / 1000);
	if (left <= 0) return TimeOut;

	TimeUtil::SleepFor(left);
	uint32_t vote = 1;
	for (uint32_t i = 0; i < size; ++i) {
		if (futures[i]->ok()) {
			if (replys[i].granted_)
				++vote;
			else if (term_ < replys[i].term_)
				return Fail;
		}
	}
	if (vote > ((size + 1) / 2))
		return Success;
	return TimeOut;
}

bool RaftServer::SendAppendEntry()
{
	uint32_t size = peers_.size();
	Future *futures[size];
	AppendEntryArgs args[size];
	AppendEntryReply replys[size];
	mutex_.Lock();
	for (size_t i = 0; i < peers_.size(); ++i) {
		int32_t prev = next_[i] - 1;
		args[i] = AppendEntryArgs(term_, id_, prev, prev >= 0 ? logs_[prev].term_ : 0, commit_);
		if (prev >= 0) {
			args[i].entries_.reserve(logs_.size() - prev);
			for (uint32_t j = prev; j < logs_.size(); ++j)
				args[i].entries_.push_back(logs_[j]);
		}
		queue_->Push([this, i, &args, &replys]() {
			peers_[i].Call("RaftServer::AppendEntry", &args[i], &replys[i]);
		});
	}
	mutex_.Unlock();
	if (state_.get())
	for (uint32_t i = 0; i < future_size; ++i)
		futures[i]->Abandon();
}

void RaftServer::Background()
{
	for (;;) {
		mutex_.Lock();
	sleep:
		while (running_ && !reset_timer_)
			cond_.TimedWait(mutex_, TimeUtil::GetTimeOut());
		if (!running_) {
			mutex_.Unlock();
			return ;
		}
		if (reset_timer_) {
			reset_timer_ = false;
			mutex_.Unlock();
			continue;
		}
		for (;;) {
			state_ = Candidate;
			++term_;
			vote_for_ = id_;
			RequestVoteArgs args(term_, id_, commit_, commit_ >= 0 ? logs_[commit_].term_ : 0);
			mutex_.Unlock();
			ElectionStatus status = Election(&args);
			if (status == TimeOut) {
				mutex_.Lock();
			} else {
				if (status == Success) {
					while (SendAppendEntry())
						TimeUtil::SleepFor(HeartbeatInterval);
				}
				mutex_.Lock();
				state_    = Follower;
				vote_for_ = -1;
				goto sleep;
			}
		}
	}
}

} // namespace Mushroom
