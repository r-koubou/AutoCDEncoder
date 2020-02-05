/* =========================================================================
    EncodeExecutor.h
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/07 15:12:28 R-Koubou
               1st Edition.

   ======================================================================== */

#pragma once

#include <windows.h>
#include "CDDA.h"
#include "Macros.h"
#include "Util.h"
#include "LocalFreeDBItem.h"
#include "LocalFreeDB.h"
#include "EncodeQueue.h"

using namespace System::Diagnostics;
using namespace System::Threading;

/**
 * @author R-Koubou
 */
public ref class EncodeExecutor
{
//------------------------------------------------------------------------------
// Variables
//------------------------------------------------------------------------------
private:

    int THREADING;
    bool MKDIR_ONLY;

    CDDA_TOC* toc;
    LocalFreeDBItem^ item;
    String^ outputDir;
    String^ cdda2wavCommand;
    String^ cdda2wavDevId;
    String^ cdda2wavCommandArgvFile;
    String^ cdda2wavDefaultPrefix;
    String^ encoderCommand;
    String^ encoderCommandArgvFile;
    String^ prefix;
    String^ suffix;
    String^ artworkFile;

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

public:

    ////////////////////////////////////////////////////////////////////////////
    /**
     * Ctor.
     */
    EncodeExecutor( bool MKDIR_ONLY_,
                    int THREADING_,
                    CDDA_TOC* toc_,
                    LocalFreeDBItem^ item_,
                    String^ outputDir_,
                    String^ cdda2wavCommand_,
                    String^ cdda2wavDevId_,
                    String^ cdda2wavCommandArgvFile_,
                    String^ cdda2wavDefaultPrefix_,
                    String^ encoderCommand_,
                    String^ encoderCommandArgvFile_,
                    String^ prefix_,
                    String^ suffix_,
                    String^ artworkFile_ )
    {
        toc  = new CDDA_TOC();
        this->MKDIR_ONLY                = MKDIR_ONLY_;
        this->THREADING                 = THREADING_;
        *this->toc                      = *toc_;
        this->item                      = item_;
        this->outputDir                 = outputDir_;
        this->cdda2wavCommand           = cdda2wavCommand_;
        this->cdda2wavDevId             = cdda2wavDevId_;
        this->cdda2wavCommandArgvFile   = cdda2wavCommandArgvFile_;
        this->cdda2wavDefaultPrefix     = cdda2wavDefaultPrefix_;
        this->encoderCommand            = encoderCommand_;
        this->encoderCommandArgvFile    = encoderCommandArgvFile_;
        this->prefix                    = prefix_;
        this->suffix                    = suffix_;
        this->artworkFile               = artworkFile_;
    }

    ////////////////////////////////////////////////////////////////////////////
    /**
     * Dtor.
     */
    virtual ~EncodeExecutor( void ){}

    ////////////////////////////////////////////////////////////////////////////
    /**
     * Finalizer.
     */
    !EncodeExecutor( void )
    {
        DELETEIF( toc );
    }

    ////////////////////////////////////////////////////////////////////////////
    /**
     * Execute.
     */
    void execute( void );

    ////////////////////////////////////////////////////////////////////////////
    /**
     * Monitor my encoding threadpook.
     */
    int monitorThreadPool( array<EncodeQueue^>^ queue );

private:
    ////////////////////////////////////////////////////////////////////////////
    /**
     * Call from threadpool.
     */
    static void executeEncodeFromThreadPool( Object^ q_ )
    {
        EncodeQueue^ q = dynamic_cast<EncodeQueue^>( q_ );
        q->execute();
    }
};
