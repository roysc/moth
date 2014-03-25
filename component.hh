// -*- coding: utf-8 -*-
// component.hh
#pragma once

#include "err.hh"
#include "assert.hh"
#include "basic.hh"

// the idea is that a component is only accessed through Compt_addr keys
// using Compt_addr = pair<Entity_ptr, Compt_id>;

// Kind of like a "a trait of a (specific) entity"
// a data instance, or Value
// struct Data;
// using Data_ptr = Data*;

// using Compt_ptr = Data*;
// using Compt_ptr_k = Data const*;

// t_ can be T or Tag
#define DTYPE_CHECK_(tag_, t_) ASSERT_BINOP_(t_, tag_, %, "Invalid type");

namespace dtype
{
using T = unsigned;

// essentially, built-in types
enum Type: T
{ 
  // emacs indentation sucks
  ty_bool,                 // could be impl'd as discrete realm of 0|1
  ty_int,
  ty_float,
  // Realm-values (of a range)
  ty_rdisc, // discrete (int)
  ty_rcont, // continuous (float)
  // ?
  ty_str,   // string, for what?
  // ty_tuple, // tuple rather than child grouping?
  // ty_calc,  // compt. calculated on reference?..
  ty_ref, // reference
  ty_N // the nil-type
};

enum class Tag: unsigned
{ boolean, number, realm, expr, ref, any, N };

// meta info for dtype
struct Meta
{
  // T type;
  // string name;
  Tag tag;
  size_t size;
};

using Dtypes = map<dtype::T, Meta>;
using DtypeNames = map<dtype::T, string>;

extern Dtypes _meta_info;
extern DtypeNames _meta_names;

// subtyping relationships on Tags
// "any term of type S can be safely used in a context where a
//  term of type T is expected" -WP
bool operator%(Tag s, Tag t);
bool operator%(T s, Tag t);

template <class Ch,class Tr>
std::basic_ostream<Ch,Tr>& 
operator<<(std::basic_ostream<Ch,Tr>& out, Tag tag)
{
  static const map<Tag, string> tbl = {
    {Tag::boolean, "Tag::boolean"},
    {Tag::number, "Tag::number"},
    {Tag::realm, "Tag::realm"},
    {Tag::expr, "Tag::expr"},
    {Tag::any, "Tag::any"},
  };
  auto it = tbl.find(tag);
  if (it != end(tbl))
    return out << it->second;
  THROW_(Not_found, __PRETTY_FUNCTION__);
}

unsigned size(T dt);
// T from_string(string s, bool*);
string to_string(T dt);
}
