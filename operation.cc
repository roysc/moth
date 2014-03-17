// -*- coding: utf-8 -*-
// operation.hh

#include <algorithm>

#include "operation.hh"

namespace expr
{
namespace {
const table<string, OpType> _repr_tbl = {
  {">", OpType::op_gt},
  {"<", OpType::op_lt},
  {"≥", OpType::op_ge},
  {"≤", OpType::op_le},
  {"=", OpType::op_eq},
  {"≠", OpType::op_ne},
  {"+", OpType::op_add},
  {"-", OpType::op_sub},
  {"×", OpType::op_mul},
  {"/", OpType::op_div},
  {"∈", OpType::op_eof},
  {"∉", OpType::op_nof},
  {"~", OpType::op_match},
  {"∧", OpType::op_and},
  {"∨", OpType::op_or},
  {"¬", OpType::op_not},
};
}

OpType to_operation(string s)
{
  auto it = _repr_tbl.find(s);
  return it != end(_repr_tbl)? it->second
    : THROW_(Not_found_T<string>, s, __PRETTY_FUNCTION__);
}

string to_string(OpType op)
{
  auto it = find_if(begin(_repr_tbl), end(_repr_tbl),
                    [=](pair<string, OpType> p){return p.second == op;});
  return it != end(_repr_tbl)? it->first : (
  THROW_(Not_found_T<OpType>, op, __PRETTY_FUNCTION__)
    // THROW_(Not_found, util::concat(__PRETTY_FUNCTION__, ": ", (int)op))
  );
}
}
