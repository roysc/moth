// -*- coding: utf-8 -*-
// expression.hh
#pragma once

#include "err.hh"
#include "data.hh"

namespace expr
{
enum class Operation: unsigned
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

// template <> class std::hash<Operation>
// {
//   std::size_t operator()(Operation o) 
//   { 
//     using UT = std::underlying_type<Operation>::type;
//     return hash<UT>{}(static_cast<UT>(o));
//   }
// };

inline
Operation to_operation(string s)
{
  static const table<string, Operation> tbl = {
    {">", Operation::op_gt},
    {"<", Operation::op_lt},
    {">=", Operation::op_ge},
    {"<=", Operation::op_le},
    {"=", Operation::op_eq},
    {"!=", Operation::op_ne},
    {"+", Operation::op_add},
    {"-", Operation::op_sub},
    {"*", Operation::op_mul},
    {"/", Operation::op_div},
    {"∈", Operation::op_eof},
    {"∉", Operation::op_nof},
    {"~", Operation::op_match},
    {"!", Operation::op_not},
  };
  auto it = tbl.find(s);
  return it != end(tbl)? it->second : throw err::Not_found(__func__);
}

inline
string to_string(Operation op)
{
  static const m<Operation, string> tbl = {
    {Operation::op_gt, ">"},
    {Operation::op_lt, "<"},
    {Operation::op_ge, ">="},
    {Operation::op_le, "<="},
    {Operation::op_eq, "="},
    {Operation::op_ne, "!="},
    {Operation::op_add, "+"},
    {Operation::op_sub, "-"},
    {Operation::op_mul, "*"},
    {Operation::op_div, "/"},
    {Operation::op_eof, "∈"},
    {Operation::op_nof, "∉"},
    {Operation::op_match, "~"},
    {Operation::op_match, "!"},
  };
  auto it = tbl.find(op);
  return it != end(tbl)? it->second : throw err::Not_found(__func__);
}

inline
unsigned arity(Operation op)
{
  static const m<Operation, unsigned> tbl = {
    {Operation::op_gt, 2},
    {Operation::op_lt, 2},
    {Operation::op_ge, 2},
    {Operation::op_le, 2},
    {Operation::op_eq, 2},
    {Operation::op_ne, 2},
    {Operation::op_add, 2},
    {Operation::op_sub, 2},
    {Operation::op_mul, 2},
    {Operation::op_div, 2},
    {Operation::op_eof, 2},
    {Operation::op_nof, 2},
    {Operation::op_match, 2},
    {Operation::op_not, 1}
  };
  auto it = tbl.find(op);
  return it != end(tbl)? it->second : THROW_(Not_found, __func__);
}

// _simple_ AST
struct Expression
{
protected:
  Operation _opn;

public:
  Expression(Operation op): _opn(op) {}
  Expression(const string& e): Expression(to_operation(e)) {}
  Expression(const char* e): Expression(string(e)) {}

  // check validity of string
  // static bool valid(const string& e) {return true;}
};
}
