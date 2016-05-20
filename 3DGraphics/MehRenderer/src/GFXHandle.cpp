#include "GFXHandle.h"

Handle::Handle()
  : type(OT_NONE), index(-1)
{
}

// Private
Handle::Handle(ObjectType type, int index)
  : type(type), index(index)
{
}

// Public

ObjectType Handle::GetType(void) const
{
  return type;
}

int Handle::operator*(void) const
{
  return index;
}

Handle::operator bool(void) const
{
  return type != OT_NONE && index != -1;
}

bool Handle::operator==(const Handle& rhs) const
{
  return type == rhs.type && index == rhs.index;
}

bool Handle::operator!=(const Handle& rhs) const
{
  return type != rhs.type || index != rhs.index;
}

bool Handle::operator<(const Handle& rhs) const
{
  if (type == rhs.type)
    return index < rhs.index;
  else
    return type < rhs.type;
}