#include "common.h"
#include "uiItem_textBox.h"

#define BORDER_SIZE 2
#define SPACING 8

UIItem_TextBox::~UIItem_TextBox()
{
    delete m_geText;
    delete m_geHintText;
}

void UIItem_TextBox::Draw(SDL_Renderer* r)
{
	m_cursorAnim += TIMEDELTA * 5.0f;

    BuildGE(r);

    auto settings = gApp->GetSettings();
	{
		ClipRectScope crs(r, &m_area);
		if (m_isSelected)
			SDL_SetRenderDrawColor(r, 0, 0, 64, 255);
		else
			SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
		SDL_RenderFillRect(r, &m_area);

		if (m_isSelected)
		{
			SDL_SetRenderDrawColor(r, 255, 255, 0, 255);
			SDL_Point points[5] = {
				{m_area.x, m_area.y},
				{m_area.x + m_area.w - 1, m_area.y},
				{m_area.x + m_area.w - 1, m_area.y + m_area.h - 1},
				{m_area.x, m_area.y + m_area.h - 1},
				{m_area.x, m_area.y}
			};
			SDL_RenderDrawLines(r, points, 5);
		}

		if (m_text.empty() && !m_isSelected)
		{
			if (m_geHintText)
			{
				int offset = (m_area.w - m_geHintText->GetRect().w) / 2;
				m_geHintText->RenderAt(r, m_area.x + offset, m_area.y);
			}
		}
		else if (!m_text.empty())
		{
			if (m_geText)
			{
				m_geText->RenderAt(r, m_area.x + settings->textXMargin, m_area.y);
			}
		}

		if (m_isSelected)
		{
			string sub = m_text.substr(0, m_cursorPos);
			int textWidth;
			int x = TTF_SizeText(gApp->GetFont(), sub.c_str(), &textWidth, nullptr);
			SDL_Rect cursorRect = { m_area.x + settings->textXMargin + textWidth - 2, m_area.y, 4, m_area.h };
			int brightness = max(0, (int)(100 + cosf(m_cursorAnim) * 128));
			SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(r, 255, 255, 255, brightness);
			SDL_RenderFillRect(r, &cursorRect);
			SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
		}
	}

	if (m_geTitleText)
		m_geTitleText->RenderAt(r, m_area.x + m_area.w + SPACING, m_area.y);
}
void UIItem_TextBox::OnButtonDown(int button, int x, int y)
{
	auto settings = gApp->GetSettings();
    if (Contains(m_area, x, y))
    {
		if (button == 1)
		{
			if (!m_isSelected)
			{
				m_isSelected = true;
				m_cursorAnim = 0;
				gApp->SetCaptureTextInput(DELEGATE(UIItem_TextBox::OnCapturedTextInput));
				gApp->SetCaptureKeyInput(DELEGATE(UIItem_TextBox::OnCapturedKeyInput));

				// calculate cursor pos
				m_cursorPos = 0;
				while (m_cursorPos < m_text.size())
				{
					string sub = m_text.substr(0, m_cursorPos+1);
					int textWidth;
					TTF_SizeText(gApp->GetFont(), sub.c_str(), &textWidth, nullptr);
					int letterX = m_area.x + settings->textXMargin + textWidth;
					if (x < letterX)
						break;
					m_cursorPos++;
				}
			}
		}
		else if (button == 3)
		{
			SetText("");
		}
    }
}
void UIItem_TextBox::UpdateCursor(int x, int y)
{
	if (Overlaps(x, y))
		gApp->SetCursor(Cursor_IBeam);
}


int UIItem_TextBox::GetWidth()
{
    return m_geTitleText ? m_boxWidth + m_geTitleText->GetRect().w + SPACING*3 : m_boxWidth;
}
int UIItem_TextBox::GetHeight()
{
    auto settings = gApp->GetSettings();
    return settings->lineHeight;
}
void UIItem_TextBox::OnRendererChange(SDL_Renderer* r)
{
    DeleteClear(m_geText);
    DeleteClear(m_geHintText);
    DeleteClear(m_geTitleText);

    BuildGE(r);
}
void UIItem_TextBox::SetPos(int x, int y)
{
    auto settings = gApp->GetSettings();
    m_area = { x, y + BORDER_SIZE, m_boxWidth, settings->lineHeight - BORDER_SIZE * 2 };
}
bool UIItem_TextBox::Overlaps(int x, int y)
{
    return Contains(m_area, x, y);
}

