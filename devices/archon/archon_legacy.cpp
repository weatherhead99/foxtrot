#include "archon_legacy.h"
#include <rttr/registration>

using foxtrot::devices::archon_legacy;

// --------------------ARCHON LEGACY CODE  STARTS HERER
// ---------------------------

std::shared_ptr<archon_legacy> foxtrot::devices::archon_legacy::create(std::shared_ptr<simpleTCPBase>&& proto)
{
  std::shared_ptr<archon_legacy> out(new archon_legacy(proto));
  out->setup_modules();
  return out;
}


foxtrot::devices::archon_legacy::archon_legacy(std::shared_ptr<simpleTCPBase> proto)
  : archon(proto) {}

foxtrot::devices::archon_legacy::~archon_legacy() {}

const string foxtrot::devices::archon_legacy::getDeviceTypeName() const
{
  return "archon";
}



void foxtrot::devices::archon_legacy::update_state()
{
  
  _lg.Trace("system..");
  _system = getSystem();
  _lg.Trace("status..");
  _status = getStatus();
  _lg.Trace("frame..");
  _frame = getFrame();

  //NOTE: need a replacement for this!
  
  // for(auto& mod: _modules)
  // {
  //   if(mod.second != nullptr)
  //   {
  //    mod.second->update_variables(); 
  //   }
  // }

}




bool foxtrot::devices::archon_legacy::isbuffercomplete(int buf)
{
  std::ostringstream oss;
  oss << "BUF" << buf << "COMPLETE";
  auto complete = _frame.at(oss.str());
  
  return std::stoi(complete);

}

int foxtrot::devices::archon_legacy::get_frameno(int buf)
{
    std::ostringstream oss ;
  oss << "BUF" << buf << "FRAME";
  return std::stoi(_frame.at(oss.str()));


}

int foxtrot::devices::archon_legacy::get_height(int buf)
{
    std::ostringstream oss ;
  oss << "BUF" << buf << "HEIGHT";
  return std::stoi(_frame.at(oss.str()));

}

int foxtrot::devices::archon_legacy::get_pixels(int buf)
{
      std::ostringstream oss ;
  oss << "BUF" << buf << "PIXELS";
    return std::stoi(_frame.at(oss.str()));

};


string foxtrot::devices::archon_legacy::get_tstamp(int buf)
{
  std::ostringstream oss;
  oss << "BUF" << buf << "TIMESTAMP";
  auto tstamp = std::stoul(_frame.at(oss.str()),0,16);

  auto archontdiff = tstamp - _arch_tmr;
  //one tick of counter is 10ns
  auto frametime = _sys_tmr + std::chrono::nanoseconds(archontdiff * 10);
  return std::format("{:%FT%T}", frametime);
}



int foxtrot::devices::archon_legacy::get_rawlines(int buf)
{
  std::ostringstream oss;
  oss << "BUF" << buf << "RAWLINES";
  return std::stoi(_frame.at(oss.str()));

}


int foxtrot::devices::archon_legacy::get_rawblocks(int buf)
{
   std::ostringstream oss;
  oss << "BUF" << buf << "RAWBLOCKS";
  return std::stoi(_frame.at(oss.str()));
}



int foxtrot::devices::archon_legacy::get_width(int buf)
{
  std::ostringstream oss ;
  oss << "BUF" << buf << "WIDTH";
  return std::stoi(_frame.at(oss.str()));

}


int foxtrot::devices::archon_legacy::get_mode(int buf)
{
    std::ostringstream oss;
  oss << "BUF" << buf << "MODE";
  auto complete = _frame.at(oss.str());
  
  return std::stoi(complete);

}

bool foxtrot::devices::archon_legacy::get_32bit(int buf)
{
      std::ostringstream oss;
  oss << "BUF" << buf << "SAMPLE";
  auto complete = _frame.at(oss.str());
  
  return std::stoi(complete);
}

RTTR_REGISTRATION
{
  using namespace rttr;
  using foxtrot::devices::archon_legacy;
   registration::class_<foxtrot::devices::archon_legacy>("foxtrot::devices::archon_legacy")
   .method("update_state",&archon_legacy::update_state)
   .method("isbuffercomplete",&archon_legacy::isbuffercomplete)
    .method("get_frameno", &archon_legacy::get_frameno)
   (parameter_names("buf"))
   .method("get_width", &archon_legacy::get_width)
   (parameter_names("buf"))
   .method("get_height",&archon_legacy::get_height)
   (parameter_names("buf"))
   .method("get_mode",&archon_legacy::get_mode)
   (parameter_names("buf"))
   .method("get_32bit",&archon_legacy::get_32bit)
   .method("get_pixels",&archon_legacy::get_pixels)
   (parameter_names("buf"))
   .method("get_rawlines", &archon_legacy::get_rawlines)
   (parameter_names("buf"))
   .method("get_rawblocks", &archon_legacy::get_rawblocks)
   (parameter_names("buf"));


}
