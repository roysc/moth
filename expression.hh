// -*- coding: utf-8 -*-
// expression.hh
#pragma once

#include "err.hh"
#include "data.hh"

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
    {OpType::op_not, "!"},
  };
  auto it = tbl.find(op);
  return it != end(tbl)? it->second : THROW_(Not_found, __func__);
}

inline
unsigned arity(OpType op)
{
  static const m<OpType, unsigned> tbl = {
    {OpType::op_gt, 2},
    {OpType::op_lt, 2},
    {OpType::op_ge, 2},
    {OpType::op_le, 2},
    {OpType::op_eq, 2},
    {OpType::op_ne, 2},
    {OpType::op_add, 2},
    {OpType::op_sub, 2},
    {OpType::op_mul, 2},
    {OpType::op_div, 2},
    {OpType::op_eof, 2},
    {OpType::op_nof, 2},
    {OpType::op_match, 2},
    {OpType::op_not, 1}
  };
  auto it = tbl.find(op);
  return it != end(tbl)? it->second : THROW_(Not_found, __func__);
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

  template <class Ch,class Tr>
  friend 
  std::basic_ostream<Ch,Tr>& operator<<(
    std::basic_ostream<Ch,Tr>& out, const Operation& e
  ) {return out << to_string(e._opn);}
};
}
