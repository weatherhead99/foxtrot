from pyfoxtrot.ft_types_pb2 import ft_variant, ft_simplevariant, ft_enum, ft_struct
from pyfoxtrot.ft_types_pb2 import simplevalue_types, variant_types
from pyfoxtrot.ft_types_pb2 import variant_descriptor, struct_descriptor, enum_descriptor
from enum import Enum


def ft_variant_from_value(val, descriptor: variant_descriptor) ->ft_variant:
    vartype = descriptor.variant_type
    out = ft_variant()
    if vartype == variant_types.Value("SIMPLEVAR_TYPE"):
        out.simplevar.CopyFrom(ft_simplevariant_from_value(val, descriptor))
    elif vartype == variant_types.Value("STRUCT_TYPE"):
        out.structval.CopyFrom(ft_struct_from_value(val, descriptor))
    elif vartype == variant_types.Value("ENUM_TYPE"):
        out.enumval.CopyFrom(ft_enum_from_value(val, descriptor))
    else:
        raise RuntimeError("couldn't determine type from descriptor")
    return out


def _ensure_type(val, tp: type, cond=None, errmsg=None):
    if not isinstance(val,tp):
        raise TypeError("method requires %s value" % tp.__name__ )
    if cond is not None:
        if not cond(val):
            raise ValueError("Value failed condition: %s" % errmsg)

def ft_simplevariant_from_value(val, descriptor: variant_descriptor):
    out = ft_simplevariant()
    out.size = descriptor.simplevalue_sizeof
    svt = descriptor.simplevalue_type
    if svt == simplevalue_types.Value("FLOAT_TYPE"):
        _ensure_type(val, float)
        out.dblval = val
    elif svt == simplevalue_types.Value("INT_TYPE"):
        _ensure_type(val, int)
        out.intval = val
    elif svt == simplevalue_types.Value("UNSIGNED_TYPE"):
        _ensure_type(val,int, lambda s: s > 0, "must be unsigned")
        out.uintval = val
    elif svt == simplevalue_types.Value("BOOL_TYPE"):
        _ensure_type(val, bool)
        out.boolval = val
    elif svt == simplevalue_types.Value("STRING_TYPE"):
        _ensure_type(val, str)
        out.stringval = val.encode("UTF-8")
    elif svt == simplevalue_types.Value("VOID_TYPE"):
        raise TypeError("cannot assign to void type")
    else:
        raise RuntimeError("failed to convert, maybe a problem in descriptor")
    
    return out

def ft_struct_from_value(val: dict, descriptor: variant_descriptor) -> ft_struct:
    out = ft_struct()
    _ensure_type(val, dict,
                 lambda s : all(isinstance(_,str) for _ in s.keys()),
                 "all keys must of string type")
    
    out.struct_name = descriptor.struct_desc.struct_name
    for nm, desc in descriptor.struct_desc.struct_map.items():
        if nm not in val:
            raise NameError("missing key: %s in input struct" % nm)
        out.value[nm].MergeFrom(ft_variant_from_value(val[nm], desc))
    
    return out

def ft_enum_from_value(val, descriptor: variant_descriptor) -> ft_enum:
    pass


