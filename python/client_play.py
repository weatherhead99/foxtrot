#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Apr 19 12:05:26 2017

@author: weatherill
"""

from foxtrot.Client import Client
import foxtrot.foxtrot_pb2 


cl = Client("localhost:50051")

dev = cl._devices[0]

cap = dev._caps[5]


ags = foxtrot.Client._construct_args(cap._argnames,1,2)
