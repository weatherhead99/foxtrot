#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Apr 19 11:44:13 2017

@author: weatherill
"""
from functools import wraps
from .foxtrot_pb2_grpc import *
from .foxtrot_pb2 import *

DEFAULT_CHUNKSIZE=1000

def _capability_argument_from_value(val):
    arg = capability_argument()
    
    if isinstance(val,float):
        arg.dblarg = val
    elif isinstance(val,int):
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
    


class Client:
    def __init__(self,connstr):
        self._channel = grpc.insecure_channel(connstr)
        self._stub = exptserveStub(self._channel)
        
        self._servdescribe = self._stub.DescribeServer(empty())
        self._comment = self._servdescribe.servcomment
        
        self._devices = []
        
        for devkey in self._servdescribe.devs_attached.keys():
            dev = self._servdescribe.devs_attached.get(devkey)
            self._devices.append(Device(dev.devid, dev.devtype, dev.caps, dev.devcomment))            
            
            
    

class Device:
    def __init__(self,devid,devtp, caps,comment):
        self._devid = devid
        self._devtp = devtp
        self._comment = comment
        
        self._caps = []
        for cap in caps:
            self._caps.append(Capability(cap.tp,cap.capname,cap.argnames,cap.argtypes,cap.rettp))
        
    def __repr__(self):
        if len(self._comment )> 0:
            return str(self._devtp + ": " + self._comment)
        
        else:
            return str(self._devtp)
        

class Capability:
    def __init__(self,captp,capname,argnames,argtypes,rettp):
        self._captp = captp
        self._capname = capname
        self._argnames = argnames
        self._argtypes = argtypes
        self._rettp = rettp
        self._msgid = 0
        self.chunksize = DEFAULT_CHUNKSIZE
        
    def __repr__(self):
        if len(self._argnames) > 0:
            return self._capname + "(" + " ".join(self._argnames)  + ")" 
        
        else:
            return self._capname
        
    def construct_request(self,devid,*args,**kwargs):
        capargs = _construct_args(self._argnames,*args,**kwargs)
        
        reqtp = chunk_request if self._captp is STREAM else capability_request
        
        req = reqtp(msgid = self._msgid, devid = devid, capname = self._capname, args = capargs)        
        self._msgid +=1
        
        if self._capname is STREAM:
            req.chunksize = self.chunksize
            
        
        return req

        