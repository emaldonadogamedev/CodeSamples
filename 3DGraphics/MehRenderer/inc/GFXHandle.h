#pragma once

#include "GFXTypes.h"

class Handle
{
public:
  Handle();

  ObjectType GetType(void) const;

  int operator*(void) const;
  operator bool(void) const;
  bool operator==(const Handle& rhs) const;
  bool operator!=(const Handle& rhs) const;
  bool operator<(const Handle& rhs) const; // For STL comparisons

private:
  Handle(ObjectType type, int index);

  ObjectType type;
  int index;
};
