#include "qt/window/QtEulaWindow.h"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>

#include "utility/ResourcePaths.h"
#include "utility/text/TextAccess.h"

QtEulaWindow::QtEulaWindow(QWidget *parent, bool forceAccept)
	: QtWindow(false, parent)
	, m_forceAccept(forceAccept)
{
}

QSize QtEulaWindow::sizeHint() const
{
	return QSize(700, 600);
}

void QtEulaWindow::populateWindow(QWidget* widget)
{
	QVBoxLayout* layout = new QVBoxLayout(widget);
	layout->setContentsMargins(0, 0, 0, 0);

	if (m_forceAccept)
	{
		layout->addSpacing(5);

		QLabel* label = new QLabel("Please read and agree to our End User License Agreement:");
		QFont _font = label->font();
		_font.setBold(true);
		label->setFont(_font);
		layout->addWidget(label);
	}

	std::shared_ptr<TextAccess> text =
		TextAccess::createFromFile(ResourcePaths::getGuiPath().concatenate(L"installer/EULA.txt"));

	QTextEdit* licenseText = new QTextEdit();
	licenseText->setObjectName("textField");
	licenseText->setReadOnly(true);
	licenseText->setText(QString::fromStdString(text->getText()));
	layout->addWidget(licenseText);

	widget->setLayout(layout);
}

void QtEulaWindow::windowReady()
{
	updateTitle("End User License Agreement");

	if (m_forceAccept)
	{
		updateCloseButton("Quit");
		updateNextButton("I agree");
	}
	else
	{
		updateCloseButton("Close");
		setNextVisible(false);
	}

	setPreviousVisible(false);
}
