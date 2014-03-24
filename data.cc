// -*- coding: utf-8 -*-
// data.cc

#include "data.hh"
#include "entity.hh"

// dereference
Data* Compt_addr::operator()(size_t ix) const
{
  if (ix >= second.size()) THROW_T_(Out_of_bounds, ix, second.size());
  return first->get(second.at(ix));
}

string Data::to_string() const
{
  string ret;
    
  // `val_' for value
#define DATA_TO_STRING_CASE_(dt_, Type_, str_expr_) \
  case dtype::dt_: {                                \
    const data::Type_& val_ = get<data::Type_>();   \
    ret = ("{" #Type_ ",") + (str_expr_) + '}';    \
  } break
    
  switch (_dtype) {
  case dtype::ty_N:
    ret = "{()}";
    break;
    
  DATA_TO_STRING_CASE_(ty_bool, Bool, util::concat(val_.value));
  DATA_TO_STRING_CASE_(ty_int, Int, util::concat(val_.value));
  DATA_TO_STRING_CASE_(ty_float, Float, util::concat(val_.value));
  DATA_TO_STRING_CASE_(ty_rdisc, RlmDisc, util::concat(
                         '(', val_.index,
                         ',', val_.realm_max, 
                         ',', val_.offset, 
                         ')'));
  DATA_TO_STRING_CASE_(ty_rcont, RlmCont, util::concat(
                         '(', val_.index,
                         ',', val_.realm_max, 
                         ',', val_.offset, 
                         ')'));
  DATA_TO_STRING_CASE_(ty_str, Str, string("\"\""));
  DATA_TO_STRING_CASE_(ty_ref, Ref, util::concat(val_.address));

#undef DATA_TO_STRING_CASE_
  
  default:
    THROW_(Invalid_T<dtype::T>, _dtype);
  }
  return ret;
}
