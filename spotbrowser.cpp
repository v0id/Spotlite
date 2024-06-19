#include "spotbrowser.h"
#include <QToolBar>
#include <QToolButton>
#include <QWebView>
#include <QVBoxLayout>


SpotBrowser::SpotBrowser(QWidget *parent) :
    QWidget(parent)
{
    /* Webbrowser spul */
    _web = new QWebView(this);

    /* Toolbar spul */
    QToolBar *toolbar = new QToolBar( tr("Navigatie"), this);
    _prev = new QToolButton(toolbar);
    _prev->setIcon( QIcon(":/icons/resultset_previous.png") );
    _prev->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _next = new QToolButton(toolbar);
    _next->setIcon( QIcon(":/icons/resultset_next.png") );
    _next->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbar->addWidget(_prev);
    toolbar->addWidget(_next);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(toolbar);
    layout->addWidget(_web);
    setLayout(layout);
}

