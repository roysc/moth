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
  return it != end(tbl)? it->second
    : THROW_(Not_found, __PRETTY_FUNCTION__);
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
  return it != end(tbl)? it->second : (
    THROW_(Not_found, __PRETTY_FUNCTION__)
    // THROW_(Not_found, util::concat(__PRETTY_FUNCTION__, ": ", (int)op))
  );
}

template <class Ch,class Tr>
std::basic_ostream<Ch,Tr>& 
operator<<(std::basic_ostream<Ch,Tr>& out, OpType ot)
// {out << to_string(ot); return out;}
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
    THROW_(Not_found, __PRETTY_FUNCTION__);
  return out;
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
  friend std::basic_ostream<Ch,Tr>& 
  operator<<(std::basic_ostream<Ch,Tr>& out, const Operation& e)
  {return out << e._opn;}

  OpType optype() const {return _opn;}
  // bool operator<(Operation const& that) {return _opn < that._opn;}

  // function arg types
  v<dtype::Tag> arg_dtags() const
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
    auto it = tbl.find(_opn);
    return it != end(tbl)? it->second : THROW_(Not_found, __func__);
  }

  // function result types
  dtype::Tag res_dtag() const
  {
    using dtype::Tag;
    static const m<OpType, Tag> tbl = {
      {OpType::op_gt, Tag::boolean},
      {OpType::op_lt, Tag::boolean},
      {OpType::op_ge, Tag::boolean},
      {OpType::op_le, Tag::boolean},
      {OpType::op_eq, Tag::boolean},
      {OpType::op_ne, Tag::boolean},
      // maybe want to rethink non-predicate expr's
      // {OpType::op_add, Tag::number},
      // {OpType::op_sub, Tag::number},
      // {OpType::op_mul, Tag::number},
      // {OpType::op_div, Tag::number},
      {OpType::op_eof, Tag::boolean},
      {OpType::op_nof, Tag::boolean},
      {OpType::op_match, Tag::boolean},
      {OpType::op_not, Tag::boolean},
    };
    auto it = tbl.find(_opn);
    return it != end(tbl)? it->second : THROW_(Not_found, __func__);
  }

  std::size_t arity() const {return arg_dtags().size();}
};
}
