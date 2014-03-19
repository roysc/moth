// -*- coding: utf-8 -*-
// entdef.hh
#pragma once

#include "typedefs.hh"
#include "basic.hh"

struct EntDef
{
  string _name;
  set<Compt_id> _contents;
  const string& name() const {return _name;}
  const set<Compt_id>& contents() const {return _contents;};
};
