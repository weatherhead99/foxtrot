#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Apr 19 11:44:13 2017

@author: weatherill
"""

import struct
import grpc
from itertools import chain
from .protos.foxtrot_pb2_grpc import exptserveStub
from .protos.ft_capability_pb2_grpc import capabilityStub
from .protos.ft_flags_pb2_grpc import flagsStub
from .common import _check_repl_err
from .server_flags import ServerFlag, FlagProxy
from .TypeConversion import ft_variant_from_value, value_from_ft_variant
from .TypeConversion import string_describe_ft_variant
from .protos.ft_types_pb2 import empty
from .protos.foxtrot_pb2 import  broadcast_notification
from .protos.ft_capability_pb2 import capability_request, capability_argument, VALUE_READONLY, VALUE_READWRITE, ACTION, STREAM
from .protos.ft_types_pb2 import UCHAR_TYPE, USHORT_TYPE, UINT_TYPE, BDOUBLE_TYPE, IINT_TYPE
from .protos.ft_capability_pb2 import chunk_request
from .protos.ft_types_pb2 import ENUM_TYPE
from .EnumCreator import define_enum


DEFAULT_CHUNKSIZE = 1000

class Client:
    def __init__(self, connstr: str, certfile: str = None) -> None:
        if certfile is None:
            self._channel = grpc.insecure_channel(connstr)
        else:
            with open(certfile, "rb") as f:
                cert = f.read()
            creds = grpc.ssl_channel_credentials(root_certificates=cert)
            self._channel = grpc.secure_channel(connstr, creds)
        self._stub = capabilityStub(self._channel)
        self._flagstub = flagsStub(self._channel)
        self._estub = exptserveStub(self._channel)

        self._servdescribe = _check_repl_err(self._stub.DescribeServer(empty()))
        self._comment = self._servdescribe.servcomment

        self._enum_descs = []
        self._enum_types = []
        self._active_session = None
        self._setup_device_tree()

    def _add_enum_type(self, enumdesc) -> None:
        if enumdesc not in self._enum_descs:
            self._enum_descs.append(enumdesc)
            self._enum_types.append(define_enum(enumdesc))

    def _lookup_enum_type(self, enumdesc):
        if enumdesc in self._enum_descs:
            idx = self._enum_descs.index(enumdesc)
            return self._enum_types[idx]
        return None

    def _setup_device_tree(self) -> None:
        self._devices = []

        for devkey in self._servdescribe.devs_attached.keys():
            dev = self._servdescribe.devs_attached.get(devkey)
            self._devices.append(Device(dev.devid, dev.devtype, dev.caps,
                                        dev.devcomment, self))


        for dev in self._devices:
            if dev._comment:
                setattr(self, dev._comment, dev)
            else:
                setattr(self, dev._devtp, dev)

    def save_servdesc(self, fname):
        ss = self._servdescribe.SerializeToString()
        with open(fname, 'wb') as f:
            f.write(ss)

    def __getitem__(self, keystr):
        if not hasattr(self, "_devtypes"):
            self._devtypes = [_._devtp for _ in self._devices]
            self._devcomments = [_._comment for _ in self._devices]
            self._devids = [_._devid for _ in self._devices]

        if isinstance(keystr, int):
            return self._devices[self._devids.index(keystr)]

        if keystr in self._devcomments:
            return self._devices[self._devcomments.index(keystr)]

        return self._devices[self._devtypes.index(keystr)]

    def get_all_flags_dict(self) -> dict:
        return {_.name : _ for _ in self.flags}

    def get_all_flags_list(self) -> list:
        return [_ for _ in self.flags]


    def create_new_flag(self, name: str, initval):
        sf = ServerFlag(self, name)
        sf.value = initval
        return sf

    def drop_server_flag(self, flag) -> None:
        if isinstance(flag, str):
            ServerFlag(self, flag).drop()
        elif isinstance(flag, ServerFlag):
            flag.drop()

    def broadcast_notification(self, body: str, title: str = None, channel: str = None):
        req = broadcast_notification(body=body)
        if title is None:
            req.use_default_title = True
            req.title = ""
        else:
            req.title = title

        if channel is None:
            req.use_default_channel = True
            req.channel_target = ""
        else:
            req.channel_target = channel

        repl = self._estub.BroadcastNotification(req)
        _check_repl_err(repl)

    @property
    def flags(self):
        return FlagProxy(self)



class Device:
    def __init__(self, devid, devtp, caps, comment, client):
        self._devid = devid
        self._devtp = devtp
        self._comment = comment
        self._cl = client

        self._caps = []
        self._props = {}
        for cap in caps:
            self._caps.append(Capability(cap.tp, cap.capname, cap.argnames,
                                         cap.argtypes, cap.rettp, devid, client))

        for cap in self._caps:
            setattr(self, cap._capname, cap)
            if(cap._captp == VALUE_READONLY or cap._captp == VALUE_READWRITE):
                propname = "value_%s" % cap._capname
                fget = lambda s: getattr(s,propname)()
                fset = lambda s, val: getattr(s,propname)(val)
                self._props[propname] = property(fget, fset)

    def __repr__(self):
        if self._comment:
            return str("Device(" + self._devtp + ": " + self._comment + ")")

        return str("Device(" + self._devtp + ")")



class Capability:
    def __init__(self, captp, capname, argnames, argtypes, rettp, devid, client):
        self._captp = captp
        self._capname = capname
        self._argnames = argnames
        self._argtypes = argtypes
        self._rettp = rettp
        self._msgid = 0
        self.chunksize = DEFAULT_CHUNKSIZE
        self._devid = devid
        self._cl = client
        self._enum_return_type = None
        self._enum_arg_types = [None] * len(argtypes)
        
        for tp in chain([rettp], argtypes):
            if tp.variant_type == ENUM_TYPE:
                client._add_enum_type(tp.enum_desc)
                
        if rettp.variant_type == ENUM_TYPE:
            self._enum_return_type = client._lookup_enum_type(rettp.enum_desc)
            
        for idx,var in enumerate(argtypes):
            if var.variant_type == ENUM_TYPE:
                self._enum_arg_types[idx] = client._lookup_enum_type(var.enum_desc)
    
    def __repr__(self):
        if self._captp == VALUE_READONLY:
            infostr = "readonly value"
        elif self._captp == VALUE_READWRITE:
            infostr = "read/write value"
        elif self._captp == ACTION:
            infostr = "action"
        else:
            infostr = "data stream"

        argnamestrs = map(lambda s : "unknown" if not s else s , self._argnames)
        argtypestrs = [string_describe_ft_variant(_) for _ in self._argtypes]
        rettypestr = string_describe_ft_variant(self._rettp)
        
        argnametypestrs = ["%s:%s" % (n,t) for n,t in zip(argnamestrs,argtypestrs)]
        
        displaystr = "%s (%s) -> %s, [%s]" % (self._capname, 
                      ", ".join(argnametypestrs),
                      rettypestr,
                      infostr)
        

        return displaystr

    def get_enum_type(self, argpos):
        if isinstance(argpos, int):
            return self._enum_arg_types[argpos]
        elif isinstance(argpos, str):
            idx = self._argnames.index(argpos)
            return self._enum_arg_types[idx]
        
    def get_enum(self, argpos, *args, **kwargs):
        tp = self.get_enum_type(argpos)
        return tp(*args, **kwargs)

    def _construct_args(self, *args, **kwargs):
        rawargs = [None] * len(self._argnames)
        for name, val in kwargs.items():
            if name not in self._argnames:
                raise ValueError("no such argument with name: %s in capability"
                                 % name)
            pos = self._argnames.index(name)
            argdesc = self._argtypes[pos]
            rawargs[pos] = capability_argument(
                pos=pos,
                value=ft_variant_from_value(val, argdesc))

        if (len(args) + len(kwargs)) > len(rawargs):
            raise ValueError("too many arguments provided")

        for idx, val in enumerate(args):
            if rawargs[idx] is not None:
                raise IndexError("conflicting positional and keyword arguments")
            desc = self._argtypes[idx]
            rawargs[idx] = capability_argument(position=idx,
                                               value=ft_variant_from_value(val, desc))

        if any(_ is None for _ in rawargs):
            raise IndexError("not all arguments filled in")

        return rawargs

    def _process_sync_response(self, repl):
        if self._captp == STREAM:
            rawbytes = bytearray()

            for chunk in repl:
                _check_repl_err(chunk)
                dtp = chunk.dtp
                rawbytes += chunk.data

            if dtp == UCHAR_TYPE:
                return list(rawbytes)
            if dtp == USHORT_TYPE:
                structstr = '<%dH' % (len(rawbytes) // 2)
            elif dtp == UINT_TYPE:
                structstr = '<%dI' % (len(rawbytes) // 4)
            elif dtp == IINT_TYPE:
                structstr = '<%di' % (len(rawbytes) // 4)
            elif dtp == BDOUBLE_TYPE:
                structstr = '<%dd' % (len(rawbytes) // 8)
            else:
                raise NotImplementedError("data type not supported yet")
            return list(struct.unpack(structstr, rawbytes))

        _check_repl_err(repl)
        out = value_from_ft_variant(repl.returnval)
        return out

    def construct_request(self, *args, **kwargs):
        if self._captp != VALUE_READWRITE:
            capargs = self._construct_args(*args, **kwargs)
        elif len(args) > 0 or len(kwargs) > 0:
            capargs = self._construct_args(*args, **kwargs)
        else:
            capargs = []

        reqtp = chunk_request if self._captp == STREAM else capability_request

        req = reqtp(msgid=self._msgid, devid=self._devid,
                    capname=self._capname, args=capargs)
        self._msgid += 1

        if self._captp == STREAM:
            req.chunksize = self.chunksize

        return req

    def call_cap_sync(self, client, *args, **kwargs):
        stubfun = client._stub.FetchData if self._captp == STREAM else client._stub.InvokeCapability
        req = self.construct_request(*args, **kwargs)
        if client._active_session is None:
            ret = stubfun(req)
        else:
            print("session active using metadata")
            metadata = [("session_secret-bin", client._active_session._secret)]
            ret,status = stubfun.with_call(request=req, metadata=metadata)
        
        return ret

    def __call__(self, *args, **kwargs):
        repl = self.call_cap_sync(self._cl, *args, **kwargs)
        return self._process_sync_response(repl)
