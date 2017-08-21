#include "client.h"
#include "Logging.h"
#include <backward.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char** argv)
{
    backward::SignalHandling sh;
    foxtrot::Logging lg("archon_bridge");
    foxtrot::setDefaultSink();
    
    foxtrot::Client cl("localhost:50051");
    
    auto sd = cl.DescribeServer();
    
    auto devid = foxtrot::find_devid_on_server(sd,"archon");
    if(devid == -1)
    {
        lg.Fatal("no archon found on foxtrot server");
        return -1;
    };
    
    boost::asio::io_service io_service;
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 50053));
    
    for(;;)
    {
     tcp::socket sock(io_service);
     acceptor.accept(sock);
     boost::asio::streambuf buffer;
     boost::system::error_code err_code;
     
    }
    



}
