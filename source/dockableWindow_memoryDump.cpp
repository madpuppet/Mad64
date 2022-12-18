#include "common.h"
#include "dockableWindow_memoryDump.h"

void DockableWindow_MemoryDump::OnChildRendererChange()
{
    RecreateTexture();
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
            DrawSprite();
            break;
        case MODE_SpriteMC:
            DrawSpriteMC();
            break;
        case MODE_CharSet:
            DrawCharSet();
            break;
        case MODE_CharSetMC:
            DrawCharSetMC();
            break;
        case MODE_Bitmap:
            DrawBitmap();
            break;
        case MODE_BitmapMC:
            DrawBitmapMC();
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

    RecreateTexture();
}

void DockableWindow_MemoryDump::RecreateTexture()
{
    if (m_memMapTexture)
    {
        SDL_DestroyTexture(m_memMapTexture);
        delete[] m_memMap;
        m_memMapTexture = nullptr;
        m_memMap = nullptr;
    }

    switch (m_currentMode)
    {
        case MODE_Sprite:
        case MODE_SpriteMC:
            {
                int width = 24;
                int height = ((m_memoryEnd - m_memoryStart + 63) / 64) * 21;
                m_memMapTexture = SDL_CreateTexture(GetRenderer(), SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height);
                m_memMap = (u8*)malloc(width * height * 3);
            }
            break;

        case MODE_CharSet:
        case MODE_CharSetMC:
            break;

        case MODE_Bitmap:
        case MODE_BitmapMC:
            break;
    }
}

void DockableWindow_MemoryDump::OnRangeChange(const string& text)
{
    m_memoryStart = 0x0000;
    m_memoryEnd = 0xffff;
    m_dataCount = 16;

    const string& startStr = m_memoryStartBox->GetText();
    const string& endStr = m_memoryEndBox->GetText();
    const string& dataStr = m_dataCountBox->GetText();
    if (!startStr.empty())
    {
        m_memoryStart = TextToNumber(startStr);
    }
    if (!endStr.empty())
    {
        m_memoryEnd = TextToNumber(endStr);
    }
    if (!dataStr.empty())
    {
        m_dataCount = SDL_clamp(TextToNumber(dataStr), 1, 256);
    }
}

void DockableWindow_MemoryDump::CreateChildIcons()
{
    int width = gApp->GetWhiteSpaceWidth() * 12;
    m_memoryStartBox = new UIItem_TextBox("", "<start>", width, DELEGATE(DockableWindow_MemoryDump::OnRangeChange), DELEGATE(DockableWindow_MemoryDump::OnRangeChange));
    m_memoryEndBox = new UIItem_TextBox("", "<end>", width, DELEGATE(DockableWindow_MemoryDump::OnRangeChange), DELEGATE(DockableWindow_MemoryDump::OnRangeChange));
    m_memoryStartBox->SetTabCallbacks(DELEGATE(DockableWindow_MemoryDump::ActivateEndBox), DELEGATE(DockableWindow_MemoryDump::ActivateEndBox));
    m_memoryEndBox->SetTabCallbacks(DELEGATE(DockableWindow_MemoryDump::ActivateStartBox), DELEGATE(DockableWindow_MemoryDump::ActivateStartBox));
    m_dataCountBox = new UIItem_TextBox("Count", "<cnt>", width, DELEGATE(DockableWindow_MemoryDump::OnRangeChange), DELEGATE(DockableWindow_MemoryDump::OnRangeChange));

    int modeButtonWidth = gApp->GetWhiteSpaceWidth() * 10;
    vector<string> modes = { "Hex8", "Hex16", "Bin8", "Bin16", "Petsci", "Sprite", "SpriteMC", "CharSet", "CharSetMC", "Bitmap", "BitmapMC", };
    m_modeButton = new UIItem_EnumButton(0, modes, modeButtonWidth, DELEGATE(DockableWindow_MemoryDump::OnModeChange));

    m_titleIconsLeft.push_back(m_memoryStartBox);
    m_titleIconsLeft.push_back(m_memoryEndBox);
    m_titleIconsLeft.push_back(m_dataCountBox);
    m_titleIconsLeft.push_back(m_modeButton);
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
    // calculate area of screen that is visible
}
void DockableWindow_MemoryDump::DrawSpriteMC()
{

}
void DockableWindow_MemoryDump::DrawCharSet()
{

}
void DockableWindow_MemoryDump::DrawCharSetMC()
{

}
void DockableWindow_MemoryDump::DrawBitmap()
{

}
void DockableWindow_MemoryDump::DrawBitmapMC()
{

}

