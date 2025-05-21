#include <grpcpp/security/server_credentials.h>
#include <iostream>


#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_awaitable.hpp>


#include <foxtrot/Logging.h>
#include <proto/foxtrot.grpc.pb.h>
#include <agrpc/asio_grpc.hpp>


namespace asio = boost::asio;
using agrpc::GrpcContext;


using std::cout;
using std::endl;



foxtrot::exptserve::AsyncService service;



int main()
{
  cout << "hello" << endl;
  foxtrot::setDefaultSink();
  
 
  grpc::ServerBuilder builder;

  GrpcContext ctxt(builder.AddCompletionQueue());
  

  builder.AddListeningPort("localhost:50051", grpc::InsecureServerCredentials());

  builder.RegisterService(&service);
  auto serv = builder.BuildAndStart();

  
  cout << "calling repeatedly_request" << endl;

  using RPCT = agrpc::ServerRPC<&foxtrot::exptserve::AsyncService::RequestGetServerInfo>;
  agrpc::register_awaitable_rpc_handler<RPCT>(ctxt, service,
					      [] (RPCT& rpc, RPCT::Request& req) -> asio::awaitable<void>
					      {
						foxtrot::server_info sinfo;
						sinfo.set_rpc_version(12);
						co_await rpc.finish(sinfo, grpc::Status::OK);

						co_return;
					      }, asio::detached);
						

  

  ctxt.run();

}
