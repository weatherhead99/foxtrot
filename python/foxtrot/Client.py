#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Apr 19 11:44:13 2017

@author: weatherill
"""
from functools import wraps
from foxtrot.foxtrot_pb2_grpc import *
from foxtrot.foxtrot_pb2 import *
from foxtrot.Errors import *

import types
import struct

DEFAULT_CHUNKSIZE=1000

value_type_to_return_field_dict = {FLOAT_TYPE : ('dblret', float),
                                   INT_TYPE   : ('intret',int),
                                    BOOL_TYPE : ('boolret',bool),
                                  STRING_TYPE : ('stringret',bytes)
}

def _capability_argument_from_value(val):
    arg = capability_argument()
    
    if isinstance(val,float):
        arg.dblarg = val
    #not isinstance(True,int) is True, so using 
    #isinstance() here false positives bool values
    elif type(val) is int:
        arg.intarg = val
    elif isinstance(val,bool):
        arg.boolarg = val
    elif isinstance(val,str):
        arg.strarg = val.encode("ASCII")
    elif isinstance(val,bytes):
        arg.strarg = val

    return arg        

def _fill_capability_args(args):
    out = []
    for idx,arg in enumerate(args):
        a = _capability_argument_from_value(arg)
        a.position = idx
        out.append(a)
    return out


def _construct_args(argnames, *args,**kwargs):
    rawargs = [None] * len(argnames)
    for name, val in kwargs.items():
        if name not in argnames:
            raise KeyError("passed an unrecognized keyword arg")
        pos = argnames.index(name)
        rawargs[pos] = val

    
    if len(args) + len(kwargs) > len(argnames):
        raise ValueError("too many arguments provided")
        
    for idx,val in enumerate(args):
        if rawargs[idx] is not None:
            raise IndexError("conflicting positional and keyword arguments")
        rawargs[idx] = val
    
    if any( _ is None for _ in rawargs ):
        raise IndexError("not all arguments filled in")
        
    return _fill_capability_args(rawargs)
    

def _process_sync_response(repl,streamraw=False):
    if hasattr(repl,"__iter__"):
        rawbytes = bytearray()
        
        for chunk in repl:
            _check_repl_err(chunk)
            dtp = chunk.dtp
            rawbytes += chunk.data
                        
        if streamraw:
            return rawbytes
            
        if dtp is UCHAR_TYPE:
            print("UCHAR")
            return list(rawbytes)
        elif dtp is USHORT_TYPE:
            structstr = '<%dH' % (len(rawbytes) // 2)
        
        elif dtp is UINT_TYPE:
            structstr = '<%dI' % (len(rawbytes) // 4)
            
        elif dtp is BDOUBLE_TYPE:
            print("BDOUBLE")
            structstr = '<%dd' % (len(rawbytes) // 8)
        
        else:
            raise NotImplementedError("data type not supported yet")
        
        return list(struct.unpack(structstr,rawbytes))
        
        
    else:
        _check_repl_err(repl)
        whichattr = repl.WhichOneof("return")
        if whichattr is None:
            return
        ret = getattr(repl,whichattr)
        if whichattr == "stringret" and len(ret) == 0:
            return None
        return ret
        
        
        
def _check_repl_err(repl):
    if not repl.HasField('err'):
        return

    if repl.err.tp == 0:
        raise RuntimeError(repl.err.msg)
    elif repl.err.tp == 1:
        raise DeviceError(repl.err.msg)
    elif repl.err.tp == 2:
        raise ProtocolError(repl.err.msg)
    elif repl.err.tp == 3:
        raise ValueError(repl.err.msg)
    elif repl.err.tp == 5:
        raise ContentionError(repl.err.msg)
    else:
        raise RuntimeError("unknown error")
        
def _reinterpret_cast_bytes(bts,tp):
    pass


class Client:
    def __init__(self,connstr,certfile=None):
        if certfile is None:
            self._channel = grpc.insecure_channel(connstr)
        else:
            with open(certfile,"rb") as f:
                cert = f.read()
            creds = grpc.ssl_channel_credentials(root_certificates=cert)
            self._channel = grpc.secure_channel(connstr,creds)
        self._stub = exptserveStub(self._channel)
        
        self._servdescribe = self._stub.DescribeServer(empty())
        self._comment = self._servdescribe.servcomment
        
        self._setup_device_tree()
        
    def _setup_device_tree(self):
        self._devices = []
        
        for devkey in self._servdescribe.devs_attached.keys():
            dev = self._servdescribe.devs_attached.get(devkey)
            self._devices.append(Device(dev.devid, dev.devtype, dev.caps, dev.devcomment,self))            
        
        
        for dev in self._devices:
            if len(dev._comment) > 0:
                setattr(self,dev._comment,dev)
            else:
                setattr(self,dev._devtp,dev)
    
    def save_servdesc(self,fname):
        ss = self._servdescribe.SerializeToString()
        with open(fname,'wb') as f:
            f.write(ss)
        
    def __getitem__(self,keystr):
        if not hasattr(self,"_devtypes"):
            self._devtypes = [_._devtp for _ in self._devices]
            self._devcomments = [_._comment for _ in self._devices]
            self._devids = [_._devid for _ in self._devices]
            
        if isinstance(keystr,int):
            return self.devices[self._devids.index(keystr)]
        
        else:
            if keystr in self._devcomments:
                return self._devices[self._devcomments.index(keystr)]

            return self._devices[self._devtypes.index(keystr)]    


def _fake_call_sync(obj,client,*args,**kwargs):
    req = obj.construct_request(*args,**kwargs)
    
    if len(args) + len(kwargs) != len(req.args):
        raise IndexError("incorrect number of arguments to dummy func")
    
    repl = datachunk() if obj._captp is STREAM else capability_response()
    
    repl.msgid = req.msgid
    repl.capname = req.capname
    repl.devid = req.devid
    
    if obj._captp is STREAM:
        pass    
    else:
        if obj._rettp in value_type_to_return_field_dict:
            attrname,tp = value_type_to_return_field_dict[obj._rettp]
            setattr(repl,attrname,tp())
        
    return repl
    

class DummyClient(Client):
    def __init__(self,sdfname):
        with open(sdfname,'rb') as f:
            ss = f.read()
        self._servdescribe = servdescribe()
        self._servdescribe.ParseFromString(ss)
        self._setup_device_tree()
        
        for dev in self._devices:
            for cap in dev._caps:
                cap.call_cap_sync = types.MethodType(_fake_call_sync,cap)
     

class Device:
    def __init__(self,devid,devtp, caps,comment,client):
        self._devid = devid
        self._devtp = devtp
        self._comment = comment
        self._cl = client
        
        self._caps = []
        for cap in caps:
            self._caps.append(Capability(cap.tp,cap.capname,cap.argnames,cap.argtypes,cap.rettp,devid,client))
        
        for cap in self._caps:
            setattr(self,cap._capname,cap)
        
    def __repr__(self):
        if len(self._comment )> 0:
            return str(self._devtp + ": " + self._comment)
        
        else:
            return str(self._devtp)
        
            

class Capability:
    def __init__(self,captp,capname,argnames,argtypes,rettp,devid,client):
        self._captp = captp
        self._capname = capname
        self._argnames = argnames
        self._argtypes = argtypes
        self._rettp = rettp
        self._msgid = 0
        self.chunksize = DEFAULT_CHUNKSIZE
        self._devid = devid
        self._cl = client
        
    def __repr__(self):
        if self._captp is VALUE_READONLY :
            infostr = " (readonly value)"
        elif self._captp is VALUE_READWRITE:
            infostr = " (read/write value)"
        elif self._captp is ACTION:
            infostr = " (action)"
        else:
            infostr = " (data stream)"
        
        if len(self._argnames) > 0:
            return self._capname + "(" + " ".join(self._argnames)  + ")"  + infostr
        
        else:
            return self._capname + infostr
        
    def construct_request(self,*args,**kwargs):
        if self._captp is not VALUE_READWRITE:
            capargs = _construct_args(self._argnames,*args,**kwargs)
        elif len(args) > 0 or len(kwargs) > 0:
            capargs = _construct_args(self._argnames,*args,**kwargs)
        else:
            capargs = _construct_args([],*args,**kwargs)
            
        
        reqtp = chunk_request if self._captp is STREAM else capability_request
        
        req = reqtp(msgid = self._msgid, devid = self._devid, capname = self._capname, args = capargs)        
        self._msgid +=1
        
        if self._captp is STREAM:
            req.chunksize = self.chunksize
            
        return req
    
    def call_cap_sync(self,client,*args,**kwargs):
        stubfun = client._stub.FetchData if self._captp is STREAM else client._stub.InvokeCapability
        ret = stubfun(self.construct_request(*args,**kwargs))
        
        return ret
        
    def __call__(self,*args,**kwargs):
        repl = self.call_cap_sync(self._cl,*args,**kwargs)
        return _process_sync_response(repl)
    
class ServerFlag:
    def __init__(self,client,flagname):
        self._flagname = flagname
        self._client = client

    def construct_request(self,flagname, val=0):
        req = serverflag()
        req.msgid = 0
        req.flagname = flagname
        if isinstance(val,float):
            req.dblval = val
        elif isinstance(val,bool):
            req.boolval = val
        elif isinstance(val,int):
            req.intval = val
        elif isinstance(val,str):
            req.stringval = val.encode("ASCII")
        else:
            raise TypeError("invalid value type %s for server flag" % type(val))
        
        return req

    @property
    def value(self):
        req = self.construct_request(self._flagname)
        stubfun = self._client._stub.GetServerFlag
        ret = stubfun(req)
        _check_repl_err(ret)
        whichattr = ret.WhichOneof("arg")
        if whichattr is None:
            return None
        return getattr(ret,whichattr)

    @value.setter
    def value(self,val):
        req = self.construct_request(self._flagname,val)
        stubfun = self._client._stub.SetServerFlag
        repl = stubfun(req)
        _check_repl_err(repl)
        whichattr = repl.WhichOneof("arg")
        if whichattr is None:
            return None
        return getattr(repl,whichattr)    

