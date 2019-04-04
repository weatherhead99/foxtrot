#include "ServerCommandImpl.h"
#include "ServerUtil.h"

foxtrot::ServerCommandLogic::ServerCommandLogic(foxtrot::ExperimentalSetup& setup) : _setup(setup), _lg("ServerCommandLogic")
{
    
}

/*
bool foxtrot::ServerCommandLogic::HandleRequest(reqtp& req, repltp& repl, respondertp& respond, foxtrot::HandlerTag* tag)
{
    _lg.Debug("processing server command");
    
    repl.set_msgid(req.msgid());
    
    auto cmd = req.command();
    
    try{
    switch(cmd)
    {
//         case(server_commands::ReloadSetup): ReloadSetup(); break;
        case(server_commands::ChangeDebugLevel): break;
        
        default:
            foxtrot_server_specific_error("invalid server command requested",
                                          repl, respond, _lg, tag);
            return true;
    }
    
    respond.Finish(repl,grpc::Status::OK,tag);
    return true;
    }
    catch(...)
    {
     foxtrot_rpc_error_handling(std::current_exception(),repl,respond,_lg,tag);   
        
    }
        
}

*/

/*
void foxtrot::ServerCommandLogic::ReloadSetup()
{
    _lg.Debug("running reloadsetup command");
    _setup.reset();

};

void foxtrot::ServerCommandLogic::ChangeDebugLevel(const reqtp& req)
{
  _lg.Debug("changing debug level");
  foxtrot::setLogFilterLevel(level);

  
};
*/
