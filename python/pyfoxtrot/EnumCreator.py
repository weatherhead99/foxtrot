#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jul 18 12:54:38 2019

@author: weatherill
"""

from enum import IntEnum
from .protos.ft_types_pb2 import enum_descriptor

def define_enum(desc: enum_descriptor):
    newtypename = desc.enum_name.replace("::","_")
    tp = IntEnum(newtypename, dict(desc.enum_map.items()))

    return tp
