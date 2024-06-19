#include <QObject>

#ifdef Q_WS_WIN
#include "crashdumper.h"

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType, CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);
const char *Crashdumper::_minidumpFile = NULL;

void Crashdumper::setMinidumpFile(const char *file)
{
    _minidumpFile = strdup(file);
    ::SetUnhandledExceptionFilter(topLevelFilter);
}

LONG WINAPI Crashdumper::topLevelFilter(struct _EXCEPTION_POINTERS *pei)
{
    HINSTANCE hDll  = ::LoadLibraryA("DBGHELP.DLL");
    HANDLE hFile = ::CreateFileA( _minidumpFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

    if( hFile != NULL && hFile != INVALID_HANDLE_VALUE && hDll != NULL )
    {
        MINIDUMPWRITEDUMP dump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDll, "MiniDumpWriteDump");
        MINIDUMP_EXCEPTION_INFORMATION mdei;
        mdei.ThreadId           = GetCurrentThreadId();
        mdei.ExceptionPointers  = pei;
        mdei.ClientPointers     = FALSE;
        MINIDUMP_CALLBACK_INFORMATION mci;
        mci.CallbackRoutine     = (MINIDUMP_CALLBACK_ROUTINE) miniDumpCallback;
        mci.CallbackParam       = 0;
        MINIDUMP_TYPE mdt       = (MINIDUMP_TYPE)(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory);
        dump( GetCurrentProcess(), GetCurrentProcessId(), hFile, mdt, (pei != 0) ? &mdei : 0, 0, &mci );
        CloseHandle( hFile );
   }

   return EXCEPTION_CONTINUE_SEARCH;
}


/*
 * Parameters from http://www.debuginfo.com/ article
 */
BOOL CALLBACK Crashdumper::miniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT pInput, PMINIDUMP_CALLBACK_OUTPUT pOutput)
{
    BOOL bRet = FALSE;
    if( !pInput || !pOutput )
            return FALSE;

    switch( pInput->CallbackType )
    {
        case IncludeModuleCallback:
        {
                // Include the module into the dump
                bRet = TRUE;
        }
        break;

        case IncludeThreadCallback:
        {
                // Include the thread into the dump
                bRet = TRUE;
        }
        break;

        case ModuleCallback:
        {
                // Does the module have ModuleReferencedByMemory flag set ?

                if( !(pOutput->ModuleWriteFlags & ModuleReferencedByMemory) )
                {
                        // No, it does not - exclude it

                        wprintf( L"Excl. module: %s\n", pInput->Module.FullPath );

                        pOutput->ModuleWriteFlags &= (~ModuleWriteModule);
                }

                bRet = TRUE;
        }
        break;

        case ThreadCallback:
        {
                // Include all thread information into the minidump
                bRet = TRUE;
        }
        break;

        case ThreadExCallback:
        {
                // Include this information
                bRet = TRUE;
        }
        break;

        case MemoryCallback:
        {
                // We do not include any information here -> return FALSE
                bRet = FALSE;
        }
        break;
    }

    return bRet;
}

#endif
