// Using SDL and standard IO
#include "common.h"

#if 0
#include <windows.h>
#include <commdlg.h>

OPENFILENAMEA ofn;
char szFile[] = "pork";
ZeroMemory(&ofn, sizeof(ofn));
ofn.lStructSize = sizeof(ofn);
ofn.hwndOwner = NULL;
ofn.lpstrFile = szFile;
ofn.lpstrFile[0] = '\0';
ofn.nMaxFile = sizeof(szFile);
ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
ofn.nFilterIndex = 1;
ofn.lpstrFileTitle = NULL;
ofn.nMaxFileTitle = 0;
ofn.lpstrInitialDir = NULL;
ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
GetOpenFileNameA(&ofn);
#endif


Application* gApp;
int main(int argc, char *args[])
{
    gApp = new Application();
    gApp->MainLoop();
    delete gApp;
    return 0;
}