void UIItem_TextBox::BuildGE(SDL_Renderer* r)
{
    auto font = gApp->GetFont();
    SDL_Color col = { 255, 255, 255, 255 };
    SDL_Color dark = { 128, 128, 128, 255 };
    if (!m_geText && !m_text.empty())
        m_geText = GraphicElement::CreateFromText(r, font, m_text.c_str(), col, 0, 0);
    if (!m_geHintText && !m_hintText.empty())
        m_geHintText = GraphicElement::CreateFromText(r, font, m_hintText.c_str(), dark, 0, 0);
    if (!m_geTitleText && !m_titleText.empty())
        m_geTitleText = GraphicElement::CreateFromText(r, font, m_titleText.c_str(), col, 0, 0);
}

void UIItem_TextBox::OnCapturedTextInput(bool lostCapture, const string &text)
{
	if (lostCapture)
	{
		m_isSelected = false;
	}
	else
	{
		u32string unicode_text = UTF8toUNICODE(text);
		for (auto ch : unicode_text)
		{
			if (ch <= 255)
			{
				m_text.insert(m_cursorPos++, 1, (u8)ch);
				m_cursorAnim = 0;
				if (m_onChange)
					m_onChange(m_text);
				DeleteClear(m_geText);
			}
		}
	}
}

void UIItem_TextBox::OnCapturedKeyInput(bool lostCapture, bool keyDown, u32 sym, u32 mod)
{
	if (lostCapture)
	{
		m_isSelected = false;
	}
	else if (keyDown)
	{
		switch (sym)
		{
			case SDLK_LEFT:
				if (m_cursorPos > 0)
				{
					m_cursorPos--;
					m_cursorAnim = 0;
				}
				return;
			case SDLK_RIGHT:
				if (m_cursorPos < m_text.size())
				{
					m_cursorPos++;
					m_cursorAnim = 0;
				}
				return;
			case SDLK_HOME:
				m_cursorPos = 0;
				m_cursorAnim = 0;
				return;
			case SDLK_END:
				m_cursorPos = (int)m_text.size();
				m_cursorAnim = 0;
				return;
			case SDLK_BACKSPACE:
				if (m_cursorPos > 0)
				{
					m_text.erase(--m_cursorPos, 1);
					m_cursorAnim = 0;
					if (m_onChange)
						m_onChange(m_text);
					DeleteClear(m_geText);
				}
				return;
			case SDLK_ESCAPE:
				gApp->SetCaptureKeyInput(nullptr);
				gApp->SetCaptureTextInput(nullptr);
				return;
			case SDLK_DELETE:
				if (m_cursorPos < m_text.size())
				{
					m_text.erase(m_cursorPos, 1);
					m_cursorAnim = 0;
					if (m_onChange)
						m_onChange(m_text);
					DeleteClear(m_geText);
				}
				return;
			case SDLK_RETURN:
				m_cursorAnim = 0;
				m_onEnter(m_text);
				return;
			case SDLK_TAB:
				if (mod & KMOD_SHIFT)
				{
					if (m_onShiftTab)
						m_onShiftTab();
				}
				else
				{
					if (m_onTab)
						m_onTab();
				}
				return;
			case SDLK_v:
				if (mod & KMOD_CTRL)
				{
					const char* text = SDL_GetClipboardText();
					m_text = "";
					for (const char* ch = text; *ch; ch++)
					{
						if (*ch == 0xd || *ch == 0xa)
							break;
						m_text += *ch;
					}
					m_cursorPos = (int)m_text.size();
					DeleteClear(m_geText);
					return;
				}
				break;
		}
	}
}

void UIItem_TextBox::SetText(const string& text)
{
	m_text = text;
	m_cursorPos = (int)text.size();
	DeleteClear(m_geText);
	if (m_onChange)
		m_onChange(m_text);
}

void UIItem_TextBox::SetSelected(bool selected)
{
	if (selected != m_isSelected)
	{
		m_isSelected = selected;
		if (selected)
		{
			gApp->SetCaptureTextInput(DELEGATE(UIItem_TextBox::OnCapturedTextInput));
			gApp->SetCaptureKeyInput(DELEGATE(UIItem_TextBox::OnCapturedKeyInput));
		}
		else
		{
			gApp->SetCaptureTextInput(nullptr);
			gApp->SetCaptureKeyInput(nullptr);
		}
	}
}




