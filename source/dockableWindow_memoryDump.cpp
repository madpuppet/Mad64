#include "common.h"
#include "dockableWindow_memoryDump.h"

void DockableWindow_MemoryDump::OnChildRendererChange()
{
    FreeTexture();
}

void DockableWindow_MemoryDump::DrawChild()
{
    switch (m_currentMode)
    {
        case MODE_Hex8:
            DrawHex8();
            break;
        case MODE_Hex16:
            DrawHex16();
            break;
        case MODE_Bin8:
            DrawBin8();
            break;
        case MODE_Bin16:
            DrawBin16();
            break;
        case MODE_Petsci:
            DrawPetsci();
            break;
        case MODE_Sprite:
        case MODE_SpriteMC:
            DrawSprite();
            break;
        case MODE_CharSet:
        case MODE_CharSetMC:
            DrawCharSet();
            break;
        case MODE_Bitmap:
        case MODE_BitmapMC:
            DrawBitmap();
            break;
    }
}

extern bool HexToNumber(const string& token, int& value);
int TextToNumber(const string& token)
{
    auto file = gApp->GetEditWindow()->GetActiveFile();
    if (file)
    {
        auto compiler = gApp->GetCompiler();
        auto csi = file->GetCompileInfo();
        if (csi)
        {
            auto label = compiler->FindLabel(csi, token, LabelResolve_Global, 0);
            if (label)
            {
                return (u32)label->m_value;
            }
        }
    }

    int result = 0;
    if (token[0] == '$')
    {
        string tok = token.substr(1, token.size() - 1);
        HexToNumber(tok, result);
        return result;
    }
    return atoi(token.c_str());
}

void DockableWindow_MemoryDump::OnModeChange(int mode)
{
    m_currentMode = (Mode)mode;
}

void DockableWindow_MemoryDump::OnRangeChange(const string& text)
{
    int dataCount = m_dataCount;
    int memStart = m_memoryStart;
    int memEnd = m_memoryEnd;

    const string& startStr = m_memoryStartBox->GetText();
    const string& endStr = m_memoryEndBox->GetText();
    const string& dataStr = m_dataCountBox->GetText();
    if (!startStr.empty())
    {
        memStart = TextToNumber(startStr);
    }
    if (!endStr.empty())
    {
        memEnd = TextToNumber(endStr);
    }
    if (!dataStr.empty())
    {
        dataCount = SDL_clamp(TextToNumber(dataStr), 1, 256);
    }
    if (memEnd > memStart && dataCount > 0)
    {
        m_memoryStart = memStart;
        m_memoryEnd = memEnd;
        m_dataCount = dataCount;
    }
}

void DockableWindow_MemoryDump::CreateChildIcons()
{
    int width = gApp->GetWhiteSpaceWidth() * 12;
    m_memoryStartBox = new UIItem_TextBox("", FormatString("$%04x",m_memoryStart), width, DELEGATE(DockableWindow_MemoryDump::OnRangeChange), DELEGATE(DockableWindow_MemoryDump::OnRangeChange));
    m_memoryEndBox = new UIItem_TextBox("", FormatString("$%04x", m_memoryEnd), width, DELEGATE(DockableWindow_MemoryDump::OnRangeChange), DELEGATE(DockableWindow_MemoryDump::OnRangeChange));
    m_memoryStartBox->SetTabCallbacks(DELEGATE(DockableWindow_MemoryDump::ActivateEndBox), DELEGATE(DockableWindow_MemoryDump::ActivateEndBox));
    m_memoryEndBox->SetTabCallbacks(DELEGATE(DockableWindow_MemoryDump::ActivateStartBox), DELEGATE(DockableWindow_MemoryDump::ActivateStartBox));

    int cntWidth = gApp->GetWhiteSpaceWidth() * 6;
    m_dataCountBox = new UIItem_TextBox("", FormatString("%d", m_dataCount), cntWidth, DELEGATE(DockableWindow_MemoryDump::OnRangeChange), DELEGATE(DockableWindow_MemoryDump::OnRangeChange));

    int modeButtonWidth = gApp->GetWhiteSpaceWidth() * 10;
    vector<string> modes = { "Hex8", "Hex16", "Bin8", "Bin16", "Petsci", "Sprite", "SpriteMC", "CharSet", "CharSetMC", "Bitmap", "BitmapMC", };
    m_modeButton = new UIItem_EnumButton(0, modes, modeButtonWidth, DELEGATE(DockableWindow_MemoryDump::OnModeChange));

    int zoomButtonWidth = gApp->GetWhiteSpaceWidth() * 6;
    vector<string> zoomLevels = { "x1", "x2", "x4", "x8", "x16" };
    m_zoomButton = new UIItem_EnumButton(0, zoomLevels, zoomButtonWidth, DELEGATE(DockableWindow_MemoryDump::OnZoomChanged));
    m_titleIconsRight.push_back(m_zoomButton);

    m_titleIconsLeft.push_back(m_memoryStartBox);
    m_titleIconsLeft.push_back(m_memoryEndBox);
    m_titleIconsLeft.push_back(m_dataCountBox);
    m_titleIconsLeft.push_back(m_modeButton);
}

