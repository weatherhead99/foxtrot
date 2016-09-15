#pragma once

#include "SerialProtocol.h"
#include <string>
#include <vector>
#include <scsi/sg.h>
#include <array>



namespace foxtrot
{
  namespace protocols
  {
    enum class scsidirection
    {
      NONE = SG_DXFER_NONE,
      TO_DEV = SG_DXFER_TO_DEV,
      FROM_DEV = SG_DXFER_FROM_DEV
    };
    
    
    class scsiserial : public SerialProtocol
    {
    public:
    scsiserial(const parameterset*const instance_parameters);
    virtual ~scsiserial();
    virtual void Init(const parameterset*const class_parameters);
    
    virtual std::string read(unsigned int len, unsigned* actlen = nullptr) override;
    virtual void write(const std::string& data) override;
      
    private:
      void perform_ioctl(sg_io_hdr_t& req);
      template <typename arrtp1, typename arrtp2>   
      sg_io_hdr_t get_req_struct(arrtp1& cmd, arrtp2& reply, const scsidirection dir);
      
      
      template <typename arrtp>
      void scsi_write10(arrtp& data, unsigned lba, unsigned num_lbas);
      
      std::vector<unsigned char> scsi_read10(unsigned short num_lbas, unsigned lba, unsigned len);
      
      //TODO: template impl?

      std::string _devnode;
      int _fd;
      unsigned _timeout;
      std::array<unsigned char, 32> _sense_buffer;
      
    };
    
    
  



template <typename arrtp1, typename arrtp2> 
sg_io_hdr_t scsiserial::get_req_struct(arrtp1& cmd, arrtp2& reply, 
							   const foxtrot::protocols::scsidirection dir)    
{
    sg_io_hdr_t req;
    req.interface_id = 'S'; //always 'S' for now
    req.dxfer_direction = static_cast<int>(dir);
    req.iovec_count = 0;
    req.timeout = _timeout;
    
    req.dxferp = reply.data();
    req.dxfer_len = reply.size();
    req.sbp = _sense_buffer.data();
    req.mx_sb_len = _sense_buffer.size();
    req.cmdp = cmd.data();
    req.cmd_len = cmd.size();
    
    return std::move(req);
};


template <typename arrtp> 
void scsiserial::scsi_write10(arrtp& data, unsigned lba, unsigned num_lbas)
{
  unsigned char flags = 0;
  unsigned char group_number = 0;
  unsigned char control = 0;
  
  group_number &= 0b11111;
  
  unsigned char* lbap = reinterpret_cast<unsigned char*>(&lba);
  unsigned char* lenp = reinterpret_cast<unsigned char*>(&num_lbas);
  
  std::array<unsigned char,10> cmd = {0x2A, flags,lbap[3],lbap[2],lbap[1],lbap[0], group_number,
    lenp[1],lenp[0], control};
  
  auto req = get_req_struct(cmd,data,scsidirection::TO_DEV);
};

      
  } //namespace protocols  
}; //namespace foxtrot
      