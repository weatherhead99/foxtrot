#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Apr 19 12:05:26 2017

@author: weatherill
"""

from pyfoxtrot.Client import Client
import pyfoxtrot.foxtrot_pb2 


cl = Client("localhost:50051")
#cl = DummyClient("/home/weatherill/Software/OPMD_acq/servdesc.ftsd")

dev = cl._devices[0]

addcap = dev._caps[7]
addret = addcap.call_cap_sync(cl,1,2)

strmcap = dev._caps[10]
strmret = strmcap.call_cap_sync(cl,100)


counts = cl.dummy1.getCountStream(5000)



repl = dev.getRandomVector.call_cap_sync(cl,100)
bts = foxtrot.Client._process_sync_response(repl,streamraw=True)

vec = cl.dummyDevice.getRandomVector(cl,5000)

