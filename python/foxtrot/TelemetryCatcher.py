#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Apr 19 17:47:55 2017

@author: weatherill
"""

from .foxtrot_pb2 import telemetry
import nanomsg

from datetime import datetime, timedelta

EPOCH = datetime.fromtimestamp(0)

def dt_from_u64(val):
    since_epoch = timedelta(0,0,val)
    out = EPOCH + since_epoch
    return out

class TelemetryCatcher:
    def __init__(self,connstr, topic):
#        if not isinstance(connstr, bytes):
#            connstr = connstr.encode("ASCII")
#            
        self._sock = nanomsg.Socket(nanomsg.SUB)
        self._sock.connect(connstr)
#        if not isinstance(topic,bytes):
#            topic = topic.encode("ASCII")
        self._topic = topic
        self._subscriptions = {}
        
        
    def subscribe(self,subtopic, callback):
#        if not isinstance(subtopic,bytes):
#            subtopic = subtopic.encode("ASCII")
#            
#        substr = self._topic + b'|' + subtopic
        substr = self._topic + '|' + subtopic
        print(substr)
        self._sock.set_string_option(nanomsg.SUB, 
                                     nanomsg.SUB_SUBSCRIBE,
                                     substr)
        
        self._subscriptions[substr] = callback
    
    def waitmessage(self):
        msg = self._sock.recv()
        splitpoint = msg.index(b'>')
        
        topic = msg[:splitpoint]
        protobuf = msg[splitpoint+1:]
        print(topic)
        
        t = telemetry()
        t.ParseFromString(protobuf)        
        retattr = t.WhichOneof('return')
        return (topic, dt_from_u64(t.tstamp), getattr(t,retattr))
    

    def process_telems_forever(self):
        while True:
            topic, tstamp, ret = self.waitmessage()
            key = topic.decode("ASCII")
            if key in self._subscriptions:
                fun = self._subscriptions[key]
                print("calling function...")
                fun(topic,tstamp,ret)
            
            
    
    def __del__(self):
        self._sock.close()
        
        