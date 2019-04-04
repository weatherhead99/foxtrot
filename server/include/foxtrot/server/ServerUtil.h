#pragma once
#include <exception>

#include <rttr/type>

#include <foxtrot/foxtrot.grpc.pb.h>
#include <foxtrot/Logging.h>
#include <foxtrot/DeviceError.h>
#include <foxtrot/ProtocolError.h>
#include <foxtrot/ContentionError.h>
#include <foxtrot/ServerError.h>

#include "HandlerBase.h"


namespace rttr
{
class variant;
}

namespace foxtrot
{
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

rttr::variant get_arg ( const capability_argument& arg, bool& success );

bool set_returntype ( rttr::variant& retval, foxtrot::capability_response& repl );
value_types get_appropriate_wire_type ( const rttr::variant& vr );
value_types get_appropriate_wire_type ( const rttr::type& tp );

template <typename reqtp, typename repltp> std::vector<rttr::variant> get_callargs ( rttr::method& meth,
        reqtp& req,
        repltp& repl )
{
    foxtrot::Logging lg ( "get_callargs" );


    auto args = req.args();
    std::vector<rttr::variant> argvec;
    argvec.resize ( args.size() );

    //check parameter infos
    auto param_infs = meth.get_parameter_infos();

    if ( args.size() != param_infs.size() ) {
        lg.Error ( "unexpected number of arguments supplied" );
        errstatus* errstat = repl.mutable_err();
        errstat->set_msg ( "incorrect number of arguments supplied" );
        errstat->set_tp ( error_types::out_of_range );

        throw 1;

    }

#ifdef NEW_RTTR_API
    auto paraminfsit = param_infs.begin();
#endif

    for ( auto& arg: args ) {
        bool success;
        rttr::variant outarg = get_arg ( arg,success );

        if ( !outarg.is_valid() ) {
            lg.Error ( "variant is invalid" );
            throw std::logic_error ( "variant is invalid! in get_callargs" );

        };


        if ( !success ) {
            lg.Error ( "error in getting arguments..." );
            auto msg = "argument at position " + std::to_string ( arg.position() ) + "is not set";
            set_repl_err_msg ( repl,msg,error_types::ft_Error );
            throw 1;
        }
#ifdef NEW_RTTR_API
        const auto target_argtp = ( paraminfsit++ )->get_type();
#else
        const auto target_argtp = param_infs[arg.position()].get_type();
#endif
        lg.strm ( sl::debug ) << "can convert? : " << ( int ) outarg.can_convert ( target_argtp );

        if ( !outarg.can_convert ( target_argtp ) ) {
            lg.Error ( "error converting argument " );
            auto msg = "argument at position " + std::to_string ( arg.position() ) + " of type: " + outarg.get_type().get_name()
                       + "cannot be converted to type " + target_argtp.get_name();
            lg.Debug ( msg );
            set_repl_err_msg ( repl,msg, error_types::out_of_range );
            throw 1;

        }

        if ( outarg.get_type() != target_argtp ) {
            lg.strm ( sl::debug ) << " need to convert an arg from: " << outarg.get_type().get_name() << " to " << target_argtp.get_name();

            lg.strm ( sl::debug ) << "arg value (int): " << outarg.to_int() ;
        };

        //NOTE: outarg.convert returns bool, f*ck you RTTR that is NOT obvious!

        success = outarg.convert ( target_argtp );

        if ( !success ) {
            lg.strm ( sl::error ) << "failed to convert arg..." ;
            throw std::logic_error ( "failed to convert arg" );
        };

        lg.strm ( sl::debug ) << "arg type is now: " << outarg.get_type().get_name();

        argvec[arg.position()] = outarg;
    };

    for ( auto& arg : argvec ) {
        bool success;
        auto str = arg.to_string ( &success );
        if ( !success ) {
            lg.Error ( "couldn't get string whil trying to print arg" );
            break;
        }

        lg.Trace ( "arg: " + str );

    }

    return argvec;
}


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
