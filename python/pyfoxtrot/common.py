# -*- coding: utf-8 -*-

import base64
from pyfoxtrot.Errors import DeviceError, ProtocolError, ContentionError, ServerError, AuthenticationError, FoxtrotBaseError
from warnings import warn

def _check_repl_err(repl):
    if not repl.HasField('err'):
        return repl


    err_typ_map: dict[int, type] = { 0 : RuntimeError,
                                       1 : DeviceError,
                                     2 : ProtocolError,
                                     3 : ValueError,
                                     5: ContentionError,
                                     6 : ServerError,
                                     7 : AuthenticationError}

    err = repl.err
    if err.HasField("tp"):
        #This is an actual exception
        if err.tp in err_typ_map:
            outerrtp = err_typ_map[err.tp]
            if issubclass(outerrtp, FoxtrotBaseError):
                raise outerrtp(err.msg, repl)
            else:
                raise outerrtp(err.msg)
        else:
            raise RuntimeError(f"unknown error, message is: {err.msg}")


    warn_typ_map: dict[int, type] = { 0 : DeprecationWarning}
    if err.HasField("warntp"):
        warntp = warn_typ_map.get(err.warntp, UserWarning)
        warn(err.warnstring, warntp)



def decode_sodiumkey(key_in: str) -> bytes:
    return base64.standard_b64decode(key_in + "=====")

def encode_sodiumkey(key_in: str) -> bytes:
    return base64.standard_b64encode(key_in).rstrip(b"=")
