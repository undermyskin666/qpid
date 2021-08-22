#ifndef _QPID_FRAMING_SYMS_H_H
#define _QPID_FRAMING_SYMS_H_H
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <queue>
#include <set>
#include <map>
#include "boost/shared_ptr.hpp"
#include "boost/noncopyable.hpp"
#include "boost/function.hpp"
#include "boost/detail/atomic_count.hpp"
#include <boost/intrusive_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_array.hpp>
#include "QpidSyms.h"

typedef uint8_t   _qpid_framing_FrameType;
typedef uint16_t  _qpid_framing_ChannelId;
typedef uint32_t  _qpid_framing_BatchOffset;
typedef uint8_t   _qpid_framing_ClassId;
typedef uint8_t   _qpid_framing_MethodId;
typedef uint16_t  _qpid_framing_ReplyCode;


enum _qpid_framing_TypeCode {
    TYPE_CODE_BIN8 = 0x00,
    TYPE_CODE_INT8 = 0x01,
    TYPE_CODE_UINT8 = 0x02,
    TYPE_CODE_CHAR = 0x04,
    TYPE_CODE_BOOLEAN = 0x08,
    TYPE_CODE_BIN16 = 0x10,
    TYPE_CODE_INT16 = 0x11,
    TYPE_CODE_UINT16 = 0x12,
    TYPE_CODE_BIN32 = 0x20,
    TYPE_CODE_INT32 = 0x21,
    TYPE_CODE_UINT32 = 0x22,
    TYPE_CODE_FLOAT = 0x23,
    TYPE_CODE_CHAR_UTF32 = 0x27,
    TYPE_CODE_BIN64 = 0x30,
    TYPE_CODE_INT64 = 0x31,
    TYPE_CODE_UINT64 = 0x32,
    TYPE_CODE_DOUBLE = 0x33,
    TYPE_CODE_DATETIME = 0x38,
    TYPE_CODE_BIN128 = 0x40,
    TYPE_CODE_UUID = 0x48,
    TYPE_CODE_BIN256 = 0x50,
    TYPE_CODE_BIN512 = 0x60,
    TYPE_CODE_BIN1024 = 0x70,
    TYPE_CODE_VBIN8 = 0x80,
    TYPE_CODE_STR8_LATIN = 0x84,
    TYPE_CODE_STR8 = 0x85,
    TYPE_CODE_STR8_UTF16 = 0x86,
    TYPE_CODE_VBIN16 = 0x90,
    TYPE_CODE_STR16_LATIN = 0x94,
    TYPE_CODE_STR16 = 0x95,
    TYPE_CODE_STR16_UTF16 = 0x96,
    TYPE_CODE_VBIN32 = 0xa0,
    TYPE_CODE_MAP = 0xa8,
    TYPE_CODE_LIST = 0xa9,
    TYPE_CODE_ARRAY = 0xaa,
    TYPE_CODE_STRUCT32 = 0xab,
    TYPE_CODE_BIN40 = 0xc0,
    TYPE_CODE_DEC32 = 0xc8,
    TYPE_CODE_BIN72 = 0xd0,
    TYPE_CODE_DEC64 = 0xd8,
    TYPE_CODE_VOID = 0xf0,
    TYPE_CODE_BIT = 0xf1
};

class _qpid_framing_AMQDataBlock
{
public:
    virtual ~_qpid_framing_AMQDataBlock() ;
};

class _qpid_framing_AMQBody : public _qpid_RefCounted {
  public:
    virtual ~_qpid_framing_AMQBody();
};

class _qpid_framing_AMQFrame : public _qpid_framing_AMQDataBlock
{
  public:
    ~_qpid_framing_AMQFrame();

    boost::intrusive_ptr<_qpid_framing_AMQBody> body;
    uint16_t channel : 16;
    uint8_t subchannel : 8;
    bool bof : 1;
    bool eof : 1;
    bool bos : 1;
    bool eos : 1;
};

class  _qpid_framing_FieldValue {
  public:
    class Data {
      public:
        virtual ~Data() {}
    };
    uint8_t typeOctet;
    std::auto_ptr<Data> data;
    ~_qpid_framing_FieldValue();
};

class _qpid_framing_FieldTable
{
  public:
    typedef boost::shared_ptr<_qpid_framing_FieldValue> ValuePtr;
    typedef std::map<std::string, ValuePtr> ValueMap;
    typedef ValueMap::iterator iterator;
    typedef ValueMap::const_iterator const_iterator;
    typedef ValueMap::const_reference const_reference;
    typedef ValueMap::reference reference;
    typedef ValueMap::value_type value_type;

    mutable _qpid_sys_Mutex lock;
    mutable ValueMap values;
    mutable boost::shared_array<uint8_t> cachedBytes;
    mutable uint32_t cachedSize; // if = 0 then non cached size as 0 is not a legal size
    mutable bool newBytes;

    _qpid_framing_FieldTable();
};

class _qpid_framing_Array
{
  public:
    typedef boost::shared_ptr<_qpid_framing_FieldValue> ValuePtr;
    typedef std::vector<ValuePtr> ValueVector;
    typedef ValueVector::const_iterator const_iterator;
    typedef ValueVector::iterator iterator;

  private:
    _qpid_framing_TypeCode type;
    ValueVector values;

};

class _qpid_framing_AMQMethodBody : public _qpid_framing_AMQBody {
  public:
    virtual ~_qpid_framing_AMQMethodBody();
};

class _qpid_framing_ConnectionStartOkBody : public _qpid_framing_AMQMethodBody {
public:
    _qpid_framing_FieldTable clientProperties;
    std::string mechanism;
    std::string response;
    std::string locale;
    uint16_t flags;

    virtual ~_qpid_framing_ConnectionStartOkBody();
    typedef void ResultType;
}; /* class ConnectionStartOkBody */

class _qpid_framing_ConnectionTuneBody : public _qpid_framing_AMQMethodBody {
public:
    uint16_t channelMax;
    uint16_t maxFrameSize;
    uint16_t heartbeatMin;
    uint16_t heartbeatMax;
    uint16_t flags;
    typedef void ResultType;

    virtual ~_qpid_framing_ConnectionTuneBody();

}; /* class ConnectionTuneBody */

class _qpid_framing_ConnectionOpenBody : public _qpid_framing_AMQMethodBody {
public:
    std::string virtualHost;
    _qpid_framing_Array capabilities;
    uint16_t flags;
    typedef void ResultType;
    virtual ~_qpid_framing_ConnectionOpenBody();
}; 

class _qpid_framing_ConnectionCloseBody : public _qpid_framing_AMQMethodBody {
public:
    uint16_t replyCode;
    std::string replyText;
    uint16_t flags;
    static const _qpid_framing_ClassId CLASS_ID = 0x1;
    static const _qpid_framing_MethodId METHOD_ID = 0xb;
    typedef void ResultType;
    virtual ~_qpid_framing_ConnectionCloseBody();

}; /* class ConnectionCloseBody */

#endif

