#pragma once

// ------------------------------------------------------------------------------------------------
// Author: Paulo Bueno (paulo.buenov@gmail.com)
// 
// This module defines and implements the bare minimum needed to interoperate between 
// Microsoft Excel and C++ on MacOS, providing a nicer C++ wrapper to the original C-style API.
// 
// All data exchanges with Excel/VBA must be performed via primitive types (e.g. double, int, etc) 
// or, preferably, using xl::Variant. xl::Array and xl::String are NOT the same as SAFEARRAY and 
// BSTR when it comes to sending/receiving data to/from Excel; however, xl::Variant has move 
// constructors that take each of both classes as arguments, which ensures conversion between
// them to be very cheap.
// 
// Original COM data structures and constants ported directly from Windows are contained in COM.hpp.
// 
// C++ Wrapers are contained in Array.hpp, String.hpp and Variant.hpp and can be accessed via the
// 'xl' namespace.
// 
// xl::Array is a template class that has explicit instantiations at the end of Array.cpp. This 
// started out as a workaround to enable the implementation of a templatized class on a separate 
// .cpp file, but turned out to be quite convenient: it acts as a compile-time check that avoids
// instantiating a xl::Array with an invalid type.
// ------------------------------------------------------------------------------------------------

#include <Common.hpp>