from .protos.ft_types_pb2 import ft_variant, ft_simplevariant, ft_enum, ft_struct, ft_homog_array
from .protos.ft_types_pb2 import simplevalue_types, variant_types
from .protos.ft_types_pb2 import variant_descriptor, struct_descriptor, enum_descriptor, homog_array_descriptor
from .protos.ft_types_pb2 import tuple_descriptor, ft_tuple
from .protos.ft_types_pb2 import ENUM_TYPE, STRUCT_TYPE, SIMPLEVAR_TYPE, TUPLE_TYPE
from .protos.ft_types_pb2 import INT_TYPE, UNSIGNED_TYPE, BOOL_TYPE, STRING_TYPE, VOID_TYPE, FLOAT_TYPE

from .protos.ft_types_pb2 import UCHAR_TYPE, CHAR_TYPE, USHORT_TYPE, UINT_TYPE, ULONG_TYPE, SHORT_TYPE, IINT_TYPE, LONG_TYPE, BFLOAT_TYPE, BDOUBLE_TYPE
import struct


def get_struct_string(dtp, length: int) -> str:
    if dtp == UCHAR_TYPE:
        structstr = '<%dB' % length
    elif dtp == CHAR_TYPE:
        structstr = '<%db' % length
    elif dtp == USHORT_TYPE:
        structstr = '<%dH' % (length // 2)
    elif dtp == UINT_TYPE:
        structstr = '<%dI' % (length // 4)
    elif dtp == IINT_TYPE:
        structstr = '<%di' % (length // 4)
    elif dtp == BDOUBLE_TYPE:
        structstr = '<%dd' % (length // 8)
    elif dtp == BFLOAT_TYPE:
        structstr = '<%df' % (length // 4)
    else:
        #NOTE: all the others are dodgy at the moment
        raise NotImplementedError("data type not supported yet")

    return structstr



def ft_variant_from_value(val, descriptor: variant_descriptor) -> ft_variant:
    vartype = descriptor.variant_type
    out = ft_variant()
    if vartype == SIMPLEVAR_TYPE:
        out.simplevar.CopyFrom(ft_simplevariant_from_value(val, descriptor))
    elif vartype == STRUCT_TYPE:
        out.structval.CopyFrom(ft_struct_from_value(val, descriptor))
    elif vartype == ENUM_TYPE:
        out.enumval.CopyFrom(ft_enum_from_value(val, descriptor))
    else:
        raise RuntimeError("couldn't determine type from descriptor")
    return out


def _ensure_type(val, tp: type, cond=None, errmsg=None):
    if not isinstance(val, tp):
        raise TypeError("method requires %s value" % tp.__name__)
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
        _ensure_type(val, int, lambda s: s >= 0, "must be unsigned")
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


def ft_struct_from_value(val: dict,
                         descriptor: variant_descriptor) -> ft_struct:
    out = ft_struct()
    _ensure_type(val, dict,
                 lambda s: all(isinstance(_, str) for _ in s.keys()),
                 "all keys must of string type")
    
    out.struct_name = descriptor.struct_desc.struct_name
    for nm, desc in descriptor.struct_desc.struct_map.items():
        if nm not in val:
            raise NameError("missing key: %s in input struct" % nm)
        out.value[nm].MergeFrom(ft_variant_from_value(val[nm], desc))
    
    return out

def ft_enum_from_value(val, descriptor: variant_descriptor) -> ft_enum:
    out = ft_enum(desc=descriptor.enum_desc)
    if isinstance(val, str):
        if val not in descriptor.enum_desc.enum_map:
            raise KeyError("couldn't map string %s to enum value" % val)
        sanval = descriptor.enum_desc.enum_map[val]
    elif isinstance(val, int):
        if int(val) not in descriptor.enum_desc.enum_map.values():
            raise KeyError("couldn't map int %d to valid enum value" % val)
        sanval = val
    else:
        raise RuntimeError("type: %s is not supported for converting to enum yet"
                           % type(val).__name__)

    out.enum_value = sanval
    return out


def value_from_ft_variant(variant):
    whichattr = variant.WhichOneof("value")
    if whichattr == "simplevar":
        return value_from_ft_simplevar(variant.simplevar)
    elif whichattr == "structval":
        return value_from_ft_struct(variant.structval)
    elif whichattr == "enumval":
        return value_from_ft_enum(variant.enumval)
    elif whichattr == "tupleval":
        return value_from_ft_tuple(variant.tupleval)
    elif whichattr == "arrayval":
        return value_from_ft_array(variant.arrayval)
    else:
        raise ValueError("couldn't determine variant type")


def value_from_ft_array(variant: ft_homog_array):
    whichattr = variant.WhichOneof("array")
    if whichattr == "arr_decoded":
        raise NotImplementedError("don't understand decoded arrays yet")
    else:
        rawdat = variant.arr_encoded.data
        dtp = variant.arr_encoded.dtp
        structstr = get_struct_string(dtp, len(rawdat))
        return struct.unpack(structstr, rawdat)

def value_from_ft_simplevar(variant: ft_simplevariant):
    whichattr = variant.WhichOneof("value")
    if whichattr is not None:
        return getattr(variant, whichattr)
    else:
        return None


def value_from_ft_struct(variant: ft_struct):
    return {k: value_from_ft_variant(v) for k, v in variant.value.items()}


def value_from_ft_enum(variant: ft_enum):
    return variant.enum_value

def value_from_ft_tuple(variant: ft_tuple):
    v = variant.value
    return tuple( value_from_ft_variant(_) for _ in v)

_simplevar_stringdescs_py_style = {(FLOAT_TYPE, 4): "float[4]",
                                   (FLOAT_TYPE, 8): "float[8]",
                                   (INT_TYPE, 4): "int[4]",
                                   (INT_TYPE, 8): "int[8]",
                                   (INT_TYPE, 2): "int[2]",
                                   (INT_TYPE, 1): "int[1]",
                                   (UNSIGNED_TYPE, 1): "uint[1]",
                                   (UNSIGNED_TYPE, 2): "uint[2]",
                                   (UNSIGNED_TYPE, 4): "uint[4]",
                                   (UNSIGNED_TYPE, 8): "uint[8]",
                                   BOOL_TYPE:  "bool",
                                   VOID_TYPE: "void",
                                   STRING_TYPE: "str"}

def string_describe_ft_variant(descriptor: variant_descriptor):
    if descriptor.variant_type == SIMPLEVAR_TYPE:
        tp = descriptor.simplevalue_type
        size = descriptor.simplevalue_sizeof
        if tp in (BOOL_TYPE, VOID_TYPE, STRING_TYPE):
            typestr = _simplevar_stringdescs_py_style[tp]
        elif (tp, size) in _simplevar_stringdescs_py_style:
            typestr = _simplevar_stringdescs_py_style[(tp,size)]
        else:
            typestr = f"cpptype[{descriptor.cpp_type_name}]"
    elif descriptor.variant_type == ENUM_TYPE:
        enum_name = descriptor.enum_desc.enum_name.replace("::", "_")
        return "enum[%s]" % enum_name
    elif descriptor.variant_type == STRUCT_TYPE:
        struct_name = descriptor.struct_desc.struct_name.replace("::", "_")
        return "struct[%s]" % struct_name
    elif descriptor.variant_type == TUPLE_TYPE:
        types = ",".join(string_describe_ft_variant(_) for _ in descriptor.tuple_desc.tuple_map)
        return "tuple[%s]" % types
    return typestr
