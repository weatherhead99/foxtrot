#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed May  3 22:34:46 2017

@author: danw
"""

class DeviceError(Exception):
    pass

class ProtocolError(Exception):
    pass

class ContentionError(Exception):
    pass

class ServerError(Exception):
    pass

class AuthenticationError(Exception):
    pass
