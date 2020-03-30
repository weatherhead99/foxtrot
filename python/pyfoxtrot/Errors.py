#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed May  3 22:34:46 2017

@author: danw
"""

class FoxtrotBaseError(Exception):
    def __init__(self, msg, raw_reply=None):
        super().__init__(msg)
        self.raw_reply = raw_reply

class DeviceError(FoxtrotBaseError):
    pass

class ProtocolError(FoxtrotBaseError):
    pass

class ContentionError(FoxtrotBaseError):
    pass

class ServerError(FoxtrotBaseError):
    pass

class AuthenticationError(FoxtrotBaseError):
    pass
