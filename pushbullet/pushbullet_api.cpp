#include "pushbullet_api.hh"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <iostream>

#ifdef linux
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#endif

#define PUSHBULLET_PUSHES_API "https://api.pushbullet.com/v2/pushes"

using namespace rapidjson;

foxtrot::pushbullet_api::pushbullet_api(const string & api_key)
: lg_("pushbullet_api"), api_key_(api_key)
{
    proto_ = std::make_shared<foxtrot::protocols::CurlRequest>();
}


void foxtrot::pushbullet_api::push_to_channel(const string& title, const string& body,
                                                  const string& channel)
{
    
    StringBuffer s;
    Writer<StringBuffer> writer(s);
    
    writer.StartObject();
    writer.Key("type");
    writer.String("note");
    
    writer.Key("title");
    writer.String(title.c_str());
    
    writer.Key("body");
    writer.String(body.c_str());
    
    writer.Key("channel_tag");
    writer.String(channel.c_str());
    
    writer.EndObject();
    
    std::vector<string> header{"Access-Token: " + api_key_, "Content-Type: application/json"};
    
    auto repl = proto_->blocking_post_request(PUSHBULLET_PUSHES_API,s.GetString(),&header);
    auto rcode = proto_->get_last_http_response_code();
    
    if(rcode <200  || rcode > 300)
    {
        lg_.strm(sl::debug) << "HTTP response: " << repl;
        //TODO: custom error type here
     
        throw *(lg_.strmthrow<std::runtime_error>(sl::error) << "invalid HTTP response code: " << rcode);
        
    }
    
}



string foxtrot::authorize_pushbullet_app(const string& client_id)
{
    
    std::ostringstream oss;
    oss << "https://www.pushbullet.com/authorize?client_id=" << client_id
    << "&redirect_uri=http%3A%2F%2Flocalhost%3A8005/token&response_type=code";
    
//     std::cout << "string is: " << oss.str() << std::endl;
    
    
#ifdef linux
    
    char buffer[2048];
    
    std::cout << "binding socket to get return address.." << std::endl;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        std::cerr << "couldn't create socket.." << std::endl;
        std::exit(1);
    }
    
    struct sockaddr_in serv_addr, cli_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(8005);
    
    int ret = bind(sock, (sockaddr*) &serv_addr, sizeof(serv_addr));
    if(ret == -1)
    {
        std::cerr << "couldn't bind socket" << std::endl;
        close(sock);
        std::exit(1);
    }
    
    ret = listen(sock, 1);
    if(ret == -1)
    {
        std::cerr << "bad return code from listen: " << ret << std::endl;
        close(sock);
        std::exit(1);
    }
    
    std::cout << "opening authorization page in your browser..." << std::endl;
    auto callstr = std::string{"xdg-open \""} + oss.str() + "\"";
    ret = system(callstr.c_str());
    if(ret == -1)
    {
        std::cerr << "bad return code from xdg-open:" << ret <<  std::endl;
        close(sock);
        std::exit(1);
    }
        
    unsigned int clilen = sizeof(cli_addr);
    auto newsockfd = accept(sock, (sockaddr*) &cli_addr, &clilen);
    if(newsockfd < 0)
    {
        std::cerr << "error on accept: " << newsockfd << std::endl;
        close(sock);
        std::exit(1);
    }
    
    
    
    bzero(buffer,2048);
    int n = read(newsockfd, buffer, 2047);
    if(n < 0)
    {
        std::cerr << "error reading from socket: " << n << std::endl;
        close(sock);
        std::exit(1);
    };
    
    
    std::string success{"<html> <h1> You may now close this tab </h1> </html>"};
    
    std::ostringstream return_buffer;
    return_buffer << "HTTP/1.1 200 OK" << std::endl
    << "Content-Type: text/html" << std::endl 
    << "Content-Length: " << success.size() << std::endl
    << "Connection: close" << std::endl  << std::endl
    << success ;
    
    
    std::cout << "sending reply... " << std::endl;
    std::cout << "reply buffer: " << return_buffer.str() << std::endl;
    
    n = write(newsockfd, return_buffer.str().c_str(),return_buffer.str().size());
    if(n < 0)
    {
        std::cerr << "error writing response: " << n << std::endl;
        close(sock);
        std::exit(1);
    }
    
    
    close(sock);
    
#else
#error "this is only supported on linux at the moment"
#endif
    
    std::string buf{buffer};
    auto startpos=buf.find("?code=");
    auto endpos=buf.find("&state");

    if(startpos == string::npos || endpos == string::npos)
    {
      std::cerr << "couldn't parse response!" << std::endl;
      std::cerr << "total response was: " << buffer << std::endl;
      
      throw std::runtime_error("couldn't get key, did you click deny?");
    };
    
    auto len = (endpos - startpos - 6);
    return buf.substr(startpos + 6,len);
};


