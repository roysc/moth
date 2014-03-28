// -*- coding: utf-8 -*-
// data.cc

#include "data.hh"
#include "entity.hh"

// dereference
Data* Compt_addr::operator()() const
{
  return first->get(second);
}

// dereference
Data* Compt_mult::operator()(size_t ix) const
{
  if (ix >= third.size()) THROW_T_(Out_of_bounds, ix, third.size());
  return first->get(third.at(ix));
}

string Data::to_string() const
{
  string ret;

  // table
  switch (_dtype) {
  case dtype::ty_N:
    ret = "{()}";
    break;
  
//   // `val_' for value
// #define DATA_TO_STRING_CASE_(dt_, Type_, str_expr_) \
//   case dtype::dt_: {                                \
//     const auto& val_ = get<data::Type_, 0>();         \
//     ret = ("{" #Type_ ",") + (str_expr_) + '}';    \
//   } break

#define DATA_TO_STRING_CASE_NEW_(Type_, str_expr_)  \
    case (data::d_type<data::Type_>()): {        \
      const data::Type_& val_ = get<data::Type_>(); \
        ret = ("{" #Type_ ",") + (str_expr_) + '}'; \
    }  break                                        \
      
    
// #define GET_AT_(v_, n_) get<
    
  // DATA_TO_STRING_CASE_(ty_bool, Bool, util::concat(val_.value));
  // DATA_TO_STRING_CASE_(ty_int, Int, util::concat(val_.value));
  // DATA_TO_STRING_CASE_(ty_float, Real, util::concat(val_.value));
  // DATA_TO_STRING_CASE_(ty_rdisc, Int_rlm, util::concat(
  //                        '(', val_.index,
  //                        ',', val_.realm_max, 
  //                        ',', val_.offset, 
  //                        ')'));
  // DATA_TO_STRING_CASE_(ty_rcont, Real_rlm, util::concat(
  //                        '(', val_.index,
  //                        ',', val_.realm_max, 
  //                        ',', val_.offset, 
  //                        ')'));
  // DATA_TO_STRING_CASE_(ty_str, Str, string("\"\""));
  // DATA_TO_STRING_CASE_(ty_ref, Ref, util::concat(val_.address));
    
  DATA_TO_STRING_CASE_NEW_(Bool, util::concat(val_));
  DATA_TO_STRING_CASE_NEW_(Int, util::concat(val_));
  DATA_TO_STRING_CASE_NEW_(Real, util::concat(val_));
  DATA_TO_STRING_CASE_NEW_(Int_rlm, util::concat(val_));
  DATA_TO_STRING_CASE_NEW_(Real_rlm, util::concat(val_));
  DATA_TO_STRING_CASE_NEW_(Str, string("\"\""));
  DATA_TO_STRING_CASE_NEW_(Ref, util::concat(val_));

#undef DATA_TO_STRING_CASE_
  
  default:
    THROW_(Invalid_T<dtype::T>, _dtype);
  }
  return ret;
}
