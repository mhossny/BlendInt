/*
 * Qt5Context.cpp
 */

#include "Qt5Context.hpp"
#include <QtGui/QCursor>

Qt5Context::Qt5Context (QWindow* window)
: BI::Context (), m_qt_window(window), m_layout(0)
{
	m_layout = Manage(new MainLayout);
	m_layout->Resize(size());

	PushBack(m_layout);

	events()->connect(resized(), this , &Qt5Context::OnResizeLayout);
}

Qt5Context::~Qt5Context ()
{
}

void Qt5Context::SetCursor(int cursor_type)
{
	if(m_qt_window) {

		m_qt_window->setCursor(QCursor(static_cast<Qt::CursorShape>(cursor_type)));
	}
}

void Qt5Context::OnResizeLayout(const BI::Size& size)
{
	m_layout->Resize(size);
}
