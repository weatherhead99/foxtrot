#include "ServerDescribeImpl.h"
#include <iostream>
#include <foxtrot/typeUtil.h>


//this one just to get CapabilityType

foxtrot::devcapability GetDeviceCapability(const foxtrot::Capability& cap, foxtrot::Logging* lg=nullptr)
{
  using namespace foxtrot;
  using rttr::type;
  
    foxtrot::devcapability out;
    out.set_capname(cap.CapabilityName);

    if(lg)
      lg->strm(sl::trace) << "setting capability type...";
    switch(cap.type)
    {
        case(CapabilityType::VALUE_READONLY): out.set_tp(capability_types::VALUE_READONLY); break;
        case(CapabilityType::VALUE_READWRITE): out.set_tp(capability_types::VALUE_READWRITE); break;
        case(CapabilityType::ACTION): out.set_tp(capability_types::ACTION); break;
        case(CapabilityType::STREAM): out.set_tp(capability_types::STREAM); break;
    }
    
    if(cap.type == CapabilityType::STREAM)
    {
        if(! cap.Returntype.is_template_instantiation())
        {
            throw std::logic_error("stream return type is not a template instantiation. Don't know how to deal with this");
        }
        auto tempargs = cap.Returntype.get_template_arguments();
        auto value_type = *tempargs.begin();
        
        if(value_type == type::get<unsigned char>())
            out.set_vecrettp(byte_data_types::UCHAR_TYPE);
	else if(value_type == type::get<char>())
	    out.set_vecrettp(byte_data_types::CHAR_TYPE);
        else if(value_type == type::get<unsigned short>())
            out.set_vecrettp(byte_data_types::USHORT_TYPE);
        else if(value_type == type::get<unsigned>())
            out.set_vecrettp(byte_data_types::UINT_TYPE);
        else if(value_type == type::get<unsigned long>())
            out.set_vecrettp(byte_data_types::ULONG_TYPE);
        else if(value_type == type::get<short>())
            out.set_vecrettp(byte_data_types::SHORT_TYPE);
        else if(value_type == type::get<int>())
            out.set_vecrettp(byte_data_types::IINT_TYPE);
        else if(value_type == type::get<long>())
            out.set_vecrettp(byte_data_types::LONG_TYPE);
        else if(value_type == type::get<float>())
            out.set_vecrettp(byte_data_types::BFLOAT_TYPE);
        else if(value_type == type::get<double>())
            out.set_vecrettp(byte_data_types::BDOUBLE_TYPE);
        else
            throw std::out_of_range("invalid value type for stream: " + std::string(value_type.get_name()));
        
    }
    else
    {
      if(lg)
        lg->strm(sl::debug) << "action type, describing  return type:" << cap.Returntype.get_name().to_string();
      *(out.mutable_rettp()) = describe_type(cap.Returntype, lg);
    }
    
    for(auto& name: cap.Argnames)
        out.add_argnames(name);
    
    for(auto& type: cap.Argtypes)
    {
      if(lg)
        lg->strm(sl::debug) << "describing argument with type: " << type.get_name().to_string();
      *(out.add_argtypes()) = describe_type(type, lg);

    }

    return out;



}




foxtrot::ServerDescribeLogic::ServerDescribeLogic(const std::string& comment, std::shared_ptr<foxtrot::DeviceHarness> harness)
: _servcomment(comment), _harness(harness), _lg("ServerDescribeLogic")
{
}


bool foxtrot::ServerDescribeLogic::HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag)
{
      _lg.Debug("processing server describe request");
      
      repl.set_servcomment(_servcomment);
      
      auto devmap = _harness->GetDevMap();
      
      auto outdevmap = repl.mutable_devs_attached();
      
      for(auto [devid, dev] : devmap)
      {
	if(!dev)
	  throw std::logic_error("device pointer not present in harness! Should never happen");
	
          devdescribe desc;
          desc.set_devid(devid);
          desc.set_devtype(dev->getDeviceTypeName());
          desc.set_devcomment(dev->getDeviceComment());
          
          //enumerate capabilities
          
          for(auto [capid, cap]  : dev->Registry())
          {
	    _lg.strm(sl::debug) << "adding capability: " << capid << " named: " << cap.CapabilityName ;
	    auto tempcap = GetDeviceCapability(cap, &_lg);

	    //for now, manually add the ID in here. MAKE SURE message carries the capabiility ID
	    tempcap.set_capid(capid);
	    *(desc.add_caps()) = tempcap;
          }
          
          (*outdevmap)[devid] = desc;    

      };
        
     respond.Finish(repl,grpc::Status::OK,tag);
     return true;
}
