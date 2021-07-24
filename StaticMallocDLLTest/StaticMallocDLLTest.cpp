
#include "SomeLibraryThatAllocates.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <windows.h>
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
                    if (GetModuleBaseNameA(GetCurrentProcess(), modules[i], filename, 1000))
                    {
                        printf("exe-malloc lives in %s - 0x%p\n\n", filename, malloc);
                    }
                }
            }
        }
    }
}

int main()
{
    printf("EXE> Starting...\n");

    WhereDoesMallocLive();
    {
        printf("EXE> alloc some local space\n");
        char* aLocalString = (char*)malloc(1000);

        strcpy(aLocalString, "This is a local string");
        printf("0x%p => \"%s\"\n", aLocalString, aLocalString);
        free(aLocalString);
    }
    printf("\n==========================================\n");
    {
        printf("EXE> Getting some allocated data from a library\n");

        char* libraryData = AllocateSomeData();

        printf("0x%p => \"%s\"\n", libraryData, libraryData);

        printf("EXE> If we free that library data, what will happen?\n");
        free(libraryData);
    }
    printf("EXE> ...Are we still alive?\n");
}
