// -*- coding: utf-8 -*-
// entdef.hh
#pragma once

#include "basic.hh"

struct EntDef
{
  string _name;
  set<Compt_id> _contents;
  string name() const {return _name;}
  set<Compt_id> contents() const {return _contents;};
};
