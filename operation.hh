// -*- coding: utf-8 -*-
// operation.hh
#pragma once

#include <functional>

#include "err.hh"
// TODO rename to data.hh
#include "component.hh"

namespace expr
{
enum class OpType: unsigned
{
  op_gt, // >,
  op_lt, // <,
  op_ge, // >=,
  op_le, // <=,
  op_eq, // =,
  op_ne, // !=
  op_add, // +,
  op_sub, // -,
  op_mul, // *,
  op_div, // /
  op_eof, // ∈,
  op_nof, // ∉,
  op_match, // ~
  op_not, // !
  op_N // how many
};

// template <> class std::hash<OpType>
// {
//   std::size_t operator()(OpType o) 
//   { 
//     using UT = std::underlying_type<OpType>::type;
//     return hash<UT>{}(static_cast<UT>(o));
//   }
// };

inline
OpType to_operation(string s)
{
  static const table<string, OpType> tbl = {
    {">", OpType::op_gt},
    {"<", OpType::op_lt},
    {">=", OpType::op_ge},
    {"<=", OpType::op_le},
    {"=", OpType::op_eq},
    {"!=", OpType::op_ne},
    {"+", OpType::op_add},
    {"-", OpType::op_sub},
    {"*", OpType::op_mul},
    {"/", OpType::op_div},
    {"∈", OpType::op_eof},
    {"∉", OpType::op_nof},
    {"~", OpType::op_match},
    {"!", OpType::op_not},
  };
  auto it = tbl.find(s);
  return it != end(tbl)? it->second : THROW_(Not_found, __func__);
}

inline
string to_string(OpType op)
{
  static const m<OpType, string> tbl = {
    {OpType::op_gt, ">"},
    {OpType::op_lt, "<"},
    {OpType::op_ge, ">="},
    {OpType::op_le, "<="},
    {OpType::op_eq, "="},
    {OpType::op_ne, "!="},
    {OpType::op_add, "+"},
    {OpType::op_sub, "-"},
    {OpType::op_mul, "*"},
    {OpType::op_div, "/"},
    {OpType::op_eof, "∈"},
    {OpType::op_nof, "∉"},
    {OpType::op_match, "~"},
    {OpType::op_match, "!"},
  };
  auto it = tbl.find(op);
  return it != end(tbl)? it->second : throw err::Not_found(__func__);
}

template <class Ch,class Tr>
std::basic_ostream<Ch,Tr>& 
operator<<(std::basic_ostream<Ch,Tr>& out, OpType ot)
{
  static const m<OpType, string> tbl = {
    {OpType::op_gt, "OpType::op_gt"},
    {OpType::op_lt, "OpType::op_lt"},
    {OpType::op_ge, "OpType::op_ge"},
    {OpType::op_le, "OpType::op_le"},
    {OpType::op_eq, "OpType::op_eq"},
    {OpType::op_ne, "OpType::op_ne"},
    {OpType::op_add, "OpType::op_add"},
    {OpType::op_sub, "OpType::op_sub"},
    {OpType::op_mul, "OpType::op_mul"},
    {OpType::op_div, "OpType::op_div"},
    {OpType::op_eof, "OpType::op_eof"},
    {OpType::op_nof, "OpType::op_nof"},
    {OpType::op_match, "OpType::op_match"},
    {OpType::op_not, "OpType::op_not"},
  };
  auto it = tbl.find(ot);
  if (it != end(tbl))
    out << it->second;
  else 
    THROW_(Not_found, __func__);
  return out;
}

// function arg types
inline
v<dtype::Tag> arg_dtypes(OpType op)
{
  using dtype::Tag;
  static const m<OpType, v<Tag> > tbl = {
    {OpType::op_gt, {Tag::number, Tag::number}},
    {OpType::op_lt, {Tag::number, Tag::number}},
    {OpType::op_ge, {Tag::number, Tag::number}},
    {OpType::op_le, {Tag::number, Tag::number}},
    {OpType::op_eq, {Tag::any, Tag::any}},
    {OpType::op_ne, {Tag::any, Tag::any}},
    {OpType::op_add, {Tag::number, Tag::number}},
    {OpType::op_sub, {Tag::number, Tag::number}},
    {OpType::op_mul, {Tag::number, Tag::number}},
    {OpType::op_div, {Tag::number, Tag::number}},
    {OpType::op_eof, {Tag::N, Tag::N}},
    {OpType::op_nof, {Tag::N, Tag::N}},
    {OpType::op_match, {Tag::N, Tag::N}},
    {OpType::op_not, {Tag::boolean}},
  };
  auto it = tbl.find(op);
  return it != end(tbl)? it->second : THROW_(Not_found, __func__);
}

inline
std::size_t arity(OpType op)
{
  auto tagv = arg_dtypes(op);
  return tagv.size();
}

// _simple_ AST
struct Operation
{
protected:
  OpType _opn;

public:
  Operation(OpType op): _opn(op) {}
  Operation(const string& e): Operation(to_operation(e)) {}
  Operation(const char* e): Operation(string(e)) {}
  // check validity of string
  // static bool valid(const string& e) {return true;}

