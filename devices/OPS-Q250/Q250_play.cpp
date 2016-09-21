#include <iostream>

#include "scsiserial.h"

#include <chrono>
#include <thread>


using std::cout;
using std::endl;

std::string form_command_string(const std::string& req)
{
  auto sz  = req.size();
  unsigned char* szp = reinterpret_cast<unsigned char*>(&sz);
  
  std::string out{szp[0], szp[1]  };
  
  std::cout << "command length: " << req.size() << std::endl;
  std::cout << "out+req: " << out + req << std::endl;
  
  for(auto c: (out + req))
  {
    cout << std::hex << (int) c << " ";
  }
  cout << endl;
    
  return out + req;
  
}

int main(int argc, char** argv)
{
  
  foxtrot::parameterset params{ 
    {"devnode", "/dev/sdb"},
    {"timeout", 2000u},
    {"LBA", 100000u}
  };
  



  foxtrot::protocols::scsiserial proto(&params);
  
  
  
  cout << "init protocol..." << endl;
  
  proto.Init(nullptr);
  
  auto cap = proto.scsi_read_capacity10();
  
  cout << "capacity: " << cap.first << "  " << cap.second << endl;
  
  cout << "inquiry" << endl;
  auto inq = proto.scsi_inquiry();
  
  cout << "response: " << std::string(inq.begin(), inq.end())  << endl;
  
  
  cout << "------------------ask amps-------------------" << endl;

//   auto cmdstr = form_command_string("AMPS?");
//   std::vector<unsigned char> cmdvec(cmdstr.begin(), cmdstr.end());
//   cmdvec.resize(512);
//   proto.scsi_write10(cmdvec,100000,1);  
//   
  proto.write("AMPS?");
  
  
  
  cout << "------------------asked----------------------" << endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  auto amps = proto.read(512);
  cout << "amps: " << amps << endl;
  
  
  
  cout << "------------------ask volts-------------------" << endl;

   proto.write("VOLTS?");
  
  //   cmdstr = form_command_string("VOLTS?");
//   cmdvec = std::vector<unsigned char>(cmdstr.begin(), cmdstr.end());
//   cmdvec.resize(512);
//   proto.scsi_write10(cmdvec,100000,1);
  cout << "------------------asked-----------------------" << endl;
  
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  
  auto volts = proto.read(512);
  cout << "volts: " << volts << endl;
  
  
  
  cout << "-------------------ask watts-------------------" << endl;
  proto.write("WATTS?");
  cout << "------------------asked ------------------------" << endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  auto watts = proto.read(512);
  cout << "watts: " << watts << endl;
  
  
  cout << "---------------------ask shutter----------------" << endl;
  proto.write("LAMP HRS?");
  cout << "-----------------asked--------------------------" << endl;
  
  auto shutter = proto.read(512);
  cout << "lamp hours: " << shutter << endl;
  
  
}