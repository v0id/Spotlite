#ifndef CRASHDUMPER_H
#define CRASHDUMPER_H

#include <windows.h>
#include <tchar.h>
#include <dbghelp.h>
#include <stdio.h>
// #include <crtdbg.h>

class Crashdumper
{
public:
    static void setMinidumpFile(const char *file);

protected:
    static const char *_minidumpFile;

    static LONG WINAPI topLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo );
    static BOOL CALLBACK miniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT pInput, PMINIDUMP_CALLBACK_OUTPUT pOutput);
};

#endif // CRASHDUMPER_H