void DockableWindow_MemoryDump::OnZoomChanged(int option)
{
    int zoom[] = { 1, 2, 4, 8, 16 };
    m_zoomLevel = zoom[option];
}

void DockableWindow_MemoryDump::ActivateStartBox() 
{
    m_memoryStartBox->SetSelected(true);
}

void DockableWindow_MemoryDump::ActivateEndBox()
{
    m_memoryEndBox->SetSelected(true);
}


int DockableWindow_MemoryDump::GetContentHeight()
{
    return m_renderedHeight;
}

int DockableWindow_MemoryDump::GetContentWidth()
{
    return m_renderedWidth;
}

void DockableWindow_MemoryDump::OnMouseButtonDown(int button, int x, int y)
{
    DockableWindow::OnMouseButtonDown(button, x, y);
}

void DockableWindow_MemoryDump::OnMouseMotion(int xAbs, int yAbs, int xRel, int yRel)
{
    DockableWindow::OnMouseMotion(xAbs, yAbs, xRel, yRel);
}

void DockableWindow_MemoryDump::DrawHex8()
{
    m_renderedWidth = 64;
    m_renderedHeight = 0;

    auto r = GetRenderer();
    auto settings = gApp->GetSettings();

    int y = m_contentArea.y - m_vertScroll;
    int x = m_contentArea.x + settings->textXMargin - m_horizScroll;
    auto emu = gApp->GetEmulator();
    auto font = gApp->GetFont();
    SDL_Color col = { 255, 255, 255, 255 };

    for (u32 addr = m_memoryStart; addr <= m_memoryEnd; addr += m_dataCount)
    {
        if (y + settings->lineHeight >= m_clipArea.y && y <= m_clipArea.y + m_clipArea.h)
        {
            string text = FormatString("%04x ", addr);
            for (int i = 0; i < m_dataCount; i++)
            {
                text += FormatString(" %02x", emu->GetByte(addr + i));
            }

            auto ge = GraphicElement::CreateFromText(r, font, text.c_str(), col, x, y + settings->textYMargin);
            ge->Render(r);
            m_renderedWidth = SDL_max(m_renderedWidth, settings->textXMargin + ge->GetRect().w);
            delete ge;
        }
        y += settings->lineHeight;
        m_renderedHeight += settings->lineHeight;
    }
}

