#include "TelemetryClient.h"
#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>
#include "foxtrot.pb.h"
#include <boost/variant.hpp>


foxtrot::TelemetryClient::TelemetryClient()
: _lg("TelemetryClient")
{
  
  _lg.Debug("opening telemetry socket");
  _nn_sub_skt = nn_socket(AF_SP, NN_SUB);

}

foxtrot::TelemetryClient::~TelemetryClient()
{
  if(_nn_sub_skt > 0)
  {
    auto ret = nn_close(_nn_sub_skt);
    if(ret < 0)
    {
      _lg.Error("error closing nanomsg subscribe socket: " + std::string(nn_strerror(nn_errno())) );
    }
    
  }
  

}


void foxtrot::TelemetryClient::ConnectSocket(const std::string& connstr)
{
  auto ret = nn_connect(_nn_sub_skt,connstr.c_str());
  if(ret < 0)
  {
    _lg.Error("nn_connect: " + std::string(nn_strerror(nn_errno()))); 
    throw std::runtime_error("nanomsg err: " + std::string(nn_strerror(nn_errno())));
  }
  
}


void foxtrot::TelemetryClient::Subscribe(const std::string& topicstr)
{
  auto ret = nn_setsockopt(_nn_sub_skt,NN_SUB,NN_SUB_SUBSCRIBE,topicstr.data(),topicstr.size());

}


foxtrot::telemetry_message foxtrot::TelemetryClient::waitMessageSync()
{
//   std::unique_ptr<void*, void(*)(void*)> bufptr(nullptr,[](void* buffer) { nn_freemsg(buffer);});
  
  void* bufptr;
  
  _lg.Debug("calling nn_recv");
//   auto rlen = nn_recv(_nn_sub_skt,bufptr.get(),NN_MSG,0);
  auto rlen = nn_recv(_nn_sub_skt,&bufptr,NN_MSG,0);
  
  _lg.Debug("nn_recv came back");
  
  _lg.Trace("rlen: " + std::to_string(rlen));
  
  if(rlen < 0)
  {
    _lg.Error("nn_recv: " + std::string(nn_strerror(nn_errno())));
    throw std::runtime_error("nanomsg err: " + std::string(nn_strerror(nn_errno())));
  }
  
  
  char* charbfr = reinterpret_cast<char*>(bufptr);
  std::string instr(charbfr, charbfr+rlen);
  _lg.Trace("constructed instr");
  
  auto pbuf = decode_from_string(instr);
  
  _lg.Trace("decoded protobuf");
  
  
  foxtrot::telemetry_message out;
  out.name = pbuf.name();
  
  auto usec = boost::posix_time::microseconds(pbuf.tstamp());
  auto epoch = boost::posix_time::from_time_t(0);
  
  out.timestamp = epoch + usec;
  
  switch(pbuf.return_case())
  {
    case telemetry::kDblret: out.value = pbuf.dblret(); break;
    case telemetry::kIntret: out.value = pbuf.intret(); break;
    case telemetry::kBoolret: out.value = pbuf.boolret(); break;
    case telemetry::kStringret: out.value = pbuf.stringret(); break;
    
    default:
      _lg.Error("return_case: " + std::to_string(pbuf.return_case()));
      throw std::logic_error("received invalid type in protobuf telemetry message");  
  };

  return out;
}


foxtrot::telemetry foxtrot::TelemetryClient::decode_from_string(const std::string& instr)
{
    auto pos = std::find(instr.begin(),instr.end(), '>');
    if(pos == instr.end())
    {
      _lg.Error("received bad telemetry message!");
      throw std::runtime_error("received bad telemetry message");
    };
    
    foxtrot::telemetry out;
    
    std::string msg(pos,instr.end());
    out.ParseFromString(msg);
    
    return out;
    
}

