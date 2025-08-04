#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jul 18 12:54:38 2019

@author: weatherill
"""

from enum import IntEnum
from .protos.ft_types_pb2 import enum_descriptor

_ENUM_REGISTRY : dict[str, type[IntEnum]] = {}

def define_enum(desc: enum_descriptor):
    newtypename = desc.enum_name.replace("::","_")
    tp = IntEnum(newtypename, dict(desc.enum_map.items()))

    return tp

def get_enum_type(desc: enum_descriptor) -> type[IntEnum]:
    global _ENUM_REGISTRY
    enumname: str = desc.enum_name
    
    if enumname not in _ENUM_REGISTRY:
        _ENUM_REGISTRY[desc.enum_name] = define_enum(desc)
    return _ENUM_REGISTRY[desc.enum_name]