void DockableWindow_MemoryDump::DrawHex16()
{
    m_renderedWidth = 64;
    m_renderedHeight = 0;

    auto r = GetRenderer();
    auto settings = gApp->GetSettings();

    int y = m_contentArea.y - m_vertScroll;
    int x = m_contentArea.x + settings->textXMargin - m_horizScroll;
    auto emu = gApp->GetEmulator();
    auto font = gApp->GetFont();
    SDL_Color col = { 255, 255, 255, 255 };

    for (u32 addr = m_memoryStart; addr <= m_memoryEnd; addr += m_dataCount*2)
    {
        if (y + settings->lineHeight >= m_clipArea.y && y <= m_clipArea.y + m_clipArea.h)
        {
            string text = FormatString("%04x ", addr);
            for (int i = 0; i < m_dataCount; i++)
            {
                text += FormatString(" %04x", emu->GetByte(addr + i*2) + ((u16)emu->GetByte(addr + i*2 + 1)<<8));
            }

            auto ge = GraphicElement::CreateFromText(r, font, text.c_str(), col, x, y + settings->textYMargin);
            ge->Render(r);
            m_renderedWidth = SDL_max(m_renderedWidth, settings->textXMargin + ge->GetRect().w);
            delete ge;
        }
        y += settings->lineHeight;
        m_renderedHeight += settings->lineHeight;
    }
}
void DockableWindow_MemoryDump::DrawBin8()
{
    m_renderedWidth = 64;
    m_renderedHeight = 0;

    auto r = GetRenderer();
    auto settings = gApp->GetSettings();

    int y = m_contentArea.y - m_vertScroll;
    int x = m_contentArea.x + settings->textXMargin - m_horizScroll;
    auto emu = gApp->GetEmulator();
    auto font = gApp->GetFont();
    SDL_Color col = { 255, 255, 255, 255 };

    for (u32 addr = m_memoryStart; addr <= m_memoryEnd; addr += m_dataCount)
    {
        if (y + settings->lineHeight >= m_clipArea.y && y <= m_clipArea.y + m_clipArea.h)
        {
            string text = FormatString("%04x ", addr);
            for (int i = 0; i < m_dataCount; i++)
            {
                u8 val = emu->GetByte(addr + i);
                char out[10];
                out[0] = ' ';
                out[9] = 0;
                for (int ii = 0; ii < 8; ii++)
                    out[ii + 1] = (val & (1 << ii)) ? '1' : '0';
                text += out;
            }

            auto ge = GraphicElement::CreateFromText(r, font, text.c_str(), col, x, y + settings->textYMargin);
            ge->Render(r);
            m_renderedWidth = SDL_max(m_renderedWidth, settings->textXMargin + ge->GetRect().w);
            delete ge;
        }
        y += settings->lineHeight;
        m_renderedHeight += settings->lineHeight;
    }
}
void DockableWindow_MemoryDump::DrawBin16()
{
    m_renderedWidth = 64;
    m_renderedHeight = 0;

    auto r = GetRenderer();
    auto settings = gApp->GetSettings();

    int y = m_contentArea.y - m_vertScroll;
    int x = m_contentArea.x + settings->textXMargin - m_horizScroll;
    auto emu = gApp->GetEmulator();
    auto font = gApp->GetFont();
    SDL_Color col = { 255, 255, 255, 255 };

    for (u32 addr = m_memoryStart; addr <= m_memoryEnd; addr += m_dataCount*2)
    {
        if (y + settings->lineHeight >= m_clipArea.y && y <= m_clipArea.y + m_clipArea.h)
        {
            string text = FormatString("%04x ", addr);
            for (int i = 0; i < m_dataCount; i++)
            {
                u16 val = emu->GetByte(addr + i) + ((u16)emu->GetByte(addr + i + 1) << 8);
                char out[18];
                out[0] = ' ';
                out[17] = 0;
                for (int ii = 0; ii < 16; ii++)
                    out[ii + 1] = (val & (1 << ii)) ? '1' : '0';
                text += out;
            }

            auto ge = GraphicElement::CreateFromText(r, font, text.c_str(), col, x, y + settings->textYMargin);
            ge->Render(r);
            m_renderedWidth = SDL_max(m_renderedWidth, settings->textXMargin + ge->GetRect().w);
            delete ge;
        }
        y += settings->lineHeight;
        m_renderedHeight += settings->lineHeight;
    }
}
void DockableWindow_MemoryDump::DrawPetsci()
{
    m_renderedWidth = 64;
    m_renderedHeight = 0;

    auto r = GetRenderer();
    auto settings = gApp->GetSettings();

    int y = m_contentArea.y - m_vertScroll;
    int x = m_contentArea.x + settings->textXMargin - m_horizScroll;
    auto emu = gApp->GetEmulator();
    auto font = gApp->GetFontC64();
    SDL_Color col = { 255, 255, 255, 255 };

    u16* text16 = new u16[m_dataCount + 6 + 1];
    for (u32 addr = m_memoryStart; addr <= m_memoryEnd; addr += m_dataCount)
    {
        if (y + settings->lineHeight >= m_clipArea.y && y <= m_clipArea.y + m_clipArea.h)
        {
            string text = FormatString("%04x  ", addr);
            for (int i=0; i<6; i++)
                text16[i] = (u16)text[i];

            for (int i = 0; i < m_dataCount; i++)
                text16[i+6] = 0xee00 + emu->GetByte(addr + i);

            text16[m_dataCount + 6] = 0;

            SDL_Surface* surface = TTF_RenderUNICODE_Blended(font, text16, col);
            SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surface);
            SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
            SDL_FreeSurface(surface);

            i32 w, h;
            SDL_QueryTexture(tex, NULL, NULL, &w, &h);
            SDL_Rect quad = { x, y + settings->textYMargin, w, h };
            SDL_RenderCopy(r, tex, 0, &quad);
            m_renderedWidth = SDL_max(m_renderedWidth, settings->textXMargin + w);
            SDL_DestroyTexture(tex);
        }
        y += settings->lineHeight;
        m_renderedHeight += settings->lineHeight;
    }
    delete[] text16;
}

