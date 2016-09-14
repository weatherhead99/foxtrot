#include "scsiserial.h"
#include "ProtocolUtilities.h"

#include <scsi/sg.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

foxtrot::protocols::scsiserial::scsiserial(const foxtrot::parameterset*const instance_parameters)
: SerialProtocol(instance_parameters)
{

}

foxtrot::protocols::scsiserial::~scsiserial()
{
  close(_fd);

}



void foxtrot::protocols::scsiserial::Init(const foxtrot::parameterset*const class_parameters)
{
    //call base class to merge parameter sets
    foxtrot::CommunicationProtocol::Init(class_parameters);
    
    extract_parameter_value(_devnode,_params,"devnode");
    extract_parameter_value(_timeout,_params,"timeout");
    
    _fd = open(_devnode.c_str(),O_RDWR);
    
    if(_fd < 0) 
    {
      throw ProtocolError(std::string("couldn't open devnode: ") + _devnode + std::string(" ") + strerror(errno));
    };
    
    int ret;
    
    //test scsi IOCTL
    int vers = 0;
    ret = ioctl(_fd, SG_GET_VERSION_NUM, &vers);
    if(ret < 0)
    {
      throw ProtocolError(std::string("ioctl failed: ") + strerror(ret));
    };
    
    
    
}


void foxtrot::protocols::scsiserial::perform_ioctl(sg_io_hdr_t& req)
{
  auto err = ioctl(_fd, SG_IO, &req);
  if(err <0 )
  {
    throw ProtocolError("ioctl failed: " + std::string(strerror(err)));
  };
  
  //TODO: exceptions here?
  if ( (req.info  & SG_INFO_OK_MASK ) != SG_INFO_OK)
  {
    if(req.sb_len_wr > 0)
    {
     std::cout << "got sense data: " << std::endl; 
     
     for(auto s : _sense_buffer)
     {
      std::cout << static_cast<int>(s) << " " ; 
     }
     std::cout << std::endl;
      
    }
    
    if(req.masked_status)
    {
      std::cout << "scsi status: " << std::hex << static_cast<int>(req.status) << std::endl;
    }
    
    if(req.host_status)
    {
      std::cout << "host status: " << std::hex << static_cast<int>(req.host_status) << std::endl;
    }
    
    if(req.driver_status)
    {
     std::cout << "driver status: " << std::hex << static_cast<int>(req.driver_status) << std::endl; 
      
    }
  };
  

}

