#include "scsiserial.h"
#include "ProtocolUtilities.h"

#include <scsi/sg.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <byteswap.h>
#include <cmath>

#include <algorithm>
#include <iostream>

foxtrot::protocols::scsiserial::scsiserial(const foxtrot::parameterset*const instance_parameters)
: SerialProtocol(instance_parameters)
{
  _sense_buffer.fill(0);

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
    
    extract_parameter_value(_LBA,_params,"LBA");    
    
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
    
    
    //get blen from device
    auto cap = scsi_read_capacity10();
    _blen = cap.second;
        
}


std::string foxtrot::protocols::scsiserial::read(unsigned int len, unsigned int* actlen)
{
  
  unsigned num_lbas = static_cast<unsigned>(std::ceil( len / static_cast<double>(_blen)));
//   std::cout << "num_lbas: " << num_lbas << std::endl;
  
  
  
  auto repl = scsi_read10(num_lbas,_LBA,_blen);
  
//   auto iter = repl.begin();
//   for(unsigned i = 0; i < 20; i++)
//   {
//     std::cout << (int) *iter++ << " " ;
//   
//   }
//  std::cout << std::endl; 
  
 auto zerpos = std::find(repl.begin(),repl.end(),static_cast<char>(0x00));
  return std::string(repl.begin(), zerpos);

}


void foxtrot::protocols::scsiserial::write(const std::string& data)
{
  
  unsigned num_lbas = static_cast<unsigned>(std::ceil( data.size() /static_cast<double>(_blen)));
  
//   std::cout << "num_lbas: " << num_lbas << std::endl;
  
  auto datastr = prepend_length_chrs(data);
  
  std::vector<unsigned char> datavec(datastr.begin(), datastr.end());
  //WARNING: doesn't properly handle multiple LBA case!!
  if(datavec.size() < _blen)
  {
    datavec.resize(_blen);
  };
  
  scsi_write10(datavec,_LBA,num_lbas);
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

bool foxtrot::protocols::scsiserial::scsi_test_unit_ready()
{
  unsigned char control = 0;
  std::array<unsigned char, 6> cmd {0, 0, 0, 0, 0, control};
  std::array<unsigned char, 0> reply;
  
  auto req = get_req_struct(cmd,reply,scsidirection::NONE);
  perform_ioctl(req);
  
  //TODO: make it work?
  return true;

}


std::array<unsigned char, 96> foxtrot::protocols::scsiserial::scsi_inquiry()
{
  std::array<unsigned char, 8> cmd{0x12, 0, 0, 0, 96, 0};
  std::array<unsigned char, 96> reply{0};

  auto req = get_req_struct(cmd,reply,scsidirection::FROM_DEV);
  perform_ioctl(req);
  
  return reply;
  
}



std::pair< unsigned int, unsigned int > foxtrot::protocols::scsiserial::scsi_read_capacity10()
{
  
  std::array<unsigned char, 8> cmd{0};
  std::array<unsigned char, 8> reply{0};
  
  cmd[0] = 0x25;
  
  auto req = get_req_struct(cmd,reply,scsidirection::FROM_DEV);
  perform_ioctl(req);
  
  struct replst { int LBA;
    int blen;
  };
  
  auto replstp = reinterpret_cast<replst*>(reply.data());
  std::pair<unsigned,unsigned> out{ __bswap_32(replstp->LBA), __bswap_32(replstp->blen)};
  
  return out;

}


std::vector< unsigned char > foxtrot::protocols::scsiserial::scsi_read10(short unsigned int num_lbas, unsigned int lba, unsigned int len)
{
  std::vector<unsigned char> data;
  data.reserve(len);
  
  unsigned char flags = 0;
  unsigned char group_number = 0;
  unsigned char control =0 ;
  
  unsigned char* lbap = reinterpret_cast<unsigned char*>(&lba);
  unsigned char* lenp = reinterpret_cast<unsigned char*>(&num_lbas);
  
  group_number &= 0b00011111;
  
  std::array<unsigned char, 10> cmd = {0x28, flags, lbap[3],lbap[2],lbap[1],lbap[0],group_number, 
    lenp[1],lenp[0], control};
    
  data.resize(len);
  auto req = get_req_struct(cmd,data,scsidirection::FROM_DEV);
  perform_ioctl(req);
  
//   std::cout << "req len: " << req.dxfer_len << std::endl;
  data.resize(req.dxfer_len);
  
  return data;
}

std::string foxtrot::protocols::scsiserial::read_until_endl(char endlchar)
{
  
  throw ProtocolError("not implemented yet!");

}


std::string foxtrot::protocols::scsiserial::prepend_length_chrs(const std::string& req)
{
    auto sz  = req.size();
  unsigned char* szp = reinterpret_cast<unsigned char*>(&sz);
  
  std::string out{szp[0], szp[1]  };
  
  return out + req;
  

}