void DockableWindow_MemoryDump::DrawSprite()
{
    auto emu = gApp->GetEmulator();
    auto settings = gApp->GetSettings();

    int memStart, memEnd, dataCount;
    CalcClampedMemoryRange(memStart, memEnd, dataCount);

    int spriteMin = memStart / 64;
    int spriteMax = SDL_min(memEnd / 64, 1024);

    // calculate area of screen that needs updating
    int rowStart = SDL_max(0, spriteMin*21 + (m_clipArea.y - m_contentArea.y + m_vertScroll) / m_zoomLevel);
    int rowEnd = SDL_max(0, spriteMin*21 + ((m_clipArea.y + m_clipArea.h) - m_contentArea.y + m_vertScroll) / m_zoomLevel);

    int spriteStart = SDL_clamp(rowStart / 21, spriteMin, spriteMax);
    int spriteEnd = SDL_clamp((rowEnd+20) / 21, spriteMin, spriteMax);

    int visMemStart = spriteStart * 64;
    int visMemEnd = spriteEnd * 64;
    if (visMemStart >= visMemEnd)
        return;

    if (m_textureMode != m_currentMode || visMemStart != m_visMemoryStart || visMemEnd != m_visMemoryEnd)
    {
        FreeTexture();

        m_textureMode = m_currentMode;
        m_visMemoryStart = visMemStart;
        m_visMemoryEnd = visMemEnd;

        m_textureWidth = 24;
        m_textureHeight = ((m_visMemoryEnd - m_visMemoryStart) / 64) * 21;

        m_memMapTexture = SDL_CreateTexture(GetRenderer(), SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, m_textureWidth, m_textureHeight);
        m_memMapSize = m_textureWidth * m_textureHeight;
        m_memMap = (u8*)malloc(m_memMapSize);
        memset(m_memMap, 255, m_memMapSize);
    }

    for (int i = spriteStart; i < spriteEnd; i++)
    {
        SDL_Color col = { 255, 255, 255, 255 };
        GraphicElement::RenderText(GetRenderer(), gApp->GetFont(), FormatString("%04x", i * 64).c_str(), col, m_contentArea.x - m_horizScroll + settings->textXMargin, m_contentArea.y + (i - spriteMin) * 21 * m_zoomLevel - m_vertScroll);
    }

    u8* out = m_memMap;
    u8* outEnd = m_memMap + m_memMapSize;
    int in = m_visMemoryStart;
    int inEnd = m_visMemoryEnd;

    if (m_currentMode == MODE_Sprite)
    {
        while (in < inEnd)
        {
            for (int i = 0; i < 63; i++)
            {
                u8 pixel = emu->GetByteVic(in++);
                for (int p = 0; p < 8; p++)
                {
                    if (pixel & (1 << (7 - p)))
                    {
                        *out++ = 255;
                    }
                    else
                    {
                        *out++ = 0;
                    }
                }
            }
            in++;
        }
    }
    else
    {
        u8 color[] = { 0x00, 0xf0, 0x0f, 0x44 };

        while (in < inEnd)
        {
            for (int i = 0; i < 63; i++)
            {
                u8 pixel = emu->GetByteVic(in++);
                for (int p = 0; p < 4; p++)
                {
                    u8 c = color[(pixel >> (6 - p * 2)) & 3];
                    *out++ = c;
                    *out++ = c;
                }
            }
            in++;
        }
    }

    SDL_assert(out == outEnd);
    SDL_assert(in == inEnd);

    int xOffset = gApp->GetWhiteSpaceWidth() * 8 + settings->textXMargin;
    SDL_UpdateTexture(m_memMapTexture, nullptr, m_memMap, 24);
    SDL_Rect dest = { m_contentArea.x + xOffset - m_horizScroll, m_contentArea.y + (spriteStart-spriteMin)*21*m_zoomLevel - m_vertScroll, m_textureWidth * m_zoomLevel, m_textureHeight * m_zoomLevel };

    SDL_RenderCopy(GetRenderer(), m_memMapTexture, nullptr, &dest);

    m_renderedWidth = dest.w + xOffset;
    m_renderedHeight = ((memEnd - memStart) / 64) * 21 * m_zoomLevel;
}

