// -*- coding: utf-8 -*-
// expression.hh
#pragma once

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
    {Operation::op_match, "~"}
  };
  auto it = tbl.find(op);
  return it != end(tbl)? it->second : throw err::Not_found(__func__);
}

// _simple_ AST
struct Expression
{
protected:
  string _repr;

public:
  Expression(const string& e)
  {
    assert(valid(e));
    _repr = e;
  }
  Expression(const char* c): Expression(string(c)) {}

  // check validity of string
  static bool valid(const string& e) {return true;}
};
