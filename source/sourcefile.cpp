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
    m_compileInfo = 0;
    m_dirtyCount = 0;
    m_path = path;
    m_name = basename(path);
    m_cmdManager = new CmdManager(this);
}

SourceFile::~SourceFile()
{
    delete m_cmdManager;
    for (auto l : m_lines)
        delete l;
}

void SourceFile::SetPath(const string& name)
{
    m_path = name;
    m_name = basename(name.c_str());
}

bool SourceFile::Load()
{
    size_t size;
    void* data = SDL_LoadFile(m_path.c_str(), &size);
    if (data)
    {
        const char* src = (const char*)data;
        bool foundEOL = true;
        while (foundEOL || *src)
        {
            const char* start = src;
            const char* end = FindEOL(src);
            foundEOL = (*end) != 0;
            src = SkipEOL(end);
            auto line = new SourceLine(start, end);
            m_lines.push_back(line);
        }
        return true;
    }
    else
    {
        return false;
    }
}

void SourceFile::ClearAllVisuals()
{
    for (auto line : m_lines)
    {
        line->ClearAllVisuals();
    }
}

bool SourceFile::Save()
{
    auto settings = gApp->GetSettings();
    string settingsPath = settings->GetFilePath();
    if (StrEqual(settingsPath, GetPath()))
    {
        vector<string> loadedFiles = settings->loadedFilePaths;
        if (!SaveInternal())
            return false;

        if (!gApp->IsShuttingDown())
        {
            settings->Load();
            settings->loadedFilePaths = loadedFiles;
            gApp->ReloadFont();
        }
        return true;
    }
    else
    {
        return SaveInternal();
    }
}

bool SourceFile::SaveInternal()
{
    FILE* fh = fopen(m_path.c_str(), "w");
    if (fh)
    {
        for (int i = 0; i < m_lines.size(); i++)
        {
            auto line = m_lines[i];
            string str(line->GetChars().data(), line->GetChars().size());
            if (i == m_lines.size() - 1)
                fprintf(fh, "%s", str.c_str());
            else
                fprintf(fh, "%s\n", str.c_str());

        }
        m_dirtyCount = 0;
        m_forceDirty = false;
        fclose(fh);
        
        if (m_compileInfo)
        {
            size_t lastindex = m_path.find_last_of(".");
            string prgname = m_path.substr(0, lastindex) + ".prg";
            m_compileInfo->SavePrg(prgname.c_str());
        }

        return true;
    }
    return false;
}

bool CharInStr(char ch, const char* str)
{
    while (*str)
    {
        if (ch == *str)
            return true;
        str++;
    }
    return false;
}

bool DblCharInStr(const char *src, const char* str)
{
    while (*str)
    {
        if (src[0] == str[0] && src[1] == str[1])
            return true;
        str += 2;
    }
    return false;
}

#define SINGLE_CHAR_TOKENS "[]()<>=&|$%*/#+@-~;:!,"
#define DOUBLE_CHAR_TOKENS "<<>><=>===!=&&||"

bool ScanToken(const char * &src, string &out)
{
    out = "";

    if (*src == ' ')
    {
        // scan spaces
        while (*src == ' ')
        {
            out.push_back(' ');
            src++;
        }
        return true;
    }

    if (*src == '\t')
    {
        // scan spaces
        while (*src == '\t')
        {
            out.push_back(' ');
            src++;
        }
        return true;
    }

    if (*src == ';')
    {
        while (*src)
        {
            out.push_back(*src);
            src++;
        }
        return true;
    }

    if (DblCharInStr(src, DOUBLE_CHAR_TOKENS))
    {
        out.push_back(*src++);
        out.push_back(*src++);
        return true;
    }

    if (CharInStr(*src, SINGLE_CHAR_TOKENS))
    {
        out.push_back(*src);
        src++;
        return true;
    }

    bool quoted = false;
    bool bs = false;
    while (*src && (bs || quoted || (!CharInStr(*src, SINGLE_CHAR_TOKENS) && *src != ' ' && *src != '\t')))
    {
        if (!bs && *src == '\\')
            bs = true;
        else
            bs = false;
        if (!bs && *src == '"')
            quoted = !quoted;
        out.push_back(*src);
        src++;
    }
    return !out.empty();
}

