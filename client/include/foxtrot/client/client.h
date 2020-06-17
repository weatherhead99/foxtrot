#pragma once
#include <string>
#include <memory>
#include <iterator>
#include <iostream>
#include <type_traits>
#include <initializer_list>

#include <variant>
#include <grpc++/grpc++.h>

#include <foxtrot/foxtrot.grpc.pb.h>
#include <foxtrot/ft_capability.grpc.pb.h>
#include <foxtrot/ft_capability.pb.h>
#include <foxtrot/ft_flags.pb.h>
#include <foxtrot/ft_flags.grpc.pb.h>
#include <foxtrot/Logging.h>

#include <foxtrot/DeviceError.h>
#include <foxtrot/ProtocolError.h>

namespace foxtrot
{

namespace detail2
{
template<typename T>
struct has_const_iterator {
private:
    template<typename C> static char test ( typename C::const_iterator* );
    template<typename C> static int test ( ... );
public:
    enum {value = sizeof ( test<T> ( 0 ) ) == sizeof ( char ) };
};

template<typename T, typename = void>
struct is_iterator {
    static constexpr bool value = false;
};

template<typename T>
struct is_iterator<T, typename std::enable_if<!std::is_same<typename std::iterator_traits<T>::value_type, void>::value>::type> {
    static constexpr bool value = true;
};

}

using ft_std_variant = std::variant<double, int, bool, std::string>;
using ft_vector_variant = std::variant<std::vector<unsigned char>, std::vector<unsigned short>, std::vector<unsigned>, std::vector<unsigned long>, std::vector<short>, std::vector<int>, std::vector<long>, std::vector<float>, std::vector<double>> ;



template<typename T,typename R=ft_std_variant>
using enable_has_const_iterator = typename std::enable_if<detail2::has_const_iterator<T>::value, R>::type;

template<typename T, typename R=ft_std_variant>
using enable_is_iterator = typename
                           std::enable_if<detail2::is_iterator<T>::value, R>::type;


class ft_variant_visitor
{
public:
    ft_variant_visitor ( capability_argument& arg );

    void operator() ( double& i ) const;
    void operator() ( int& d ) const;
    void operator() ( bool& i ) const;
    void operator() ( const std::string& s ) const;


private:
    capability_argument& _arg;

};

class ft_variant_flag_visitor 
{
public:
    ft_variant_flag_visitor ( serverflag& flag );

    void operator() ( const double& d ) const;
    void operator() ( const int& i ) const;
    void operator() ( const bool& b ) const;
    void operator() ( const std::string& s ) const;

private:
    serverflag& _flag;
};


template<typename T>
void check_repl_err ( const T& repl, foxtrot::Logging* lg = nullptr )
{
    if ( repl.has_err() ) {
        if ( lg ) {
            lg->Error ( "there's an error in this response..." );
            lg->strm ( sl::debug ) << "error type:" << repl.err().tp();
        };

        auto err = repl.err();

        if ( lg )
            lg->Info ( "constructing exceptions..." );


        switch ( err.tp() ) {
        case ( error_types::ft_Error ) :
            throw foxtrot::Error ( err.msg() );
        case ( error_types::ft_DeviceError ) :
            throw foxtrot::DeviceError ( err.msg() );
        case ( error_types::ft_ProtocolError ) :
            throw foxtrot::ProtocolError ( err.msg() );
        case ( error_types::out_of_range ) :
            throw std::out_of_range ( err.msg() );
        default:
            throw std::runtime_error ( err.msg() );
        }

    }


};

ft_std_variant ft_variant_from_response ( const capability_response& repl );
ft_std_variant ft_variant_from_response ( const serverflag& repl );
ft_vector_variant ft_variant_from_data ( const foxtrot::byte_data_types& tp, const std::vector<unsigned char>& data );


class ft_variant_printer
{
public:
    ft_variant_printer();

    void operator() ( const double& d );
    void operator() ( const int& i );
    void operator() ( const bool& b );
    void operator() ( const std::string& s );

    std::string string();

private:
    std::string _str;

};



class Client
{
public:
    class capability_proxy
    {
        friend class Client;
        ft_std_variant operator() ( std::initializer_list<ft_std_variant> args );
    protected:
        capability_proxy ( Client& cl, int devid, const std::string& capname );

    private:
        Client& _clientbackref;
        std::string _capname;
        int _devid;
    };


    Client ( const std::string& connstr );
    ~Client();
    servdescribe DescribeServer();

    ft_std_variant get_server_flag ( const std::string& flagname );
    void set_server_flag ( const std::string& flagname, const ft_std_variant& val );
    void drop_server_flag ( const std::string& flagname );

    std::vector<std::string> get_flag_names();


    template<typename iteratortp>
    enable_is_iterator<iteratortp> InvokeCapability ( int devid,const std::string& capname, iteratortp begin_args, iteratortp end_args );

    template<typename containertp>
    enable_has_const_iterator<containertp> InvokeCapability ( int devid,const std::string& capname, containertp args );

