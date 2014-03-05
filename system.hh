// -*- coding: utf-8 -*-
// mrun.hh
#pragma once

struct SystemHandle
{
protected:
  CptClass* _cptclass;
public:
  CptClass* cptclass() const {return _cptclass;}
};
