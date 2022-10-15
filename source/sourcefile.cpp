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

        // tokenize..
        for (auto line : m_lines)
        {
            line->Tokenize();
            line->VisualizeText();
        }

        // compile & vizualize compiled elements

        return true;
    }
    else
    {
        return false;
    }
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

#define SINGLE_CHAR_TOKENS "[]()<>*/#+-"
#define WHITE_SPACE_TOKENS " \t"

const char* ScanToken(const char *src, const char *end)
{
    if (src >= end)
        return nullptr;

    char ch = *src++;

    // whitespace
    if (ch == ' ')
    {
        // scan white space
        while (src < end && *src == ' ')
            src++;
        return src;
    }
    if (ch == '\t')
    {
        // scan tabs space
        while (src < end && *src == '\t')
            src++;
        return src;
    }

    // comment line
    if (ch == ';')
    {
        return end;
    }

    // single char tokens
    if (CharInStr(ch, SINGLE_CHAR_TOKENS))
        return src;

    // quoted string
    if (ch == '"')
    { 
        while (src < end)
        {
            ch = *src++;
            if (ch == '"')
                break;
        }
        return src;
    }

    // normal text
    while (src < end && !CharInStr(*src, "[]()<>*/#+-") && !CharInStr(*src, WHITE_SPACE_TOKENS))
        src++;
    return src;
}

void SourceLine::Tokenize()
{
    m_tokens.clear();
    if (!m_chars.empty())
    {
        bool isComment = false;

        // calc positions of all chars
        int xLoc = 0;
        int charIndex = 0;

        // break into tokens
        const char* src = &m_chars[0];
        const char* tokenEnd;
        const char* end = src + m_chars.size();
        while (tokenEnd = ScanToken(src, end))
        {
            int len = (int)(tokenEnd - src);
            char* token = (char *)SDL_malloc((size_t)(len+1));
            memcpy(token, src, len);
            token[len] = 0;
            m_tokens.push_back(token);
            src = tokenEnd;
        }
    }
}

void SourceLine::FreeTokens()
{
    for (auto token : m_tokens)
        SDL_free(token);
    m_tokens.clear();
}

void SourceLine::GetCharX(int column, int& xStart, int& xEnd)
{
    if (column >= m_charXOffset.size()-1)
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

int SourceLine::GetColumnAtX(int x)
{
    for (int i = 0; i < m_charXOffset.size()-1; i++)
    {
        if (m_charXOffset[i+1] > x)
            return i;
    }
    return (int)m_chars.size();
}

void SourceLine::VisualizeText()
{
    auto settings = gApp->GetSettings();

    if (m_gcText)
    {
        delete m_gcText;
        m_gcText = nullptr;
    }

    m_charXOffset.push_back(0);
    if (!m_tokens.empty())
    {
        m_gcText = new GraphicChunk();

        int xLoc = 0;
        int charIdx = 0;
        for (auto token : m_tokens)
        {
            // process token
            int len = (int)SDL_strlen(token);
            if (token[0] == ' ')
            {
                // skip white space
                int skip = settings->whiteSpaceWidth * len;
                for (int i = 0; i < len; i++)
                {
                    xLoc += settings->whiteSpaceWidth;
                    m_charXOffset.push_back(xLoc);
                }
            }
            else if (token[0] == '\t')
            {
                for (int i = 0; i < len; i++)
                {
                    int tabFwd = xLoc + 1 + settings->tabWidth;
                    xLoc = tabFwd - (tabFwd % settings->tabWidth);
                    m_charXOffset.push_back(xLoc);
                }
            }
            else
            {
                int textWidth, textHeight;

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
                else if (gApp->GetCompiler()->FindOpCode(token) != -1)
                    col = settings->opCodeColor;

                m_gcText->Add(GraphicElement::CreateFromText(gApp->GetFont(), token, col, xLoc, 0));
                xLoc += textWidth;
            }
            charIdx += len;
        }
    }
}


