import grpc

from pyfoxtrot.ft_sessions_pb2_grpc import sessionsStub
from pyfoxtrot.ft_sessions_pb2 import session_info, session_list
from pyfoxtrot.ft_types_pb2 import empty
from pyfoxtrot.Client import Client
from pyfoxtrot.common import _check_repl_err, decode_sodiumkey, encode_sodiumkey
from pyfoxtrot.Errors import ServerError
from datetime import datetime, timedelta
from time import sleep

class SessionConflictError(ServerError):
    @classmethod
    def _from_raw_error(cls, err: ServerError):
        obj = cls(*err.args)
        obj.owner = err.raw_reply.user_identifier
        obj.comment = err.raw_reply.comment
        return obj

class Session:
    @classmethod
    def _from_repl(cls, repl: session_info, cl: Client = None, sm= None,
                   interactive: bool=True):
        obj = cls()
        obj._userid = repl.user_identifier
        obj._comment = repl.comment
        obj._secret = repl.sessionid
        obj._expiry = datetime.fromtimestamp(repl.expiry.seconds)
        obj.raw_repl = repl
        obj._devices = repl.devices
        obj._flags = repl.flags
        obj._cl = cl
        obj._sm = sm
        obj._interactive = interactive
        return obj
    
    def close(self):
        if self._sm is None:
            raise ValueError("this session object doesn't have a session manager attached")
        self._sm._close_session(self)
    
    @property
    def userid(self):
        return self._userid
    
    @property
    def comment(self):
        return self._comment
    
    @property
    def expiry(self):
        return self._expiry
    
    @property
    def devices(self):
        if self._cl is not None:
            return [self._cl[_] for _ in self._devices]
        else:
            return self._devices
    
    @property
    def flags(self):
        if self._cl is not None:
            return [self._cl.flags[_] for _ in self._flags]
        else:
            return self._flags
    
    @property
    def isexpired(self) -> bool:
        now = datetime.now()
        return self.expiry < now
    
    @property
    def isowned(self) -> bool:
        return len(self._secret) > 0
    
    def __repr__(self) -> str:
        isexpiredstr = "EXPIRED" if self.isexpired else "ACTIVE"
        
        ownedstr = "OWNER" if self.isowned else "INFO"
        
        strtime = self.expiry.strftime("%c")
        return "Session(user=%s, comment='%s', expiry='%s') - %s, %s"  % \
                  (self.userid, self.comment, strtime, isexpiredstr, ownedstr)

    def __del__(self):
        if self._interactive:
            print("closing interactive session")
            self.close()

class SessionManager:
    def __init__(self, cl: Client):
        self._stub = sessionsStub(cl._channel)
        self._cl = cl
    
    def _start_session(self, userid: str, comment: str, devices=None,
                        flags=None, expiry_request: datetime=None) -> Session:
        req = session_info(user_identifier=userid,
                           comment=comment)
        
        if devices is not None:
            req.devices.extend([_._devid for _ in devices])
        if flags is not None:
            req.flags.extend([_._flagname for _ in flags])
        if expiry_request is not None:
            req.expiry.seconds = int(expiry_request.timestamp())
        
        repl = self._stub.StartSession(req)
        
        try:
            _check_repl_err(repl)
        except ServerError as err:
            sce = SessionConflictError._from_raw_error(err)
            raise sce
        
        ses = Session._from_repl(repl, self._cl, self)
        self._cl._active_session = ses
        return ses

    def _close_session(self, session: Session):
        req = session_info(sessionid = session._secret)
        
        repl = self._stub.CloseSession(req)
        _check_repl_err(repl)
        if self._cl._active_session is session:
            self._cl._active_session = None

    def _list_sessions(self):
        req = empty()
        repl = self._stub.ListSessions(req)
        _check_repl_err(repl)
        
        return [Session._from_repl(_,self._cl, self) for _ in repl.sessions]

    def _renew_session(self, ses: Session, requested_expiry: datetime = None):
        req = session_info(sessionid = ses._secret)
        if requested_expiry is not None:
            req.expiry.seconds = int(requested_expiry.timestamp())
        
        repl = self._stub.KeepAliveSession(req)
        _check_repl_err(repl)
        ses._expiry = datetime.fromtimestamp(repl.expiry.seconds)

if __name__ == "__main__":
    cl = Client("localhost:50051")
    
    sm = SessionManager(cl)
    ses = sm._start_session("danw", "comment1", [cl.dummy1])
    print(ses)
#    sleep(1)
#    print("trying to close session..")
#    sm._close_session(ses)
