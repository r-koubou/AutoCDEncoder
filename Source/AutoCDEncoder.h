/* =========================================================================
    AutoCDEncoder.h
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/04 22:13:09 R-Koubou
               1st Edition.

   ======================================================================== */

#pragma once

//------------------------------------------------------------------------------
// Native Classes Header
//------------------------------------------------------------------------------

// Win32
#include <windows.h>
#include <vcclr.h>

//------------------------------------------------------------------------------
// CLI / User Classes Header
//------------------------------------------------------------------------------

// CLI
#using <System.dll>

// User
#include "CDDA.h"
#include "Properties.h"
#include "Util.h"
#include "LocalFreeDBItem.h"
#include "LocalFreeDB.h"
#include "EncodeExecutor.h"