#ifndef MUDUO_PROTORPC_ZURG_PROCESS_H
#define MUDUO_PROTORPC_ZURG_PROCESS_H

#include <examples/zurg/proto/slave.pb.h>

#include <muduo/net/Buffer.h>
#include <muduo/net/TimerId.h>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace muduo
{
namespace net
{
class Channel;
class EventLoop;
}
}

struct rusage;

namespace zurg
{

class Pipe;
class Sink;

class Process : public boost::enable_shared_from_this<Process>,
                boost::noncopyable
{
 public:
  Process(muduo::net::EventLoop* loop,
          const RunCommandRequestPtr& request,
          const muduo::net::RpcDoneCallback& done);

  ~Process();

  int start(); // may throw

  pid_t pid() const
  {
    return pid_;
  }

  void setTimerId(const muduo::net::TimerId& timerId)
  {
    timerId_ = timerId;
  }

  void onExit(int status, const struct rusage&);
  void onTimeout();

  static void onTimeoutWeak(const boost::weak_ptr<Process>& wkPtr);

 private:
  void execChild(Pipe& execError, Pipe& stdOutput, Pipe& stdError)
       __attribute__ ((__noreturn__));;
  int afterFork(Pipe& execError, Pipe& stdOutput, Pipe& stdError);

  muduo::net::EventLoop* loop_;
  RunCommandRequestPtr request_;
  muduo::net::RpcDoneCallback doneCallback_;
  pid_t pid_;
  muduo::Timestamp startTime_;
  int64_t startTimeInJiffies_;
  muduo::net::TimerId timerId_;
  boost::shared_ptr<Sink> stdoutSink_;
  boost::shared_ptr<Sink> stderrSink_;
};
typedef boost::shared_ptr<Process> ProcessPtr;

}
#endif  // MUDUO_PROTORPC_ZURG_PROCESS_H
