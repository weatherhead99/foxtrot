# -*- coding: utf-8 -*-

        
def _check_repl_err(repl):
    if not repl.HasField('err'):
        return repl

    if repl.err.tp == 0:
        raise RuntimeError(repl.err.msg)
    elif repl.err.tp == 1:
        raise DeviceError(repl.err.msg)
    elif repl.err.tp == 2:
        raise ProtocolError(repl.err.msg)
    elif repl.err.tp == 3:
        raise ValueError(repl.err.msg)
    elif repl.err.tp == 5:
        raise ContentionError(repl.err.msg)
    elif repl.err.tp == 6:
        raise ServerError(repl.err.msg)
    elif repl.err.tp == 7:
        raise AuthenticationError(repl.err.msg)
    else:
        raise RuntimeError("unknown error")
