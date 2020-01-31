# -*- coding: utf-8 -*-

import base64
from pyfoxtrot.Errors import DeviceError, ProtocolError, ContentionError, ServerError, AuthenticationError

def _check_repl_err(repl):
    if not repl.HasField('err'):
        return repl

    if repl.err.tp == 0:
        raise RuntimeError(repl.err.msg)
    elif repl.err.tp == 1:
        raise DeviceError(repl.err.msg, repl)
    elif repl.err.tp == 2:
        raise ProtocolError(repl.err.msg, repl)
    elif repl.err.tp == 3:
        raise ValueError(repl.err.msg)
    elif repl.err.tp == 5:
        raise ContentionError(repl.err.msg, repl)
    elif repl.err.tp == 6:
        raise ServerError(repl.err.msg, repl)
    elif repl.err.tp == 7:
        raise AuthenticationError(repl.err.msg, repl)
    else:
        raise RuntimeError("unknown error")

def decode_sodiumkey(key_in: str) -> bytes:
    return base64.standard_b64decode(key_in + "=====")

def encode_sodiumkey(key_in: str) -> bytes:
    return base64.standard_b64encode(key_in).rstrip(b"=")