// -*- coding: utf-8 -*-
// mrun.hh
#pragma once

// struct SystemHandle: public CptClass {};

struct SystemHandle
{
  const CptClass* cptclass() const {return _cptclass;}
protected:
  const CptClass* _cptclass;
};
