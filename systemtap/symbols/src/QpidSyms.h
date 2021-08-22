
#ifndef _QPID_SYMS_H_H
#define _QPID_SYMS_H_H
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <queue>
#include <set>
#include "boost/shared_ptr.hpp"
#include "boost/noncopyable.hpp"
#include "boost/function.hpp"
#include "boost/detail/atomic_count.hpp"
#include <boost/intrusive_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/scoped_ptr.hpp>

template <class T>
class _qpid_SharedObject : private boost::noncopyable
{
public:
        typedef boost::shared_ptr<T> shared_ptr;

        virtual ~_qpid_SharedObject() {}
};

class _qpid_sys_IOHandle {
    int fd;
};

class _qpid_sys_TransportAcceptor : public _qpid_SharedObject<_qpid_sys_TransportAcceptor>
{
  public:
    virtual ~_qpid_sys_TransportAcceptor();
};

class _qpid_sys_TransportConnector : public _qpid_SharedObject<_qpid_sys_TransportConnector>
{
public:
    typedef boost::function2<void, int, std::string> ConnectFailedCallback;

    virtual ~_qpid_sys_TransportConnector();
};

struct _qpid_sys_SocketTransportOptions {
    bool tcpNoDelay;
    bool nodict;
    uint32_t maxNegotiateTime;
    ~_qpid_sys_SocketTransportOptions();
};

struct _qpid_broker_Broker_TransportInfo
{
        boost::shared_ptr<_qpid_sys_TransportAcceptor> acceptor;
        boost::shared_ptr<_qpid_sys_TransportConnector> connectorFactory;
        uint16_t port;

        ~_qpid_broker_Broker_TransportInfo();        
};

class _qpid_sys_Socket
{
public:
    virtual ~_qpid_sys_Socket() {};
};
class _qpid_sys_BSDSocket : public _qpid_sys_Socket
{
public:
    virtual ~_qpid_sys_BSDSocket();

protected:
    mutable boost::scoped_ptr<_qpid_sys_IOHandle> handle;
    mutable std::string localname;
    mutable std::string peername;
    mutable int fd;
    mutable int lastErrorCode;
    mutable bool nonblocking;
    mutable bool nodelay;
};

class _qpid_sys_AsynchAcceptor {
public:
    typedef boost::function1<void, const _qpid_sys_Socket&> Callback;
    virtual ~_qpid_sys_AsynchAcceptor() {};
};

class  _qpid_sys_Runnable
{
public:
    typedef boost::function0<void> Functor;
     virtual ~_qpid_sys_Runnable();
};

class _qpid_sys_Mutex : private boost::noncopyable {
    pthread_mutex_t mutex;
};

class _qpid_sys_Condition
{
    pthread_cond_t condition;
};

class _qpid_sys_Monitor : public _qpid_sys_Mutex, public _qpid_sys_Condition {
};




class _qpid_RefCounted : private boost::noncopyable {
    mutable boost::detail::atomic_count count;
public:
    _qpid_RefCounted() : count(0) {}
    void addRef() const { ++count; }
    void release() const { if (--count==0) released(); }
    long refCount() { return count; }

protected:
    virtual ~_qpid_RefCounted() {};
    // Allow subclasses to over-ride behavior when refcount reaches 0.
    virtual void released() const { delete this; }
};

inline void intrusive_ptr_add_ref(const _qpid_RefCounted* p) { p->addRef(); }
inline void intrusive_ptr_release(const _qpid_RefCounted* p) { p->release(); }

typedef int64_t TimePrivate;
class _qpid_sys_AbsTime {
    TimePrivate timepoint;
};

class _qpid_sys_Duration {
    int64_t nanosecs;
};

class _qpid_sys_TimerTask : public _qpid_RefCounted {
public:
    std::string name;
    _qpid_sys_AbsTime sortTime;
    _qpid_sys_Duration period;
    _qpid_sys_AbsTime nextFireTime;
    _qpid_sys_Monitor stateMonitor;
    enum {WAITING, CALLING, CANCELLED} state;

     virtual ~_qpid_sys_TimerTask() {}
};

class _qpid_sys_ThreadPrivate {
    pthread_t thread;
};

