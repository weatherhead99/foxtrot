from .TypeConversion import ft_variant_from_value
from .protos.ft_capability_pb2 import capability_argument


def construct_args(tgt_argnames: list[str], tgt_argtps: list, *args, **kwargs) -> list:
    rawargs = [None] * len(tgt_argnames)
    for name, val in kwargs.items():
        if name not in tgt_argnames:
            raise ValueError("no such argument with name: %s in capability"
                                 % name)
        pos = tgt_argnames.index(name)
        argdesc = tgt_argtps[pos]
        rawargs[pos] = capability_argument(
            pos=pos,
            value=ft_variant_from_value(val, argdesc))

    if (len(args) + len(kwargs)) > len(rawargs):
            raise ValueError("too many arguments provided")

    for idx, val in enumerate(args):
        if rawargs[idx] is not None:
            raise IndexError("conflicting positional and keyword arguments")
        desc = tgt_argtps[idx]
        rawargs[idx] = capability_argument(position=idx,
                                               value=ft_variant_from_value(val, desc))

    if any(_ is None for _ in rawargs):
        raise IndexError("not all arguments filled in")
    return rawargs

