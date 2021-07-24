// SomeLibraryThatAllocates.cpp : Defines the exported functions for the DLL.
//

#define SOMELIBRARYTHATALLOCATES_EXPORTS 1

#include "SomeLibraryThatAllocates.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Windows.h>
#include <psapi.h>

static void WhereDoesMallocLive()
{
    HMODULE modules[100];
    DWORD needed;
    if (EnumProcessModules(GetCurrentProcess(), modules, 100, &needed))
    {
        for (size_t i = 0; i < needed; ++i)
        {
            MODULEINFO info = { 0 };
            if (GetModuleInformation(GetCurrentProcess(), modules[i], &info, sizeof(info)))
            {
                if (info.lpBaseOfDll <= reinterpret_cast<void*>(malloc)
                    && (reinterpret_cast<intptr_t>(info.lpBaseOfDll) + info.SizeOfImage) > reinterpret_cast<intptr_t>(malloc))
                {
                    char filename[1000];
                    // this is where malloc lives
                    GetModuleBaseNameA(GetCurrentProcess(), modules[i], filename, 1000);

                    printf("LIB> lib-malloc lives in %s - 0x%p\n\n", filename, malloc);
                }
            }
        }
    }
}

// This is an example of an exported function.
DLLEXPORT char * AllocateSomeData()
{
    WhereDoesMallocLive();

    char* data = (char*)malloc(1000);

    strcpy(data, "This data came from the library");

    return data;
}