class _qpid_sys_Thread
{
    boost::shared_ptr<_qpid_sys_ThreadPrivate> impl;
};


class _qpid_sys_Timer : private _qpid_sys_Runnable {
    _qpid_sys_Monitor monitor;
    std::priority_queue<boost::intrusive_ptr<_qpid_sys_TimerTask> > tasks;
    _qpid_sys_Thread runner;
    bool active;
    
    virtual ~_qpid_sys_Timer() {}
};

class _qpid_sys_PollerHandle;
class _qpid_sys_PollerHandlePrivate {
    
    enum FDStat {
        ABSENT,
        MONITORED,
        INACTIVE,
        HUNGUP,
        MONITORED_HUNGUP,
        INTERRUPTED,
        INTERRUPTED_HUNGUP,
        DELETED
    };

    ::__uint32_t events;
    const _qpid_sys_IOHandle* ioHandle;
    _qpid_sys_PollerHandle* pollerHandle;
    FDStat stat;
    _qpid_sys_Mutex lock;    
};

class _qpid_sys_PollerHandle {
    _qpid_sys_PollerHandlePrivate* const impl;
    virtual ~_qpid_sys_PollerHandle() {}
};


class _qpid_sys_HandleSet
{
    _qpid_sys_Mutex lock;
    std::set<_qpid_sys_PollerHandle*> handles;
};
class _qpid_sys_AtomicCount {
    boost::detail::atomic_count  count;
};

class _qpid_sys_PollerPrivate {
    static const int DefaultFds = 256;
    struct ReadablePipe {
        int fds[2];
    };

    ReadablePipe alwaysReadable;
    int alwaysReadableFd;

    class InterruptHandle: public _qpid_sys_PollerHandle {
        std::queue<_qpid_sys_PollerHandle*> handles;
    };

    const int epollFd;
    bool isShutdown;
    InterruptHandle interruptHandle;
    _qpid_sys_HandleSet registeredHandles;
    _qpid_sys_AtomicCount threadCount;
};

class _qpid_sys_Poller : public _qpid_sys_Runnable {
    _qpid_sys_PollerPrivate* const impl;

public:
    typedef boost::shared_ptr<_qpid_sys_Poller> shared_ptr;

    enum Direction {
        NONE = 0,
        INPUT,
        OUTPUT,
        INOUT
    };

    enum EventType {
        INVALID = 0,
        READABLE,
        WRITABLE,
        READ_WRITABLE,
        DISCONNECTED,
        SHUTDOWN,
        TIMEOUT,
        INTERRUPTED
    };

    struct Event {
        _qpid_sys_PollerHandle* handle;
        EventType type;
        
    };
};

class _qpid_sys_Codec
{
  public:
    virtual ~_qpid_sys_Codec() {}
};

class _qpid_sys_ConnectionCodec : public _qpid_sys_Codec {
  public:
    virtual ~_qpid_sys_ConnectionCodec() {}
    struct Factory {
        virtual ~Factory() {}
    };
};

typedef boost::function3<void, boost::shared_ptr<_qpid_sys_Poller>, const _qpid_sys_Socket&, _qpid_sys_ConnectionCodec::Factory*> _qpid_sys_EstablishedCallback;

class _qpid_sys_SocketAcceptor : public _qpid_sys_TransportAcceptor {
    boost::ptr_vector<_qpid_sys_Socket> listeners;
    boost::ptr_vector<_qpid_sys_AsynchAcceptor> acceptors;
    _qpid_sys_Timer& timer;
    _qpid_sys_SocketTransportOptions options;
    const _qpid_sys_EstablishedCallback established;
};
class _qpid_sys_SecurityLayer : public _qpid_sys_Codec
{
  public:
    int ssf;

    virtual ~_qpid_sys_SecurityLayer() {}
};

class _qpid_sys_SecureConnection : public _qpid_sys_ConnectionCodec
{
    std::auto_ptr<_qpid_sys_ConnectionCodec> codec;
    std::auto_ptr<_qpid_sys_SecurityLayer> securityLayer;
    bool secured;

    virtual ~_qpid_sys_SecureConnection();
};

#endif
