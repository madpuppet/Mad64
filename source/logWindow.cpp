#include "common.h"
#include "logWindow.h"

static const char* s_titles[] = { "Compiler", "Contextual Help", "Labels", "Memory", "Registers", "Emulator", "MemDump"};
static const char* s_short_titles[] = { "CMP", "HLP", "LAB", "MEM", "REG", "EMU", "DMP"};
static char s_titleChars[] = { 'C','H','L','M','R','E', 'D'};

string LogWindow::GetOpenLogs()
{
	string result = "";
	for (int i = 0; i < LF_MAX; i++)
	{
		if (m_logGroups[i].m_groupOpen)
			result += s_titleChars[i];
	}
	return result;
}

void LogWindow::SetOpenLogs(const string& logs)
{
	for (int i = 0; i < LF_MAX; i++)
		m_logGroups[i].m_groupOpen = false;

	for (auto ch : logs)
	{
		for (int i = 0; i < LF_MAX; i++)
		{
			if (ch == s_titleChars[i])
				m_logGroups[i].m_groupOpen = true;
		}
	}
}


LogWindow::LogWindow()
{
	m_markerAnim = 0.0f;
	auto settings = gApp->GetSettings();
	for (int i = 0; i < LF_MAX; i++)
	{
		m_logGroups[i].m_groupOpen = true;
		m_logGroups[i].m_geTitle = 0;
		m_logGroups[i].m_geIcon = 0;
	}
	m_dragMode = DRAG_None;
	m_scroll = 0;
	m_targetScroll = 0.0f;
	m_highlightRow = -1;
	m_autoScroll = 0;
	m_logGroups[2].m_groupOpen = false;
	m_emulatorZoom = 2;
	m_dumpMode = DUMP_Hex;

	BuildIcons();

	m_memMapTexture = SDL_CreateTexture(gApp->GetRenderer(), SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, 256, 256);
	m_memMap = (u8*)malloc(65536);
	memset(m_memMap, 0, 65536);
	SDL_Rect memMapRect = { 0, 0, 256, 256 };
	SDL_UpdateTexture(m_memMapTexture, &memMapRect, m_memMap, 256);
	m_memMapDirty = false;
}

LogWindow::~LogWindow()
{
}

GraphicElement* LogWindow::GetGroupTitleGE(LogFilter group)
{
	auto& lg = m_logGroups[group];

	if (!lg.m_geTitle)
	{
		auto settings = gApp->GetSettings();
		SDL_Color col = settings->helpGroupColor;
		lg.m_geTitle = GraphicElement::CreateFromText(gApp->GetFont(), s_titles[group], col, 0, 0);

		if (lg.m_geTitle == 0)
		{
			Log("FAILED TO CREATE GRAPHIC ELEMENT");
		}

	}
	return lg.m_geTitle;
}

void LogWindow::LogText(LogFilter filter, string text, int lineNmbr, int colIdx, u16 addr)
{
	auto settings = gApp->GetSettings();
	SDL_Color color;
	switch (colIdx)
	{
		case 0:
			color = settings->helpTitleColor;
			break;
		case 1:
			color = settings->helpBodyColor1;
			break;
		case 2:
			color = settings->helpBodyColor2;
			break;
	}

	auto logItem = new LogItem();
	logItem->col = color;
	logItem->text = text;
	logItem->line = lineNmbr;
	logItem->addr = addr;
	m_logGroups[filter].m_logLines.push_back(logItem);
}

void LogWindow::ClearAllLogs()
{
	for (int i = 0; i < LF_MAX; i++)
		ClearLog((LogFilter)i);
}

void LogWindow::ClearLog(LogFilter filter)
{
	auto settings = gApp->GetSettings();
	auto &lg = m_logGroups[filter];

	for (auto line : lg.m_logLines)
		delete line;
	lg.m_logLines.clear();
	if (filter == LF_Memory)
		m_memMapDirty = true;
}

