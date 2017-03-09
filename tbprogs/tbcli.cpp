#include "client.h"
#include <boost/program_options.hpp>
#include <iostream>

int main(int argc, char** argv)
{
    namespace po = boost::program_options;
    
    po::options_description desc("Allowed Options");
    
    desc.add_options()("cmd", po::value<std::string>(), "top level command");
    
    
    po::positional_options_description pdesc;
    pdesc.add("cmd",1);
    
    po::variables_map vm;
    
    auto parsed = po::command_line_parser(argc,argv)
    .options(desc).positional(pdesc).allow_unregistered().run();
    
    
    po::store(parsed, vm);
    
    auto cmd = vm["cmd"].as<std::string>();
    if(cmd == "heater")
    {
        std::cout << "heater command selected" << std::endl;
        
        
        
    }
    
    
    
    
    po::notify(vm);



}
