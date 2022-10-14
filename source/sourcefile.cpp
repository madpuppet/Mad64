#include "common.h"
#include "sourceFile.h"

const char* FindEOL(const char* src)
{
    while (*src != 0 && *src != 0xa && *src != 0xd)
        src++;
    return src;
}

const char* SkipEOL(const char* src)
{
    if (*src == 0xa)
    {
        src++;
        if (*src == 0xd)
            src++;
    }
    else if (*src == 0xd)
    {
        src++;
        if (*src == 0xa)
            src++;
    }
    return src;
}

const char* basename(const char* path)
{
    const char* ch = strrchr(path, '/');
    if (ch)
        return ch + 1;

    ch = strrchr(path, '\\');
    if (ch)
        return ch + 1;

    return path;
}

SourceFile::SourceFile(const char* path)
{
    m_path = path;
    m_name = basename(path);
}

bool SourceFile::Load()
{
    size_t size;
    void* data = SDL_LoadFile(m_path.c_str(), &size);
    if (data)
    {
        const char* src = (const char*)data;
        while (*src)
        {
            const char* start = src;
            const char* end = FindEOL(src);
            src = SkipEOL(end);
            m_lines.push_back(new SourceLine(start, end));
        }
        return true;
    }
    else
    {
        return false;
    }
}
