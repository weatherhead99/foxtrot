from __future__ import annotations
from typing import Sequence, Mapping, Any, Optional,  TYPE_CHECKING
from functools import cache

from .TypeConversion import ft_variant_from_value
from .argument_utils import construct_args

if TYPE_CHECKING:
    from .Client import Capability

class CapabilityOverloadSet:
    def __init__(self, capname: str):
        self._capname: str = capname
        self._caps: list = []
        self._arglens: list[int] = []

    def add_cap(self, cap) -> None:
        self._caps.append(cap)
        self._arglens.append(len(cap.argtypes))
        self.__repr__.cache_clear()

    def _resolve_by_arglens(self, args: Sequence, kwargs: Mapping[str, Any]) -> Optional[Capability]:
        tot_args: int = len(args) + len(kwargs)
        if self._arglens.count(tot_args) > 1:
            return None

        elif tot_args not in self._arglens:
            raise ValueError(f"no matching overload resolution for {tot_args} arguments")
        call_tgt_ind: int = self._arglens.index(tot_args)
        return self._caps[call_tgt_ind]
        
    def resolve_call(self, *args, **kwargs) -> "Capability":
        funs = [self._resolve_by_arglens, self._resolve_by_types]
        for fun in funs:
            cap = fun(args, kwargs)
            if cap is not None:
                break
        else:
            raise RuntimeError("couldn't find a way to resolve overloaded call!")

        return cap


    def _resolve_by_types(self, args: Sequence, kwargs: Mapping[str, Any] ) -> Optional[Capability]:
        if len(kwargs) > 0:
            #cannot resolve non-positional stuff yet BTW
            return None
        candidate_caps = self._caps.copy()
        for ind,candidate_cap in enumerate(candidate_caps):
            try:
                rawargs = construct_args(candidate_cap.argnames, candidate_cap.argtypes, *args, **kwargs)
            except Exception as err:
                candidate_caps.pop(ind)
        if len(candidate_caps) > 1:
            return None
        if len(candidate_caps) == 0:
            return None
        return candidate_caps[0]
            

                
    def __call__(self, *args, **kwargs):
        calltgt = self.resolve_call(*args, **kwargs)
        return calltgt.__call__(*args, **kwargs)

    @cache
    def __repr__(self) -> str:
        substrs: str = '\n'.join(_.__repr__() for _ in self._caps)
        outstr: str = f"OverloadedCapability{{ {substrs} }}"
        return outstr
        
        
    

    
