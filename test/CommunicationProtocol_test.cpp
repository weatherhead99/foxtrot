#include "CommunicationProtocol.h"
#include <map>
#include <boost/variant.hpp>

#define BOOST_TEST_MODULE CommunicationProtocol
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace foxtrot;




BOOST_AUTO_TEST_CASE(test_parameter_merge)
{
  
  //test that assigning the "class parameters" - B during Init
  //overwrites the "instance parameters" - A in the protocol handler
  
  parameterset A{ {"A", 1 }, {"B",  1 }, {"C", 1 }  };
  
  CommunicationProtocol proto(&A);
  
  parameterset B{ {"A", 2 }  };
  
  proto.Init(&B);
  
  auto protoparams = proto.GetParameters();
  
  BOOST_CHECK_EQUAL( boost::get<int>(protoparams["A"]) , 2 );

  BOOST_CHECK_EQUAL( boost::get<int>(protoparams["B"]), 1);
  
  
};