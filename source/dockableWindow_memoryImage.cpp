#include "common.h"
#include "dockableWindow_memoryImage.h"

#define BORDER_SIZE 8

DockableWindow_MemoryImage::DockableWindow_MemoryImage(const string& title) : DockableWindow(title)
{
	m_memMapTexture = SDL_CreateTexture(GetRenderer(), SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, 256, 256);
	m_memMap = (u8*)malloc(65536);
	memset(m_memMap, 0, 65536);
	SDL_Rect memMapRect = { 0, 0, 256, 256 };
	SDL_UpdateTexture(m_memMapTexture, &memMapRect, m_memMap, 256);
	m_memMapDirty = false;
}


int DockableWindow_MemoryImage::GetScreenRenderWidth()
{
	return 512;
}
int DockableWindow_MemoryImage::GetScreenRenderHeight()
{
	return 512;
}

int DockableWindow_MemoryImage::GetContentHeight()
{
	return GetScreenRenderHeight() + BORDER_SIZE * 2;
}

int DockableWindow_MemoryImage::GetContentWidth()
{
	return GetScreenRenderWidth() + BORDER_SIZE * 2;
}

void DockableWindow_MemoryImage::DrawChild()
{
	auto settings = gApp->GetSettings();
	auto r = GetRenderer();

	if (m_memMapDirty)
	{
		SDL_UpdateTexture(m_memMapTexture, nullptr, m_memMap, 256);
		m_memMapDirty = false;
	}

	SDL_Rect destRect = { m_contentArea.x + BORDER_SIZE, m_contentArea.y + BORDER_SIZE, GetScreenRenderWidth(), GetScreenRenderHeight() };
	SDL_RenderCopy(r, m_memMapTexture, nullptr, &destRect);

	// draw a flashing box at the current location
	auto file = gApp->GetEditWindow()->GetActiveFile();
	if (file)
	{
		auto csi = file->GetCompileInfo();
		if (csi)
		{
			int line = gApp->GetEditWindow()->GetActiveLine();
			if (csi->m_lines.size() > line)
			{
				auto cli = csi->m_lines[line];
				if (!cli->data.empty())
				{
					int s = 4;
					int x = destRect.x + (cli->memAddr & 255) * 2;
					int y = destRect.y + (cli->memAddr >> 8) * 2;
					int brightness = (int)(sinf(m_markerAnim * 3.1452f * 4) * 120) + 128;
					SDL_SetRenderDrawColor(r, brightness, brightness, brightness, brightness);
					SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
					SDL_RenderDrawLine(r, x - s, y - s, x + s, y - s);
					SDL_RenderDrawLine(r, x + s, y - s, x + s, y + s);
					SDL_RenderDrawLine(r, x + s, y + s, x - s, y + s);
					SDL_RenderDrawLine(r, x - s, y + s, x - s, y - s);
					SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
				}
			}
		}
	}
}

void DockableWindow_MemoryImage::OnChildRendererChange()
{
	SDL_DestroyTexture(m_memMapTexture);

	m_memMapTexture = SDL_CreateTexture(GetRenderer(), SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, 256, 256);
	SDL_Rect memMapRect = { 0, 0, 256, 256 };
	SDL_UpdateTexture(m_memMapTexture, &memMapRect, m_memMap, 256);
}

void DockableWindow_MemoryImage::CreateChildIcons()
{
}

void DockableWindow_MemoryImage::OnMouseButtonDown(int button, int x, int y)
{
    DockableWindow::OnMouseButtonDown(button, x, y);
}

void DockableWindow_MemoryImage::OnMouseMotion(int xAbs, int yAbs, int xRel, int yRel)
{
    DockableWindow::OnMouseMotion(xAbs, yAbs, xRel, yRel);
}



