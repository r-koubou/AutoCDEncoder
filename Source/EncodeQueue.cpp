/* =========================================================================
    EncodeQueue.cpp
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/07 17:44:06 R-Koubou
               1st Edition.

   ======================================================================== */

#include "EncodeQueue.h"

////////////////////////////////////////////////////////////////////////////////
/**
 * Called from theradpool from EncodeExecutor.
 */
void EncodeQueue::execute()
{
    finished = false;

    bool result;

    Process^ p = gcnew Process();
    p->StartInfo->FileName         = encoderCommand;
    p->StartInfo->Arguments        = encoderCommandArgv;
    p->StartInfo->WorkingDirectory = outputDir;
    p->StartInfo->CreateNoWindow   = true;
    result = p->Start();

    if( !result )
    {
        System::Console::WriteLine( "[ERROR] cannot launch encoder : {0} {1}", encoderCommand, encoderCommandArgv );
    }
    else
    {
        while( ! p->HasExited )
        {
            Util::sleep( 50 );
        }
    }

    finished = true;
}
