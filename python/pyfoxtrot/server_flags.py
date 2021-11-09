#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Jul 16 15:04:58 2019

@author: weatherill
"""

#from __future__ import annotations
from pyfoxtrot.common import _check_repl_err
from pyfoxtrot.protos.ft_types_pb2 import  empty
from pyfoxtrot.protos.ft_flags_pb2 import serverflag

class ServerFlag:
    def __init__(self, client, flagname: str):
        self._flagname = flagname
        self._client = client

    @staticmethod
    def construct_request(flagname: str, val=0): #-> serverflag:
        req = serverflag()
        req.msgid = 0
        req.flagname = flagname
        if isinstance(val, float):
            req.dblval = val
        elif isinstance(val, bool):
            req.boolval = val
        elif isinstance(val, int):
            req.intval = val
        elif isinstance(val, str):
            req.stringval = val.encode("ASCII")
        else:
            raise TypeError("invalid value type %s for server flag" % type(val))

        return req

    def __repr__(self) -> str:
        tval = self.value
        tempstr = "ServerFlag( %s : %s (%s))"
        return tempstr % (self._flagname, tval, type(tval).__name__)

    @property
    def name(self) -> str:
        return self._flagname

    def drop(self) -> None:
        req = serverflag()
        req.flagname = self._flagname
        repl = self._client._flagstub.DropServerFlag(req)
        _check_repl_err(repl)

    @property
    def value(self):
        req = self.construct_request(self._flagname)
        ret = self._client._flagstub.GetServerFlag(req)
        _check_repl_err(ret)
        whichattr = ret.WhichOneof("arg")
        if whichattr is None:
            return None
        return getattr(ret, whichattr)

    @value.setter
    def value(self, val):
        req = self.construct_request(self._flagname, val)
        repl = self._client._flagstub.SetServerFlag(req)
        _check_repl_err(repl)
        whichattr = repl.WhichOneof("arg")
        if whichattr is None:
            return None
        return getattr(repl, whichattr)

class FlagProxy:
    def __init__(self, cl):
        self._cl = cl

    def __getitem__(self, flagname: str):
        flg = ServerFlag(self._cl, flagname)
        return flg

    def __setitem__(self, flagname: str, val) -> None:
        flg = ServerFlag(self._cl, flagname)
        flg.value = val

    def __iter__(self):
        response = self._cl._flagstub.ListServerFlags(empty())
        _check_repl_err(response)
        self._flaglist = [ServerFlag(self._cl, _.flagname) for _ in response.flags]
        self._flagit = iter(self._flaglist)
        return self

    def __next__(self):
        return next(self._flagit)
