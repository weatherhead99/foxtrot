#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Apr 19 17:49:43 2017

@author: weatherill
"""

import asyncio
import os
os.environ['LD_LIBRARY_PATH'] = '/local/home/weatherill/lib/'


from foxtrot.TelemetryCatcher import TelemetryCatcher, dt_from_u64

tc = TelemetryCatcher("tcp://127.0.0.1:50052","test_telem")
#tc.subscribe("counter", lambda x : 0)

def print_telem(topic,tstamp,ret):
    print("topic: " + topic.decode("ASCII"))
    print("timestamp: " + str(tstamp))
    print("ret: " + str(ret))


tc.subscribe("counter",print_telem)
tc.subscribe("randomdouble",print_telem)
    
#testmsg = tc.waitmessage()
tc.process_telems_forever()


