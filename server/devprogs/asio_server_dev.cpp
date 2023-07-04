#include <grpcpp/security/server_credentials.h>
#include <iostream>


#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/spawn.hpp>

#include <foxtrot/Logging.h>
#include <foxtrot/foxtrot.grpc.pb.h>
#include <agrpc/asio_grpc.hpp>


namespace asio = boost::asio;
using agrpc::GrpcContext;


using std::cout;
using std::endl;



foxtrot::exptserve::AsyncService service;

template<typename H>
void handler(asio::basic_yield_context<H>& yield)
{
  grpc::ServerContext sctxt;
  foxtrot::empty req;

  grpc::ServerAsyncResponseWriter<foxtrot::server_info> writer{&sctxt};

  agrpc::request(&foxtrot::exptserve::AsyncService::RequestGetServerInfo,
		 service, sctxt, req, writer, yield);
  

  foxtrot::server_info repl;
  repl.set_rpc_version(12);


}





int main()
{
  cout << "hello" << endl;
  foxtrot::setDefaultSink();
  
 
  grpc::ServerBuilder builder;

  GrpcContext ctxt(builder.AddCompletionQueue());

  asio::basic_yield_context<agrpc::GrpcExecutor> yield;
  
  auto exec = ctxt.get_executor();

  builder.AddListeningPort("localhost:50051", grpc::InsecureServerCredentials());

  builder.RegisterService(&service);
  auto serv = builder.BuildAndStart();

  //  asio::spawn(ctxt, handler<decltype(exec)>);;
  

}