void DockableWindow_MemoryDump::DrawCharSet()
{
    auto emu = gApp->GetEmulator();
    auto settings = gApp->GetSettings();

    int memStart, memEnd, dataCount;
    CalcClampedMemoryRange(memStart, memEnd, dataCount);

    int charSetMin = memStart / 2048;
    int charSetMax = SDL_min(memEnd / 2048, 0x10000 / 2048);

    int linesPerCharSet = 2048 / dataCount;

    // calculate area of screen that needs updating
    int lineStart = SDL_max(0, charSetMin * linesPerCharSet + (m_clipArea.y - m_contentArea.y + m_vertScroll) / m_zoomLevel);
    int lineEnd = SDL_max(0, charSetMin * linesPerCharSet + ((m_clipArea.y + m_clipArea.h) - m_contentArea.y + m_vertScroll) / m_zoomLevel);

    int charSetStart = SDL_clamp(lineStart / linesPerCharSet, charSetMin, charSetMax);
    int charSetEnd = SDL_clamp((lineEnd + linesPerCharSet - 1) / linesPerCharSet, charSetMin, charSetMax);

    int visMemStart = charSetStart * 2048;
    int visMemEnd = charSetEnd * 2048;
    if (visMemStart >= visMemEnd)
        return;

    if (m_textureMode != m_currentMode || visMemStart != m_visMemoryStart || visMemEnd != m_visMemoryEnd || dataCount != m_visDataCount)
    {
        FreeTexture();

        m_textureMode = m_currentMode;
        m_visMemoryStart = visMemStart;
        m_visMemoryEnd = visMemEnd;
        m_visDataCount = dataCount;

        m_textureWidth = dataCount*8;
        m_textureHeight = ((m_visMemoryEnd - m_visMemoryStart) / dataCount);

        m_memMapTexture = SDL_CreateTexture(GetRenderer(), SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, m_textureWidth, m_textureHeight);
        m_memMapSize = m_textureWidth * m_textureHeight;
        m_memMap = (u8*)malloc(m_memMapSize);
        memset(m_memMap, 255, m_memMapSize);
    }

    for (int i = charSetStart; i < charSetEnd; i++)
    {
        SDL_Color col = { 255, 255, 255, 255 };
        GraphicElement::RenderText(GetRenderer(), gApp->GetFont(), FormatString("%04x", i * 2048).c_str(), col, m_contentArea.x - m_horizScroll + settings->textXMargin, m_contentArea.y + (i - charSetMin) * linesPerCharSet * m_zoomLevel - m_vertScroll);
    }

    if (m_currentMode == MODE_CharSet)
    {
        for (int charSet = charSetStart; charSet < charSetEnd; charSet++)
        {
            for (int ch = 0; ch < 256; ch++)
            {
                for (int r = 0; r < 8; r++)
                {
                    int in = charSet * 2048 + ch * 8 + r;
                    int row = (charSet - charSetStart) * linesPerCharSet + (ch / dataCount)*8 + r;
                    int col = (ch % dataCount) * 8;
                    u8* out = m_memMap + row * dataCount * 8 + col;
                    u8 pixel = emu->GetByteVic(in++);
                    for (int p = 0; p < 8; p++)
                    {
                        *out++ = (pixel & (1 << (7-p))) ? 255 : 0;
                    }
                }
            }
        }
    }
    else
    {
        u8 color[] = { 0x00, 0xf0, 0x0f, 0x44 };
        for (int charSet = charSetStart; charSet < charSetEnd; charSet++)
        {
            for (int ch = 0; ch < 256; ch++)
            {
                for (int r = 0; r < 8; r++)
                {
                    int in = charSet * 2048 + ch * 8 + r;
                    int row = (charSet - charSetStart) * linesPerCharSet + (ch / dataCount) * 8 + r;
                    int col = (ch % dataCount) * 8;
                    u8* out = m_memMap + row * dataCount * 8 + col;
                    u8 pixel = emu->GetByteVic(in++);
                    for (int p = 0; p < 4; p++)
                    {
                        u8 c = color[(pixel >> (6 - p * 2)) & 3];
                        *out++ = c;
                        *out++ = c;
                    }
                }
            }
        }
    }

    int xOffset = gApp->GetWhiteSpaceWidth() * 8 + settings->textXMargin;
    SDL_UpdateTexture(m_memMapTexture, nullptr, m_memMap, dataCount*8);
    SDL_Rect dest = { m_contentArea.x + xOffset - m_horizScroll, m_contentArea.y + (charSetStart - charSetMin) * linesPerCharSet * m_zoomLevel - m_vertScroll, m_textureWidth * m_zoomLevel, m_textureHeight * m_zoomLevel };

    SDL_RenderCopy(GetRenderer(), m_memMapTexture, nullptr, &dest);

    m_renderedWidth = dest.w + xOffset;
    m_renderedHeight = ((memEnd - memStart) / 2048) * linesPerCharSet * m_zoomLevel;
}
void DockableWindow_MemoryDump::DrawBitmap()
{

}