void LogWindow::BuildIcons()
{
	auto settings = gApp->GetSettings();
	auto r = gApp->GetRenderer();
	SDL_Color col = { 255, 255, 255, 255 };
	for (int i = 0; i < LF_MAX; i++)
	{
		auto& lg = m_logGroups[i];
		lg.m_geIcon = GraphicElement::CreateFromText(gApp->GetFont(), s_short_titles[i], col, 0, 0);
	}
}

void LogWindow::LayoutIcons()
{
	auto settings = gApp->GetSettings();
	auto r = gApp->GetRenderer();
	int iconX = m_titleArea.x + settings->textXMargin;
	for (int i = 0; i < LF_MAX; i++)
	{
		auto& lg = m_logGroups[i];
		lg.m_geIcon->SetPos(iconX, m_titleArea.y + settings->textYMargin);
		iconX += settings->textXMargin*2 + lg.m_geIcon->GetRect().w;
	}
}

GraphicElement *LogWindow::LogItem::GetGE()
{
	if (!ge)
		ge = GraphicElement::CreateFromText(gApp->GetFont(), text.c_str(), col, 0, 0);
	return ge;
}

void LogWindow::Update()
{
	if (m_logGroups[LF_Registers].m_groupOpen)
	{
		// update registers
		auto cpu = gApp->GetEmulator()->GetCpu();
		auto vic = gApp->GetEmulator()->GetVic();
		Cpu6502::Registers& cpuRegs = cpu->Regs();
		Vic::Registers& vicRegs = vic->Regs();
		int rasterLine = vic->CurrentRasterLine();
		int rasterRow = vic->CurrentRasterRow();

		ClearLog(LF_Registers);
		LogText(LF_Registers, "CYCLE       PC   SR SP  A  X  Y   N V - B D I Z C");
		LogText(LF_Registers, FormatString("%08x    %04x %02x %02x  %02x %02x %02x  %d %d   %d %d %d %d %d",
			cpuRegs.frameCycle, cpuRegs.PC, cpuRegs.SR, cpuRegs.SP, cpuRegs.A, cpuRegs.X, cpuRegs.Y,
			(cpuRegs.SR & Cpu6502::SR_Negative) ? 1 : 0, (cpuRegs.SR & Cpu6502::SR_Overflow) ? 1 : 0, (cpuRegs.SR & Cpu6502::SR_Break) ? 1 : 0, (cpuRegs.SR & Cpu6502::SR_Decimal) ? 1 : 0,
			(cpuRegs.SR & Cpu6502::SR_Interrupt) ? 1 : 0, (cpuRegs.SR & Cpu6502::SR_Zero) ? 1 : 0, (cpuRegs.SR & Cpu6502::SR_Carry) ? 1 : 0));

		LogText(LF_Registers, "ROW COL     XSC YSC   CSL RSL   ECM MCM BMM");
		LogText(LF_Registers, FormatString("%03x %03x      %d   %d     %d   %d     %d   %d   %d", 
			rasterLine, rasterRow,
			vicRegs.control2 & Vic::XSCROLL, vicRegs.control1 & Vic::YSCROLL,
			vicRegs.control1 & Vic::CSEL ? 1 : 0, vicRegs.control1 & Vic::RSEL ? 1 : 0,
			vicRegs.control1 & Vic::ECM ? 1 : 0, vicRegs.control2 & Vic::MCM ? 1 : 0, vicRegs.control1 & Vic::BMM ? 1 : 0));
	}

	if (m_autoScroll)
	{
		m_targetScroll += TIMEDELTA * m_autoScroll;
	}
	ClampTargetScroll();
	m_scroll += (int)((m_targetScroll - (float)m_scroll) * 0.25f);

	m_markerAnim = fmodf(m_markerAnim + 1/60.0f, 1.0f);
}

void LogWindow::DrawLine(int lineIdx, int y, bool highlight)
{
	auto settings = gApp->GetSettings();
	auto r = gApp->GetRenderer();

	int brighten = highlight ? 16 : 0;
	int col = 24 - ((lineIdx & 1) ? 4 : 0) + brighten;
	SDL_Rect lineQuad = { m_logArea.x, y, m_logArea.w, settings->lineHeight };
	SDL_SetRenderDrawColor(r, col, col, col, 255);
	SDL_RenderFillRect(r, &lineQuad);
}

