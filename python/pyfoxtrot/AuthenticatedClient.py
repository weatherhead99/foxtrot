from pyfoxtrot.Client import Client, _check_repl_err
from pyfoxtrot.foxtrot_pb2 import *
from pyfoxtrot.common import decode_sodiumkey
import json
import nacl.signing
import datetime



class AuthenticatedClient(Client):
    def __init__(self,connstr: str, certfile: str, userid: str, 
                 seckey: bytes)->None:
        super().__init__(connstr,certfile)
        seed = seckey[:32]
        self._seckey = nacl.signing.SigningKey(seed)
        self._userid = userid

    def _compute_auth_response(self, challenge: str) -> str:
        pass

    def _get_challenge_str(self, userid: str = None) -> str:
        req = auth_request(userid=userid)
        repl = self._stub.RequestAuthChallenge(req)
        _check_repl_err(repl)
        return repl

    def _prepare_auth_response(self, challengemess: auth_challenge, userid: str):
        #signmess = self._seckey.sign(decode_sodiumkey(challengemess.challenge))
        signmess = self._seckey.sign(challengemess.challenge)
        sig = signmess.signature
        req = auth_response(sig=sig, userid=userid, challengeid = challengemess.challengeid)
        return req
        
    def login(self):
        challenge = self._get_challenge_str(self._userid)
        resp = self._prepare_auth_response(challenge, self._userid)
        
        confirm = self._stub.RespondAuthChallenge(resp)
        _check_repl_err(confirm)
        self._seskey = confirm.sessionkey
        self._expiry = datetime.datetime.fromtimestamp(confirm.expiry)
        self._authlevel = confirm.authlevel
    
    @property
    def authlevel(self) -> int:
        return self._authlevel

    @property
    def expiry(self) -> datetime.datetime:
        return self._expiry

    @property
    def remaining_time(self) -> datetime.timedelta:
        now = datetime.datetime.now()
        return self.expiry - now
    
    @classmethod
    def from_credsfile(cls, connstr: str, certfile: str, 
                       userid: str, credsfile: str):
        with open(credsfile, "r") as f:
            creds = json.load(f)
        seckey = decode_sodiumkey(creds[userid]["secretkey"])
        return cls(connstr, certfile, userid, seckey)
