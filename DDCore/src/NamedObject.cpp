// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework includes
#include "DD4hep/NamedObject.h"
#include "DD4hep/Handle.inl"
#include "TObject.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

DD4HEP_INSTANTIATE_HANDLE_NAMED(NamedObject);

/// Standard constructor
NamedObject::NamedObject()  {
}

/// Initializing constructor
NamedObject::NamedObject(const char* nam, const char* typ)
  : name(nam ? nam : ""), type(typ ? typ : "")
{
}

/// Initializing constructor
NamedObject::NamedObject(const std::string& nam)
  : name(nam), type()
{
}

/// Initializing constructor
NamedObject::NamedObject(const std::string& nam, const std::string& typ)
  : name(nam), type(typ)
{
}

/// Copy constructor
NamedObject::NamedObject(const NamedObject& c)  : name(c.name), type(c.type) {
}

/// Default destructor
NamedObject::~NamedObject()  {
}

/// Assignment operator
NamedObject& NamedObject::operator=(const NamedObject& c)  {
  if ( this != &c ) {
    name = c.name;
    type = c.type;
  }
  return *this;
}

