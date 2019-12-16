#pragma once
#include <exception>

#include <rttr/type>

#include <foxtrot/foxtrot.grpc.pb.h>
#include <foxtrot/ft_capability.pb.h>
#include <foxtrot/Logging.h>
#include <foxtrot/DeviceError.h>
#include <foxtrot/ProtocolError.h>
#include <foxtrot/ContentionError.h>
#include <foxtrot/ServerError.h>


namespace rttr
{
class variant;
}

namespace foxtrot
{
    
    void set_retval_from_variant(const rttr::variant& in, foxtrot::capability_response& resp, foxtrot::Logging* lg = nullptr);
    
    rttr::variant wire_arg_to_variant(const foxtrot::capability_argument& arg,
                                      bool& success,
                                      const rttr::type& target_tp,
                                      foxtrot::Logging* lg = nullptr);
    
    

class HandlerTag;
    
template <typename replT> void set_repl_err_msg ( replT& repl, const std::string& msg, error_types etype )
{
    auto errstat = repl.mutable_err();
    errstat->set_msg ( msg );
    errstat->set_tp ( etype );
}


template <typename replT, typename errT> void set_repl_err ( replT& repl, errT& err, error_types etype )
{
    auto errstat = repl.mutable_err();
    errstat->set_msg ( err.what() );
    errstat->set_tp ( etype );
}



template <typename retT, typename reqT> retT init_chunk ( reqT& req )
{
    retT out;
    out.set_msgid ( req.msgid() );
    out.set_devid ( req.devid() );
    out.set_capname ( req.capname() );

    return out;

}


template <typename T> struct empty_struct {

};


template<typename respondtp> struct responder_error_call {

};

template <typename T> struct responder_error_call<empty_struct<T>> {
    void operator() ( empty_struct<T>& responder, const T& repl, HandlerTag* tag )
    {

    };

};


template<typename T> struct responder_error_call<grpc::ServerAsyncResponseWriter<T>> {
    void operator() ( grpc::ServerAsyncResponseWriter<T>& responder, const T& repl, HandlerTag* tag )
    {
        responder.Finish ( repl, grpc::Status::OK, tag );
    };
};

template<typename T> struct responder_error_call<grpc::ServerAsyncWriter<T>> {
    void operator() ( grpc::ServerAsyncWriter<T>& responder, const T& repl, HandlerTag* tag )
    {
        responder.Write ( repl,tag );
    };
};


template <typename repltp, typename respondtp=empty_struct<repltp>> void foxtrot_rpc_error_handling ( std::exception_ptr eptr, repltp& repl, respondtp& responder, foxtrot::Logging& lg, HandlerTag* tag )
{
    responder_error_call<respondtp> errcall;

    try {
        if ( eptr ) {
            std::rethrow_exception ( eptr );
        }

    } catch ( class foxtrot::DeviceError& err ) {
        lg.Error ( "caught device error: " + std::string ( err.what() ) );
        set_repl_err ( repl,err,error_types::ft_DeviceError );
        errcall ( responder,repl,tag );
    } catch ( class foxtrot::ProtocolError& err ) {
        lg.Error ( "caught protocol error: " + std::string ( err.what() ) );
        set_repl_err ( repl,err,error_types::ft_ProtocolError );
        errcall ( responder,repl,tag );
    } catch ( class foxtrot::ContentionError& err ) {
        lg.Error ( "caught contention error: " + std::string ( err.what() ) );
        set_repl_err ( repl,err,error_types::contention_timeout );
        errcall ( responder,repl,tag );
    } catch ( class foxtrot::ServerError& err ) {
        lg.Error ( "caught server error: " + std::string ( err.what() ) );
        set_repl_err ( repl,err,error_types::ft_ServerError );
        errcall ( responder,repl,tag );
    } catch ( std::out_of_range& err ) {
        lg.Error ( "caught out of range error" );
        set_repl_err ( repl,err,error_types::out_of_range );
        errcall ( responder,repl,tag );
    } catch ( std::exception& err ) {
        lg.Error ( "caught general error: " + std::string ( err.what() ) );
        set_repl_err ( repl,err,error_types::ft_Error );
        errcall ( responder,repl,tag );
    } catch ( ... ) {
        lg.Error ( "caught otherwise unspecified exception" );
        set_repl_err_msg ( repl,"unknown error",error_types::unknown_error );
        errcall ( responder,repl,tag );
    }

};

template <typename repltp> void foxtrot_rpc_error_handling ( std::exception_ptr eptr, repltp& repl, foxtrot::Logging lg, HandlerTag* tag )
{
    empty_struct<repltp> responder;

    foxtrot_rpc_error_handling ( eptr,repl,responder,lg,tag );

};


template <typename repltp, typename respondtp> void foxtrot_server_specific_error ( const std::string& msg, repltp& repl, respondtp& respond, foxtrot::Logging& lg, HandlerTag* tag, error_types&& errtp=error_types::ft_Error )
{
    lg.Error ( msg );
    set_repl_err_msg ( repl,msg,errtp );
    responder_error_call<respondtp> errcall;
    errcall ( respond, repl, tag );

};


std::unique_ptr<unsigned char[]> byte_view_data ( rttr::variant& arr, unsigned& byte_size, foxtrot::byte_data_types& dtype );


// bool set_returntype ( rttr::variant& retval, foxtrot::capability_response& repl );
// value_types get_appropriate_wire_type ( const rttr::variant& vr );
// value_types get_appropriate_wire_type ( const rttr::type& tp );


template<typename T> bool is_ft_call_streaming ( const T& propmeth )
{
    auto streammeta = propmeth.get_metadata ( "streamdata" );
    if ( streammeta.is_valid() ) {
        if ( streammeta.to_bool() ) {
            return true;
        };

    };
    return false;
};

}