void LogWindow::Draw()
{
	auto settings = gApp->GetSettings();
	auto r = gApp->GetRenderer();
	auto emu = gApp->GetEmulator();

	int y = m_logArea.y - m_scroll;
	int lineIdx = 0;
	int fileHeight = CalcLogHeight();

	// draw toggle buttons
	SDL_RenderSetClipRect(r, &m_titleArea);
	SDL_SetRenderDrawColor(r, 64, 32, 0, 255);
	SDL_RenderFillRect(r, &m_titleArea);

	m_items.clear();
	for (int i = 0; i < LF_MAX; i++)
	{
		auto& lg = m_logGroups[i];

		SDL_SetRenderDrawColor(r, 64, 32, 0, 255);
		SDL_RenderFillRect(r, &lg.m_geIcon->GetRect());

		if (lg.m_groupOpen)
		{
			SDL_SetTextureColorMod(lg.m_geIcon->GetTexture(), 255, 255, 0);
		}
		else
		{
			SDL_SetTextureColorMod(lg.m_geIcon->GetTexture(), 96, 96, 0);
		}
		lg.m_geIcon->Render(r);
	}

	SDL_RenderSetClipRect(r, &m_logArea);

	int yMin = m_logArea.y;
	int yMax = m_logArea.y + m_logArea.h;
	for (int i = 0; i < LF_MAX; i++)
	{
		auto &lg = m_logGroups[i];
		if (!lg.m_groupOpen)
			continue;

		int x = m_logArea.x + settings->textXMargin;
		if (containsRange(yMin, yMax, y, y+settings->lineHeight))
		{
			DrawLine(lineIdx, y, lineIdx == m_highlightRow);
			GetGroupTitleGE((LogFilter)i)->RenderAt(r, x, y + settings->textYMargin);
		}
		lineIdx++;
		y += settings->lineHeight;

		if (i == LF_Memory)
		{
			if (containsRange(yMin, yMax, y, y + 512 + settings->textYMargin*2))
			{
				if (m_memMapDirty)
				{
					SDL_Rect rect = { 0, 0, 256, 256 };
						SDL_UpdateTexture(m_memMapTexture, &rect, m_memMap, 256);
						m_memMapDirty = false;
				}

				SDL_Rect destRect = { m_logArea.x + settings->textXMargin, y + settings->textYMargin, 512, 512 };
				SDL_RenderCopy(r, m_memMapTexture, nullptr, &destRect);

				MappedLogItem item;
				item.area = { destRect };
				item.group = LF_Memory;
				item.item = 0;
				m_items.push_back(item);

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
			y += 512 + settings->textYMargin * 2;
		}
		else if (i == LF_Emulator)
		{
			auto vic = gApp->GetEmulator()->GetVic();
			int h = vic->GetScreenHeight() * m_emulatorZoom;

			if (containsRange(yMin, yMax, y, y + h + settings->textYMargin*2))
			{
				vic->Render(m_logArea.x + settings->textXMargin, y + settings->textYMargin, m_emulatorZoom);
				MappedLogItem item;
				item.area = { x + settings->textXMargin, y + settings->textYMargin, m_logArea.w, h };
				item.group = i;
				item.item = -1;
				m_items.push_back(item);

				if (!gApp->IsEmulatorRunning())
				{
					// draw flashing box at current raster line / raster byte
					int rasterLine = vic->CurrentRasterLine();
					int rasterCol = vic->CurrentRasterRow();
					int sx = 4 * m_emulatorZoom + 4;
					int sy = 4;
					int xx = item.area.x + rasterCol * 8 * m_emulatorZoom + 4;
					int yy = item.area.y + rasterLine * m_emulatorZoom;
					int brightness = (int)(sinf(m_markerAnim * 3.1452f * 4) * 120) + 128;
					SDL_SetRenderDrawColor(r, brightness, brightness, brightness, brightness);
					SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
					SDL_RenderDrawLine(r, xx - sx, yy - sy, xx + sx, yy - sy);
					SDL_RenderDrawLine(r, xx + sx, yy - sy, xx + sx, yy + sy);
					SDL_RenderDrawLine(r, xx + sx, yy + sy, xx - sx, yy + sy);
					SDL_RenderDrawLine(r, xx - sx, yy + sy, xx - sx, yy - sy);
					SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
				}
			}
			y += h + settings->textYMargin * 2;
		}
		else if (i == LF_MemoryDump)
		{
			auto font = gApp->GetFont();
			auto emu = gApp->GetEmulator();
			int charWidth = gApp->GetWhiteSpaceWidth();

			MappedLogItem item;
			item.area = { m_logArea.x, y, m_logArea.w, settings->lineHeight * 4096 };
			item.group = LF_MemoryDump;
			item.item = 0;
			m_items.push_back(item);

			int firstLine = (yMin - y) / settings->lineHeight;
			int lastLine = (yMax - y) / settings->lineHeight;
			SDL_Color col = { 250,250,250,255};
			if (firstLine < 4096 && lastLine >= 0)
			{
				firstLine = SDL_clamp(firstLine, 0, 4095);
				lastLine = SDL_clamp(lastLine, 0, 4095);
				for (int l = firstLine; l < lastLine; l++)
				{
					int lineY = y + l * settings->lineHeight;
					u16 addr = l*16;
					if (gApp->IsMemoryBreakpointInRange(addr, 16))
					{
						// draw red under breakpoints
						for (int b = 0; b < 16; b++)
						{
							if (gApp->IsMemoryBreakpointInRange(addr+b, 1))
							{
								SDL_Rect area = { x + (6+b*3)*charWidth, lineY + settings->textYMargin, 2*charWidth, settings->lineHeight - settings->textYMargin*2 };
								SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
								SDL_RenderFillRect(r, &area);
							}
						}
					}

					string text = FormatString("%04x  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ",
						addr, emu->GetByte(addr), emu->GetByte(addr+1), emu->GetByte(addr+2), emu->GetByte(addr+3),
						emu->GetByte(addr+4), emu->GetByte(addr+5), emu->GetByte(addr+6), emu->GetByte(addr+7),
						emu->GetByte(addr+8), emu->GetByte(addr+9), emu->GetByte(addr+10), emu->GetByte(addr+11),
						emu->GetByte(addr+12), emu->GetByte(addr+13), emu->GetByte(addr+14), emu->GetByte(addr+15));

					auto ge = GraphicElement::CreateFromText(font, text.c_str(), col, x, lineY + settings->textYMargin);
					ge->Render(r);
					delete ge;
				}
			}

			y += settings->lineHeight * 4096;
		}
		else
		{
			int groupItem = 0;
			for (auto line : lg.m_logLines)
			{
				if (containsRange(yMin, yMax, y, y + settings->lineHeight))
				{
					if (settings->renderLineBackgrounds)
						DrawLine(lineIdx, y, lineIdx == m_highlightRow);
					line->GetGE()->RenderAt(r, x, y + settings->textYMargin);

					if (i == LF_LabelHelp)
					{
						SDL_Color col = { 255,255,255 };
						auto ge = GraphicElement::CreateFromText(gApp->GetFont(), 
							FormatString("%02x %02x %02x %02x", emu->GetByte(line->addr), emu->GetByte(line->addr + 1), 
								emu->GetByte(line->addr + 2), emu->GetByte(line->addr + 3)).c_str(),  col,
							    x + line->GetGE()->GetRect().w + 40, y + settings->textYMargin);
						ge->Render(r);
						delete ge;
					}

					MappedLogItem item;
					item.area = { x, y, m_logArea.w, settings->lineHeight };
					item.group = i;
					item.item = groupItem;
					m_items.push_back(item);
				}

				lineIdx++;
				y += settings->lineHeight;
				groupItem++;
			}
		}
	}

	// draw scroll bar
	int barY1, barY2;
	CalcVertScrollBar(barY1, barY2);

	SDL_Rect BarBack = { m_logArea.x + m_logArea.w - settings->scrollBarWidth, m_logArea.y, settings->scrollBarWidth, m_logArea.h };
	SDL_Rect Bar = { BarBack.x + 4, barY1, settings->scrollBarWidth - 4, barY2 - barY1 };
	SDL_SetRenderDrawColor(r, 0, 0, 32, 255);
	SDL_RenderFillRect(r, &BarBack);
	if (m_dragMode == DRAG_LogVertScroll)
	{
		SDL_SetRenderDrawColor(r, 255, 255, 0, 255);
	}
	else
	{
		SDL_SetRenderDrawColor(r, 64, 64, 255, 255);
	}
	SDL_RenderFillRect(r, &Bar);
	SDL_RenderSetClipRect(r, nullptr);
}

int LogWindow::CalcLogHeight()
{
	auto settings = gApp->GetSettings();
	auto vic = gApp->GetEmulator()->GetVic();
	int y = 0;
	for (int i = 0; i < LF_MAX; i++)
	{
		auto& lg = m_logGroups[(LogFilter)i];
		if (lg.m_groupOpen)
		{
			switch (i)
			{
				case LF_Memory:
					y += settings->lineHeight + 512 + settings->textYMargin*2;
					break;
				case LF_Emulator:
					y += settings->lineHeight + vic->GetScreenHeight() * m_emulatorZoom + settings->textYMargin * 2;
					break;
				case LF_MemoryDump:
					y += settings->lineHeight * 4096;
					break;
				default:
					y += settings->lineHeight * (int)lg.m_logLines.size() + settings->lineHeight;
					break;
			}
		}
	}


	return y;
}

bool LogWindow::CalcVertScrollBar(int& start, int& end)
{
	start = 0;
	end = 0;
	int fileHeight = CalcLogHeight();

	int viewStart = m_scroll;
	int viewEnd = m_scroll + m_logArea.h;
	float relStart = SDL_clamp((float)viewStart / (float)fileHeight, 0.0f, 1.0f);
	float relEnd = SDL_clamp((float)viewEnd / (float)fileHeight, 0.0f, 1.0f);
	start = (int)(m_logArea.y + relStart * m_logArea.h);
	end = (int)(m_logArea.y + relEnd * m_logArea.h);

	return relStart > 0.0f || relEnd < 1.0f;
}

bool LogWindow::FindLogLineAt(int x, int y, int& line)
{
	int group, item;
	if (FindLogItemAt(x, y, group, item))
	{
		return FindLogLine(group, item, line);
	}
	line = -1;
	return false;
}


bool LogWindow::FindLogLine(int group, int item, int& line)
{
	if (group == LF_Memory)
	{
		auto file = gApp->GetEditWindow()->GetActiveFile();
		if (file)
		{
			auto csi = file->GetCompileInfo();
			if (csi)
			{
				for (auto cli : csi->m_lines)
				{
					if (!cli->data.empty() && cli->memAddr <= (u32)item && cli->memAddr+cli->data.size() > (u32)item)
					{
						line = cli->lineNmbr;
						return true;
					}
				}
			}
		}
	}
	else if (group == LF_MemoryDump)
	{
		return false;
	}
	else if (item != -1)
	{
		line = m_logGroups[group].m_logLines[item]->line;
		return line != -1;
	}
	return false;
}

bool LogWindow::FindGroupItemAt(int x, int y, int& group)
{
	auto settings = gApp->GetSettings();
	if (Contains(m_titleArea, x, y))
	{
		for (int i = 0; i < LF_MAX; i++)
		{
			if (Contains(m_logGroups[i].m_geIcon->GetRect(), x, y))
			{
				group = i;
				return true;
			}
		}
	}
	group = -1;
	return false;
}

bool LogWindow::FindLogItemAt(int x, int y, int& group, int& item)
{
	auto settings = gApp->GetSettings();
	for (auto& i : m_items)
	{
		if (Contains(i.area, x, y))
		{
			group = i.group;
			if (group == LF_Memory)
			{
				item = ((y - i.area.y) / 2) * 256 + ((x - i.area.x) / 2);
			}
			else if (group == LF_MemoryDump)
			{
				int charWidth = gApp->GetWhiteSpaceWidth();
				int row = (y - i.area.y) / settings->lineHeight;
				int charCol = (x - i.area.x) / charWidth;
				if (charCol > 6)
				{
					int col = (charCol - 6) / 3;
					int colX = (col*3+6+1) * charWidth + i.area.x;
					if (x > colX)
					{
						item = row * 16 + col;
					}
					else
					{
						item = -1;
					}
				}
				else
				{
					item = -1;
				}
			}
			else
			{
				item = i.item;
			}
			return true;
		}
	}
	return false;
}

void LogWindow::SnapScrollBarToMouseY(int y)
{
	float rel = (float)(y - m_logArea.y) / (float)m_logArea.h;
	int fileHeight = CalcLogHeight();
	m_targetScroll = fileHeight * rel - m_logArea.h * 0.5f;
	ClampTargetScroll();
}

void LogWindow::LogTextArray(LogFilter filter, const char** textArray, int col)
{
	const char** t = textArray;
	while (*t)
	{
		LogText(filter, string(*t), -1, col);
		t++;
	}
}

void LogWindow::SelectCursor(int x, int y)
{
	if (Contains(m_titleArea, x, y))
	{
		for (auto &group : m_logGroups)
		{
			if (Contains(group.m_geIcon->GetRect(), x, y))
			{
				gApp->SetCursor(Cursor_Hand);
				return;
			}
		}
	}
	else if (Contains(m_logArea, x, y))
	{
		int group, item, line;
		if (FindLogItemAt(x, y, group, item))
		{
			if ((group == LF_MemoryDump && item != -1) || FindLogLine(group, item, line))
				gApp->SetCursor(Cursor_Hand);
			return;
		}
	}
	gApp->SetCursor(Cursor_Arrow);
}

void LogWindow::OnMouseDown(SDL_Event* event)
{
	int group, line, item;
	if (FindGroupItemAt(event->button.x, event->button.y, group))
	{
		if (event->button.button == 3)
		{
			for (int i = 0; i < LF_MAX; i++)
				m_logGroups[i].m_groupOpen = (group == i);
		}
		else
		{
			m_logGroups[group].m_groupOpen = !m_logGroups[group].m_groupOpen;
		}
		ClampTargetScroll();
	}
	else if (FindLogItemAt(event->button.x, event->button.y, group, item))
	{
		switch (group)
		{
			case LF_CompilerWarning:
			case LF_InstructionHelp:
			case LF_LabelHelp:
			case LF_Memory:
			case LF_Registers:
			case LF_Emulator:
				if (FindLogLine(group, item, line))
				{
					gApp->GetEditWindow()->GotoLineCol(line, 0, MARK_None, true);
				}
				return;
			case LF_MemoryDump:
				{
					if (item != -1)
					{
						if (event->button.button == 3)
							gApp->ClearAllMemoryBreakpoints();

						gApp->ToggleMemoryBreakpoint(item);
					}
				}
				return;
		}
	
	}

	else if (FindLogLineAt(event->button.x, event->button.y, line))
	{
	}
}

void LogWindow::SetRect(const SDL_Rect& area)
{
	auto settings = gApp->GetSettings();
	m_area = area;
	m_titleArea = { area.x, area.y, area.w, settings->lineHeight };
	m_logArea = { area.x, area.y + settings->lineHeight, area.w, area.h - settings->lineHeight };
	LayoutIcons();
}

void LogWindow::OnMouseMotion(SDL_Event* event)
{
	auto settings = gApp->GetSettings();
	int x = event->motion.x;
	int y = event->motion.y;

	if (x < m_logArea.x || y >= m_logArea.x + m_logArea.w)
		m_highlightRow = -1;
	else
		m_highlightRow = (y - m_logArea.y + m_scroll) / settings->lineHeight;
}


void LogWindow::OnMouseWheel(SDL_Event* e)
{
	m_targetScroll += (e->wheel.preciseY * -80.0f);
	ClampTargetScroll();
}

void LogWindow::ClampTargetScroll()
{
	int fileHeight = CalcLogHeight();
	int maxScroll = max(0, fileHeight - m_logArea.h);
	m_targetScroll = SDL_clamp(m_targetScroll, 0.0f, (float)maxScroll);
}

void LogWindow::OnMouseUp(SDL_Event* e)
{
	m_autoScroll = 0;
}

