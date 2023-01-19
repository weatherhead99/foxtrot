#include <vector>
#include <iostream>
#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <foxtrot/DeviceHarness.h>

#include "exptserve.h"

using std::cout;
using std::endl;
namespace pt = boost::property_tree;

void dump_setup(const foxtrot::DeviceHarness& harness, const std::string& dumpfile)
{
  pt::ptree setup_tree;
  
  
  auto constdevs = harness.GetDevMap();
  
  std::vector<int> devids;
  devids.reserve(constdevs.size());
    
  for(const auto& dev : constdevs)
  {
      pt::ptree device_tree;
      cout << "devid: " << dev.first << endl;
      cout << "comment: " << dev.second->getDeviceComment() << endl;
      cout << "type: " << dev.second->getDeviceTypeName() << endl;
      
      device_tree.add("devid" , dev.first);
      device_tree.add("comment", dev.second->getDeviceComment());
      setup_tree.add_child(dev.second->getDeviceTypeName(),device_tree);
            
  }
  
  std::ofstream ofs(dumpfile);
  
  pt::write_json(ofs, setup_tree);
    
    
    
};
