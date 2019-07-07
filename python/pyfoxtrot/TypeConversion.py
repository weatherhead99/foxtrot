from pyfoxtrot.ft_types_pb2 import ft_variant, ft_simplevariant, ft_enum, ft_struct

def ft_variant_from_value(val) -> ft_variant:
    if isinstance(val, dict):
        return ft_struct_from_value(


def ft_simplevariant_from_value(val, unsigned=False) -> ft_simplevariant:
    out = ft_simplevariant()
    if isinstance(val, float):
        out.dblval = val
    elif type(val) is int and not unsigned:
        out.intval = val
    elif type(val) is int:
        out.uintval = val
    elif isinstance(val, bool):
        out.boolval = val
    elif isinstance(val,str):
        out.stringval = val.encode("UTF-8")
    elif isinstance(val,bytes):
        out.stringval = val
    else:
        raise TypeError("couldn't convert value to ft_simplevariant")
    
    return out

def ft_enum_from_value(val) -> ft_enum:
    pass

def ft_struct_from_value(val: dict, name: str) -> ft_struct:
    out = ft_struct()
    out.struct_name = name.encode("UTF-8")
    for k,v in val.items():
        out.value[k] = ft_variant_from_value(v)
    
    return out
