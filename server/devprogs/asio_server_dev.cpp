#include <grpcpp/security/server_credentials.h>
#include <iostream>


#include <agrpc/asio_grpc.hpp>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <boost/asio/thread_pool.hpp>

#include <foxtrot/Logging.h>
#include <foxtrot/foxtrot.grpc.pb.h>


namespace asio = boost::asio;
using agrpc::GrpcContext;







using std::cout;
using std::endl;



int main()
{
  cout << "hello" << endl;
  foxtrot::setDefaultSink();
  


  grpc::ServerBuilder builder;

  GrpcContext ctxt(builder.AddCompletionQueue());

  builder.AddListeningPort("localhost:50051", grpc::InsecureServerCredentials());

  foxtrot::exptserve::AsyncService service;
  builder.RegisterService(&service);
  auto serv = builder.BuildAndStart();


  asio::thread_pool thread_pool(1);
  
  
  

}
