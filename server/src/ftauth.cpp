#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include <foxtrot/config.h>
#include <foxtrot/server/auth_utils.h>

namespace po = boost::program_options;
using std::string;

using std::cerr;
using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    std::string command;
    std::string server_credstore;
    std::string client_credstore;
    std::string userid;
    std::string keyname;
    int authlevel;
    
    po::options_description desc("foxtrot auth tool");
    desc.add_options()
    ("command",po::value<string>(&command)->required(), "action to perform")
    ("server_credstore,s",po::value<string>(&server_credstore), "path to server credential store")
    ("client_credstore,c",po::value<string>(&client_credstore), "path to client credential store")
    ("userid,u",po::value<string>(&userid), "user id, needed for some commands")
    ("authlevel,a", po::value<int>(&authlevel), "auth level, needed for some commands")
    ("keyname,k", po::value<string>(&keyname)->default_value("default"), "key name, needed for some commands")
    ("help","display usage information");
    
    po::positional_options_description pdesc;
    pdesc.add("command",1);
    pdesc.add("userid",1);
    pdesc.add("authlevel",1);
    
    po::variables_map vm;
    auto parsed = po::command_line_parser(argc,argv).
                options(desc).positional(pdesc).allow_unregistered().run();
    po::store(parsed,vm);
    
    
    if(vm.count("help"))
    {
        cout << "usage: " << argv[0] << " command [userid] [options]" << endl;
        cout << "description of options: " << endl;
        cout << desc << endl;
        std::exit(0);
    }
        
    try
    {
        po::notify(vm);
    }
    catch(po::required_option& err)
    {
        cerr <<  err.what() << endl;
        std::exit(1);
    }
    if(!vm.count("client_credstore"))
    {
        client_credstore = foxtrot::get_config_file_path("FOXTROT_CLIENT_CREDS","client_creds.json");
    }
    if(!vm.count("server_credstore"))
    {
        server_credstore = foxtrot::get_config_file_path("FOXTROT_SERVER_CREDS","server_creds.json");
    }
    
    

    if(command == "createuser")
    {
        if(!vm.count("userid"))
        {
            cerr << "must provide a userid for the createuser command" << endl;
            std::exit(1);
        }
        cout << "generating new keypair for user " << userid << endl;
        auto keypair = foxtrot::generate_new_key();
        foxtrot::save_creds_to_file(client_credstore,userid,keypair);
    }
    else if(command == "authuser")
    {

        if(!vm.count("userid"))
        {
            cerr << "must provide a userid for the createuser command" << endl;
            std::exit(1);
        }
        
        if(!vm.count("authlevel"))
        {
            cerr << "must provide authlevel for the authuser command" << endl;
            std::exit(1);
        }
        
        auto keys = foxtrot::get_creds_from_file(client_credstore, userid);
        foxtrot::auth_user_to_file(server_credstore, userid, std::get<0>(keys), authlevel, keyname);
        
    }
    else
    {
        cerr << "invalid command: " << command << endl;
        std::exit(1);
    }
}
