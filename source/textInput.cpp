#include "common.h"
#include "textInput.h"

TextInput::TextInput(DMFastDelegate::FastDelegate1<const string &> onEnter)
{
	m_onEnter = onEnter;
	m_gc = new GraphicChunk();
}

TextInput::~TextInput()
{
	delete m_gc;
}

void TextInput::Draw()
{
	auto r = gApp->GetRenderer();
	auto settings = gApp->GetSettings();

	SDL_Color backCol = { 0, 22, 0, 255 };
	if (m_isActive)
		backCol = { 16, 32, 16, 255 };

	SDL_SetRenderDrawColor(r, 0, 22, 0, 255);
	SDL_RenderFillRect(r, &m_area);

	m_gc->Draw();

	if (m_isActive)
	{
		string sub = m_text.substr(0, m_cursorPos);
		int textWidth;
		int x = TTF_SizeText(gApp->GetFont(), sub.c_str(), &textWidth, nullptr);
		SDL_Rect cursorRect = { m_area.x + settings->textXMargin + textWidth, m_area.y, 4, settings->lineHeight };
		int brightness = max(0, (int)(100 + cosf(m_cursorAnim) * 128));
		SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(r, 255, 255, 255, brightness);
		SDL_RenderFillRect(r, &cursorRect);
		SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
	}
}

void TextInput::OnKeyDown(SDL_Event* e)
{
	switch (e->key.keysym.sym)
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
				Visualize();
			}
			return;
		case SDLK_DELETE:
			if (m_cursorPos < m_text.size())
			{
				m_text.erase(m_cursorPos, 1);
				m_cursorAnim = 0;
				Visualize();
			}
			return;
		case SDLK_RETURN:
			m_cursorAnim = 0;
			m_onEnter(m_text);
			return;
	}

	char ch = e->key.keysym.sym;
	if (ch >= SDLK_SPACE && ch <= SDLK_z)
	{
		ch = KeySymToAscii(e->key.keysym);
		m_text.insert(m_cursorPos++, 1, ch);
		m_cursorAnim = 0;
		Visualize();
	}
}

void TextInput::Visualize()
{
	SDL_Color col = { 255,255,255,255 };
	auto ge = GraphicElement::CreateFromText(gApp->GetFont(), m_text.c_str(), col, m_area.x + gApp->GetSettings()->textXMargin, m_area.y + gApp->GetSettings()->textYMargin);
	m_gc->Clear();
	m_gc->Add(ge);
}

void TextInput::Update()
{
	if (m_isActive)
	{
		m_cursorAnim += TIMEDELTA * 5.0f;
	}
}