void DockableWindow_MemoryDump::FreeTexture()
{
    if (m_memMapTexture)
    {
        SDL_DestroyTexture(m_memMapTexture);
        delete[] m_memMap;
        m_memMapTexture = nullptr;
        m_memMap = nullptr;
    }
}

int clamp_to_power_of_2(int x) 
{
    // If x is already a power of 2, return it
    if ((x & (x - 1)) == 0) return x;

    // Find the next highest power of 2
    int result = 1;
    while (result < x) result <<= 1;

    return result;
}

void DockableWindow_MemoryDump::CalcClampedMemoryRange(int& startMem, int& endMem, int &dataCount)
{
    startMem = SDL_clamp(m_memoryStart, 0, 0x10000);
    endMem = SDL_clamp(m_memoryEnd, 0, 0x10000);
    dataCount = 2048/64;

    switch (m_currentMode)
    {
        case MODE_Sprite:
        case MODE_SpriteMC:
            {
                startMem = m_memoryStart & 0x1ffc0;
                endMem = (m_memoryEnd + 63) & 0x1ffc0;
                int startSprite = m_memoryStart / 64;
                int endSprite = (m_memoryEnd+63) / 64;
                startMem = startSprite * 64;
                endMem = endSprite * 64;
            }
            break;

        case MODE_CharSet:
        case MODE_CharSetMC:
            {
                startMem = m_memoryStart & 0x1f800;
                endMem = (m_memoryEnd + 2047) & 0x1f800;
                int startCharSet = m_memoryStart / 2048;
                int endCharSet = (m_memoryEnd + 2047) / 2048;
                startMem = startCharSet * 2048;
                endMem = endCharSet * 2048;
                dataCount = SDL_clamp(clamp_to_power_of_2(m_dataCount), 1, 256);
            }
            break;

        case MODE_Bitmap:
        case MODE_BitmapMC:
            break;
    }
}


