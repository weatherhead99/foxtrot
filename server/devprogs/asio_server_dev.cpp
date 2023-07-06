#include <grpcpp/security/server_credentials.h>
#include <iostream>


#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/detached.hpp>

#include <foxtrot/Logging.h>
#include <foxtrot/foxtrot.grpc.pb.h>
#include <agrpc/asio_grpc.hpp>


namespace asio = boost::asio;
using agrpc::GrpcContext;


using std::cout;
using std::endl;



foxtrot::exptserve::AsyncService service;


void handler(asio::yield_context yield)
{
  grpc::ServerContext sctxt;
  foxtrot::empty req;

  grpc::ServerAsyncResponseWriter<foxtrot::server_info> writer{&sctxt};

  agrpc::request(&foxtrot::exptserve::AsyncService::RequestGetServerInfo,
		 service, sctxt, req, writer, yield);
  

  foxtrot::server_info repl;
  repl.set_rpc_version(12);

  agrpc::finish(writer, repl, grpc::Status::OK, yield);
}


template <typename Context, typename Executor, typename CompletionToken>
void rep_handler(Context&& ctxt, Executor&& exec, CompletionToken&& token)
{
  auto resp = ctxt.responder();
  foxtrot::server_info repl;

  repl.set_rpc_version(12);

  agrpc::finish(resp, repl, grpc::Status::OK, token);

}


int main()
{
  cout << "hello" << endl;
  foxtrot::setDefaultSink();
  
 
  grpc::ServerBuilder builder;

  GrpcContext ctxt(builder.AddCompletionQueue());
  
  auto exec = ctxt.get_executor();

  builder.AddListeningPort("localhost:50051", grpc::InsecureServerCredentials());

  builder.RegisterService(&service);
  auto serv = builder.BuildAndStart();

  cout << "calling repeatedly_request" << endl;
  agrpc::repeatedly_request(&foxtrot::exptserve::AsyncService::RequestGetServerInfo,
			    service,
			    asio::bind_executor(ctxt, 
			    [](auto&& rctxt, auto&& exec)
			    {
			      auto resp = rctxt.responder();
			      foxtrot::server_info repl;
			      repl.set_rpc_version(12);
			      agrpc::finish(resp, repl, grpc::Status::OK, asio::detached);

			    }));

  ctxt.run();

}
