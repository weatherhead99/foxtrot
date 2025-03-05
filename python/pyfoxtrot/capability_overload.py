from typing import Sequence, Mapping, Any, Optional
from functools import cache

class CapabilityOverloadSet:
    def __init__(self, capname: str):
        self._capname: str = capname
        self._caps: list = []
        self._arglens: list[int] = []

    def add_cap(self, cap) -> None:
        self._caps.append(cap)
        self._arglens.append(len(cap.argtypes))
        self.__repr__.cache_clear()

    def _resolve_by_arglens(self, args: Sequence, kwargs: Mapping[str, Any]) -> "Capability":
        tot_args: int = len(args) + len(kwargs)
        if self._arglens.count(tot_args) > 1:
            raise KeyError("can't resolve overloaded call by argument count, this isn't supported yet!")
        elif tot_args not in self._arglens:
            raise ValueError(f"no matching overload resolution for {tot_args} arguments")
        call_tgt_ind: int = self._arglens.index(tot_args)
        return self._caps[call_tgt_ind]
        
    def resolve_call(self, *args, **kwargs) -> "Capability":
        return self._resolve_by_arglens(args, kwargs)

    def __call__(self, *args, **kwargs):
        calltgt = self.resolve_call(*args, **kwargs)
        return calltgt.__call__(*args, **kwargs)

    @cache
    def __repr__(self) -> str:
        substrs: str = '\n'.join(_.__repr__() for _ in self._caps)
        outstr: str = f"OverloadedCapability{{ {substrs} }}"
        return outstr
        
        
    

    