    template<typename... Args>
    ft_std_variant InvokeCapability ( int devid, const std::string& capname, Args... args );
    ft_std_variant InvokeCapability ( int devid, const std::string& capname );
    ft_std_variant InvokeCapability ( int devid, const std::string& capname, std::initializer_list<ft_std_variant> args );

    capability_proxy call ( int devid, const std::string& capname );
// 	std::vector<unsigned char> FetchData(int devid, const std::string& capname, unsigned dataid, unsigned chunksize, std::initializer_list<ft_std_variant> args);
    template <typename iteratortp> ft_vector_variant FetchData ( int devid, const std::string& capname, unsigned dataid, unsigned chunksize, iteratortp begin_args, iteratortp end_args );

    template<typename... Args>
    ft_vector_variant FetchData ( int devid, const std::string& capname, unsigned dataid,
                                  unsigned chunksize, Args... args );

    void BroadcastNotification(const std::string& body, const std::string* title = nullptr,
                               const std::string* channel = nullptr);
private:
    template< typename reqtp, typename iteratortp> void fill_args ( reqtp& req, iteratortp args_begin, iteratortp args_end )
    {
        auto outargs = req.mutable_args();
        int i=0;
        for ( auto it = args_begin; it != args_end; it++ ) {
            auto arg = outargs->Add();
            std::visit( ft_variant_visitor ( *arg ),*it );
            arg->set_position ( i++ );

        };

    }

    std::unique_ptr<foxtrot::capability::Stub> _capability_stub;
    std::unique_ptr<foxtrot::flags::Stub> _flags_stub;
    std::unique_ptr<foxtrot::exptserve::Stub> _exptserve_stub;
    //std::unique_ptr<exptserve::Stub> _stub;
    std::shared_ptr<grpc::Channel> _channel;
    int _msgid = 0;
    Logging _lg;

};

int find_devid_on_server ( const foxtrot::servdescribe& sd, const std::string& devtp );
int find_capability ( const foxtrot::servdescribe& sd, int devid, const std::string& capability_name );
int get_number_of_args ( const foxtrot::servdescribe& sd, int devid, int capidx );
int get_arg_position ( const foxtrot::servdescribe& sd, int devid, int capidx, const std::string& arg_name );


template<typename iteratortp>
enable_is_iterator<iteratortp>
Client::InvokeCapability ( int devid,const std::string& capname, iteratortp begin_args, iteratortp end_args )
{

    static_assert ( std::is_same<typename std::iterator_traits<iteratortp>::value_type, ft_std_variant>::value,
                    "iterator type must dereference to ft_std_variant" );

    capability_request req;
    capability_response repl;

    req.set_msgid ( _msgid++ );
    req.set_devid ( devid );
    req.set_capname ( capname );

    fill_args ( req, begin_args,end_args );

    grpc::ClientContext ctxt;
    auto status = _capability_stub->InvokeCapability ( &ctxt,req,&repl );

    if ( status.ok() ) {
        return ft_variant_from_response ( repl );
    } else {
        _lg.strm(sl::error) << status.error_code() << ": " << status.error_message() ;
        throw std::runtime_error ( "GRPC Error" );
    }


}

template<typename... Args> ft_std_variant Client::InvokeCapability ( int devid, const std::string& capname, Args... args )
{
    std::vector<ft_std_variant> callargs{args...};
    return InvokeCapability ( devid, capname, callargs.begin(), callargs.end() );
};


template<typename containertp>
typename std::enable_if<detail2::has_const_iterator<containertp>::value, ft_std_variant>::type
Client::InvokeCapability ( int devid,const std::string& capname, containertp args )
{
    return InvokeCapability ( devid,capname,args.begin(),args.end() );
};

template <typename iteratortp> ft_vector_variant Client::FetchData ( int devid, const std::string& capname, unsigned dataid, unsigned chunksize, iteratortp begin_args, iteratortp end_args )
{
    static_assert ( std::is_same<typename std::iterator_traits<iteratortp>::value_type, ft_std_variant>::value,
                    "iterator type must dereference to ft_std_variant" );

    chunk_request req;
    req.set_devid ( devid );
    req.set_capname ( capname );

    req.set_msgid ( _msgid++ );
    req.set_chunksize ( chunksize );

    fill_args ( req, begin_args, end_args );

    grpc::ClientContext ctxt;
    auto reader = _capability_stub->FetchData ( &ctxt,req );

    std::vector<unsigned char> bytes;

    datachunk repl;
    while ( reader->Read ( &repl ) ) {
        auto thisdat = repl.data();
        bytes.insert ( bytes.end(),std::begin ( thisdat ), std::end ( thisdat ) );
        //TODO: error handling here!
        check_repl_err ( repl );
    }

    return ft_variant_from_data ( repl.dtp(),bytes );

}

template<typename... Args>
ft_vector_variant Client::FetchData ( int devid, const std::string& capname,
                                      unsigned dataid, unsigned chunksize, Args... args )
{
    std::vector<ft_std_variant> callargs{args...};
    return FetchData ( devid, capname, dataid, chunksize, callargs.begin(), callargs.end() );
}


}

