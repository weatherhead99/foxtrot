from pyfoxtrot.Client import Client, _check_repl_err
from pyfoxtrot.foxtrot_pb2 import *
import base64
import json
import nacl.signing

def decode_sodiumkey(key_in: str) -> bytes:
    return base64.standard_b64decode(key_in + "=====")


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
        

    @classmethod
    def from_credsfile(cls, connstr: str, certfile: str, 
                       userid: str, credsfile: str):
        with open(credsfile, "r") as f:
            creds = json.load(f)
        seckey = decode_sodiumkey(creds[userid]["secretkey"])
        return cls(connstr, certfile, userid, seckey)