  Data eval(v<Compt_ptr> cptrs)
  {
    using Cptrs = v<Compt_ptr>;
    using std::function;

    // decide what dtype to actually call with
    dtype::T dt_call{};

    { // validate the argument types
      assert(arity(_opn) == cptrs.size());

      auto argdts = arg_dtypes(_opn);
      for (size_t i{}; i < cptrs.size(); ++i)
      {
        auto dt = cptrs.at(i)->dtype();
        if (!i) dt_call = dt; // hack

        // Tags should be the same?
        assert(dtype::tag_of(dt) == argdts.at(i)
               && "Wrong argument type");
      }
    }
    
    // convenient, maybe
#define OP_BINPRED_NUM_(ty_dt_, opsym_, Ty_)            \
    ([](Cptrs cps_) {                                   \
      auto c0 = cps_.at(0), c1 = cps_.at(1);            \
      ::data::Ty_ a0; ::data::Ty_ a1;                   \
      c0->get(a0); c1->get(a1);                         \
      bool r = ((a0.value) opsym_ (a1.value));          \
      Data d(dtype::ty_dt_); d.set(::data::Bool{r});    \
      return d;                                         \
    })

#define OP_TBL_ENTRY_BINPRED_NUM_(opn_, ty_dt_, opsym_, Ty_)    \
    {                                                           \
      (Tbl_key{OpType::opn_, dtype::ty_dt_}),                   \
        OP_BIN_PRED_NUM_(dtype::ty_dt_, opsym_, Ty_)            \
        }

    // for simplicity, I'm designing this to work only for same-type
    // arguments, for now.
    using Tbl_key = pair<OpType, dtype::T>;
    static const m<
      Tbl_key, function<Data(Cptrs)>
      > tbl = {

      // $0 > $1
      // OP_TBL_ENTRY_BINPRED_NUM_(op_gt, ty_int, >, Int),
      // OP_TBL_ENTRY_BINPRED_NUM_(op_gt, ty_float, >, Float),

      // {{OpType::op_lt, dtype::ty_int}, OP_BIN_PRED_NUM_(<, Int)},
      // {{OpType::op_ge, dtype::ty_int}, OP_BIN_PRED_NUM_(>=, Int)},
      // {{OpType::op_ge, dtype::ty_float}, OP_BIN_PRED_NUM_(>=, Float)},
      // {{OpType::op_le, dtype::ty_int}, OP_BIN_PRED_NUM_(<=, Int)},
      
      // {{OpType::op_eq, dtype::ty_int}, OP_BIN_PRED_NUM_(==, Int)},
      // {{OpType::op_eq, dtype::ty_float}, OP_BIN_PRED_NUM_(==, Float)},
      // {{OpType::op_ne, dtype::ty_int}, OP_BIN_PRED_NUM_(!=, Int)},
      // {{OpType::op_ne, dtype::ty_float}, OP_BIN_PRED_NUM_(!=, Float)},

      // // *********** old
      // {{OpType::op_gt, dtype::ty_int}, OP_BIN_PRED_NUM_(>, Int)},
      // {{OpType::op_gt, dtype::ty_float}, OP_BIN_PRED_NUM_(>, Float)},
      // // {{OpType::op_lt, dtype::ty_int}, OP_BIN_PRED_NUM_(<, Int)},
      // {{OpType::op_ge, dtype::ty_int}, OP_BIN_PRED_NUM_(>=, Int)},
      // {{OpType::op_ge, dtype::ty_float}, OP_BIN_PRED_NUM_(>=, Float)},
      // // {{OpType::op_le, dtype::ty_int}, OP_BIN_PRED_NUM_(<=, Int)},
      // // 
      // {{OpType::op_eq, dtype::ty_int}, OP_BIN_PRED_NUM_(==, Int)},
      // {{OpType::op_eq, dtype::ty_float}, OP_BIN_PRED_NUM_(==, Float)},
      // {{OpType::op_ne, dtype::ty_int}, OP_BIN_PRED_NUM_(!=, Int)},
      // {{OpType::op_ne, dtype::ty_float}, OP_BIN_PRED_NUM_(!=, Float)},
      // ***************

      // {OpType::op_add, 2},
      // {OpType::op_sub, 2},
      // {OpType::op_mul, 2},
      // {OpType::op_div, 2},
      // 
      // {OpType::op_eof, 2},
      // {OpType::op_nof, 2},
      // 
      // {OpType::op_match, 2},

      {{OpType::op_not, dtype::ty_bool},
       [](Cptrs cps) -> Data {
          // return data
          Data rd(dtype::ty_bool);
          data::Bool cpd;
          cps.at(0)->get(cpd);
          rd.set(data::Bool{!cpd.value});
          return rd;
       }},
    };
    
    Tbl_key k{_opn, dt_call};
    auto it = tbl.find(k);
    return (it != end(tbl))?
      it->second(cptrs) : (
        LOG_(warning)("No operation implemented for ", k),
        Data(dtype::ty_N)
      );
  }

  template <class Ch,class Tr>
  friend std::basic_ostream<Ch,Tr>& 
  operator<<(std::basic_ostream<Ch,Tr>& out, const Operation& e)
  {return out << e._opn;}
};
}
