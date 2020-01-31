import grpc

from pyfoxtrot.ft_sessions_pb2_grpc import sessionsStub
from pyfoxtrot.ft_sessions_pb2 import session_info
from pyfoxtrot.Client import Client
from pyfoxtrot.common import _check_repl_err, decode_sodiumkey
from pyfoxtrot.Errors import ServerError
from datetime import datetime

class SessionConflictError(ServerError):
    @classmethod
    def _from_raw_error(cls, err: ServerError):
        obj = cls(*err.args)
        obj.owner = err.raw_reply.user_identifier
        obj.comment = err.raw_reply.comment
        return obj

class Session:
    @classmethod
    def _from_create_session_repl(cls, repl: session_info):
        obj = cls()
        obj._userid = repl.user_identifier
        obj._comment = repl.comment
        obj._secret = decode_sodiumkey(repl.sessionid)
        obj._expiry = datetime.fromtimestamp(repl.expiry.seconds)
        obj.raw_repl = repl
        return obj
        
    @property
    def userid(self):
        return self._userid
    
    @property
    def comment(self):
        return self._comment
    
    @property
    def expiry(self):
        return self._expiry

class SessionManager:
    def __init__(self, cl: Client):
        self._stub = sessionsStub(cl._channel)
    
    def _start_session(self, userid: str, comment: str, devices=None,
                        flags=None) -> Session:
        req = session_info(user_identifier=userid,
                           comment=comment)
        
        if devices is not None:
            req.devices.extend([_._devid for _ in devices])
        if flags is not None:
            req.flags.extend([_._flagname for _ in flags])
        
        repl = self._stub.StartSession(req)
        
        try:
            _check_repl_err(repl)
        except ServerError as err:
            sce = SessionConflictError._from_raw_error(err)
            raise sce
            
        return Session._from_create_session_repl(repl)

if __name__ == "__main__":
    cl = Client("localhost:50051")
    
    sm = SessionManager(cl)
    ses = sm._start_session("danw", "comment1", [cl.dummy1])
        