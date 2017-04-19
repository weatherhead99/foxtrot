#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Apr 19 17:47:55 2017

@author: weatherill
"""

from .foxtrot_pb2 import telemetry
import nanomsg

import asyncio

class TelemetryCatcher:
    def __init__(self,connstr, topic):
        if not isinstance(connstr, bytes):
            connstr = connstr.encode("ASCII")
            
        self._sock = nanomsg.Socket(nanomsg.SUB)
        self._sock.connect(connstr)
        if not isinstance(topic,bytes):
            topic = topic.encode("ASCII")
        self._topic = topic
        self._subscriptions = {}
        
        
    def subscribe(self,subtopic, callback):
        if not isinstance(subtopic,bytes):
            subtopic = subtopic.encode("ASCII")
            
        substr = self._topic + b'|' + subtopic
        print(substr)
        self._sock.set_string_option(nanomsg.SUB, 
                                     nanomsg.SUB_SUBSCRIBE,
                                     substr)
        
        self._subscriptions[subtopic] = callback
    
    def messagegen(self):
        msg = self._sock.recv()
        return msg
    
    def __del__(self):
        self._sock.close()
        
        