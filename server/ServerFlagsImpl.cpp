#include "ServerFlagsImpl.h"
#include "ServerUtil.h"
  
  
class server_flag_visitor : public boost::static_visitor<>
{
public:
  server_flag_visitor(foxtrot::serverflag& flg) : flg_(flg)
  {
     
  };
  
  void operator()(double& i) const
  {
    flg_.set_dblval(i);
  };
  void operator()(int& i) const
  {
    flg_.set_intval(i);
  };
  void operator()(bool& i) const
  {
    flg_.set_boolval(i);
  };
  void operator()(const std::string& s) const
  {
    flg_.set_stringval(s);
  };
  
private:
  foxtrot::serverflag& flg_;
  
};


foxtrot::SetServerFlagsLogic::SetServerFlagsLogic(std::shared_ptr<flagmap> vars)
: lg_("SetServerFlagsLogic"), vars_(vars)
{
}

bool foxtrot::SetServerFlagsLogic::HandleRequest(foxtrot::SetServerFlagsLogic::reqtp& req, foxtrot::SetServerFlagsLogic::repltp& repl, foxtrot::SetServerFlagsLogic::respondertp& respond, foxtrot::HandlerTag* tag)
{
  lg_.Debug("processing set server flag request");
  
  
  try{
    repl.set_msgid(req.msgid());
    
    ft_variant var;
    switch(req.arg_case())
    {
      case(reqtp::ArgCase::kDblval):
	var = req.dblval();
	break;
      case(reqtp::ArgCase::kIntval):
	var = req.intval();
	break;
      case(reqtp::ArgCase::kBoolval):
	var = req.boolval();
	break;
      case(reqtp::ArgCase::kStringval):
	var = req.stringval();
	break;
    };

      vars_->operator[](req.flagname()) = var;
  }
  catch(...)
  {
    foxtrot_rpc_error_handling(std::current_exception(), repl,respond, lg_, tag);
    lg_.Trace("returned from error handling");
    return true;
    
  }
      
    return true;
  
}


foxtrot::GetServerFlagsLogic::GetServerFlagsLogic(std::shared_ptr<flagmap> vars)
: lg_("GetServerFlagsLogic"), vars_(vars)
{

}

bool foxtrot::GetServerFlagsLogic::HandleRequest(foxtrot::GetServerFlagsLogic::reqtp& req, foxtrot::GetServerFlagsLogic::repltp& repl, foxtrot::GetServerFlagsLogic::respondertp& respond, foxtrot::HandlerTag* tag)
{
  
  lg_.Trace("processing get flag request");
  
  repl.set_msgid(req.msgid());
  
  try{
 
    auto it = vars_->find(req.flagname());
    if(it == vars_->end())
    {
      throw std::out_of_range("invalid server flag");
    }
    
    repl.set_flagname(req.flagname());    
    boost::apply_visitor( server_flag_visitor(repl), it->second) ;
    
  }
  catch(...)
  {
    foxtrot_rpc_error_handling(std::current_exception(), repl,respond, lg_,tag);
    lg_.Trace("returned from error handling");
    return true;
  }

  return true;
}