SourceLine::SourceLine(const char* start, const char* end)
{
    while (start != end)
    {
        m_chars.push_back(*start++);
    }
    m_breakpoint = false;
}

SourceLine::SourceLine(string line)
{
    m_chars = line;
    m_breakpoint = false;
}

SourceLine::SourceLine()
{
    m_breakpoint = false;
}

SourceLine::~SourceLine()
{
}

void SourceLine::Tokenize()
{
    // clear memory from old tokens
    m_tokens.clear();
    if (!m_chars.empty())
    {
        const char *src = &m_chars[0];
        string token;
        while (ScanToken(src, token))
        {
            m_tokens.push_back(token);
        }
    }
    m_charXOffset.clear();
    m_charXOffset.push_back(0);
    m_renderText.clear();
}

void SourceLine::GetCharX(int column, int& xStart, int& xEnd)
{
    LayoutRenderText();
    if (m_charXOffset.empty())
    {
        xStart = 0;
        xEnd = 0;
    }
    else
    {
        if (column >= m_charXOffset.size() - 1)
        {
            xStart = m_charXOffset.back();
            xEnd = xStart + 16;
        }
        else
        {
            xStart = m_charXOffset[column];
            xEnd = m_charXOffset[column + 1];
        }
    }
}

int SourceLine::GetColumnAtX(int x)
{
    LayoutRenderText();
    for (int i = 0; i < m_charXOffset.size()-1; i++)
    {
        if (m_charXOffset[i+1] > x)
            return i;
    }
    return (int)m_chars.size();
}

void SourceLine::ClearAllVisuals()
{
    m_renderText.clear();
    m_charXOffset.clear();
    m_charXOffset.push_back(0);
}

void SourceLine::LayoutRenderText()
{
    if (m_tokens.empty() || m_charXOffset.size() > 1)
        return;

    auto settings = gApp->GetSettings();
    auto cpu = gApp->GetEmulator()->GetCpu();
    auto r = gApp->GetRenderer();
    auto fr = gApp->GetFontRenderer();

    int whiteSpaceWidth = gApp->GetWhiteSpaceWidth();
    if (!m_tokens.empty())
    {
        int xLoc = 0;
        int charIdx = 0;
        for (auto &token : m_tokens)
        {
            // process token
            int len = (int)token.size();
            if (token[0] == ' ')
            {
                // skip white space
                int skip = whiteSpaceWidth * len;
                for (int i = 0; i < len; i++)
                {
                    xLoc += whiteSpaceWidth;
                    m_charXOffset.push_back(xLoc);
                }
            }
            else if (token[0] == '\t')
            {
                for (int i = 0; i < len; i++)
                {
                    int tabFwd = xLoc + 1 + (settings->tabWidth * whiteSpaceWidth);
                    xLoc = tabFwd - (tabFwd % (settings->tabWidth * whiteSpaceWidth));
                    m_charXOffset.push_back(xLoc);
                }
            }
            else
            {
                int textWidth = 0, textHeight = 0;

                // calculate the length at the end of each character
                char* buffer = (char*)SDL_malloc((size_t)(len + 1));
                for (int i = 0; i < len; i++)
                {
                    buffer[i] = token[i];
                    buffer[i + 1] = 0;
                    TTF_SizeText(gApp->GetFont(), buffer, &textWidth, &textHeight);
                    m_charXOffset.push_back(xLoc + textWidth);
                }
                SDL_free(buffer);

                // choose a color
                SDL_Color col = settings->textColor;
                if (token[0] == ';')
                    col = settings->commentColor;
                else if (cpu->IsOpcode(token.c_str()))
                    col = settings->opCodeColor;
                else if (token[0] == '$' || (token[0] >= '0' && token[0] <= '9'))
                    col = settings->numericColor;

                RenderText rt;
                rt.col = col;
                rt.x = xLoc;
                rt.y = 0;
                rt.text = token;
                m_renderText.push_back(rt);
                xLoc += textWidth;
            }
            charIdx += len;
        }
    }
}

CompilerSourceInfo* SourceFile::GetCompileInfo()
{
    return m_compileInfo;
}

void SourceFile::SetCompileInfo(CompilerSourceInfo* info)
{
    if (m_compileInfo)
        delete m_compileInfo;
    m_compileInfo = info;
}



