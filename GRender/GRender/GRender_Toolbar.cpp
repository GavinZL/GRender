#include "GRender.h"

void GRender::initToolBar()
{
	m_toolBar = addToolBar(tr("ToolBar"));
	m_toolBar->setMinimumHeight(50);
	m_toolBar->setIconSize(QSize(100, 50));
	m_toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

	m_testButton = new QAction(QStringLiteral("Button"), this);
	m_toolBar->addAction(m_testButton);
	connect(m_testButton, &QAction::triggered, [this]()
	{
		// todo...
	});
}