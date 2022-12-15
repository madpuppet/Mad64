#include "common.h"
#include "DockableWindow_MemoryDump.h"

void DockableWindow_MemoryDump::OnChildRendererChange()
{
}



void DockableWindow_MemoryDump::DrawChild()
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
    for (u32 addr = m_memoryStart; addr <= m_memoryEnd; addr += 16)
    {
        if (y + settings->lineHeight >= m_clipArea.y && y <= m_clipArea.y + m_clipArea.w)
        {
            string text = FormatString("%04x  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ",
                addr, emu->GetByte(addr), emu->GetByte(addr + 1), emu->GetByte(addr + 2), emu->GetByte(addr + 3),
                emu->GetByte(addr + 4), emu->GetByte(addr + 5), emu->GetByte(addr + 6), emu->GetByte(addr + 7),
                emu->GetByte(addr + 8), emu->GetByte(addr + 9), emu->GetByte(addr + 10), emu->GetByte(addr + 11),
                emu->GetByte(addr + 12), emu->GetByte(addr + 13), emu->GetByte(addr + 14), emu->GetByte(addr + 15));

            auto ge = GraphicElement::CreateFromText(r, font, text.c_str(), col, x, y + settings->textYMargin);
            ge->Render(r);
            m_renderedWidth = SDL_max(m_renderedWidth, settings->textXMargin + ge->GetRect().w);
            delete ge;
        }
        y += settings->lineHeight;
        m_renderedHeight += settings->lineHeight;
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

void DockableWindow_MemoryDump::OnRangeChange(const string& text)
{
    m_memoryStart = 0x0000;
    m_memoryEnd = 0xffff;

    const string& startStr = m_memoryStartBox->GetText();
    const string& endStr = m_memoryEndBox->GetText();
    if (!startStr.empty())
    {
        m_memoryStart = TextToNumber(startStr);
    }
    if (!endStr.empty())
    {
        m_memoryEnd = TextToNumber(endStr);
    }
}

void DockableWindow_MemoryDump::CreateChildIcons()
{
    int width = gApp->GetWhiteSpaceWidth() * 12;
    m_memoryStartBox = new UIItem_TextBox("", "<start>", width, DELEGATE(DockableWindow_MemoryDump::OnRangeChange), DELEGATE(DockableWindow_MemoryDump::OnRangeChange));
    m_memoryEndBox = new UIItem_TextBox("", "<end>", width, DELEGATE(DockableWindow_MemoryDump::OnRangeChange), DELEGATE(DockableWindow_MemoryDump::OnRangeChange));
    m_memoryStartBox->SetTabCallbacks(DELEGATE(DockableWindow_MemoryDump::ActivateEndBox), DELEGATE(DockableWindow_MemoryDump::ActivateEndBox));
    m_memoryEndBox->SetTabCallbacks(DELEGATE(DockableWindow_MemoryDump::ActivateStartBox), DELEGATE(DockableWindow_MemoryDump::ActivateStartBox));

    m_titleIconsLeft.push_back(m_memoryStartBox);
    m_titleIconsLeft.push_back(m_memoryEndBox);
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


