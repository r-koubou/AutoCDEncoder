/* =========================================================================
    EncodeQueue.h
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/07 17:44:06 R-Koubou
               1st Edition.

   ======================================================================== */

#pragma once

#include <windows.h>
#include "CDDA.h"
#include "Macros.h"
#include "Util.h"
#include "LocalFreeDBItem.h"

using namespace System::Diagnostics;
using namespace System::Threading;

/**
 * Encode Queue for Multi thread
 */
public ref class EncodeQueue
{
private:
    CDDA_TOC* toc;
    LocalFreeDBItem^ item;
    String^ outputDir;
    String^ encoderCommand;
    String^ encoderCommandArgv;
    String^ prefix;
    String^ suffix;
    String^ artworkFile;

public:
    volatile bool finished;

public:
    EncodeQueue( CDDA_TOC* toc_,
                 LocalFreeDBItem^ item_,
                 String^ outputDir_,
                 String^ encoderCommand_,
                 String^ encoderCommandArgv_,
                 String^ prefix_,
                 String^ suffix_,
                 String^ artworkFile_ )
    {
        toc  = new CDDA_TOC();
        *this->toc                      = *toc_;
        this->item                      = item_;
        this->outputDir                 = outputDir_;
        this->encoderCommand            = encoderCommand_;
        this->encoderCommandArgv        = encoderCommandArgv_;
        this->prefix                    = prefix_;
        this->suffix                    = suffix_;
        this->artworkFile               = artworkFile_;
        this->finished                  = false;
    }

    ////////////////////////////////////////////////////////////////////////////
    /**
     * Dtor.
     */
    virtual ~EncodeQueue( void ){}

    ////////////////////////////////////////////////////////////////////////////
    /**
     * Finalizer.
     */
    !EncodeQueue( void )
    {
        DELETEIF( toc );
    }

    ////////////////////////////////////////////////////////////////////////////////
    /**
     * Called from theradpool from EncodeExecutor.
     */
    void execute();
};
