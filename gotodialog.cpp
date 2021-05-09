#include "gotodialog.h"
#include "ui_gotodialog.h"

#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextCursor>

#include <QDebug>

GoToDialog::GoToDialog(QPlainTextEdit *textEdit, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GoToDialog),
    m_text_edit(textEdit)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Tool);

    QPushButton *btn_go_to = ui->buttonBox->addButton(QDialogButtonBox::StandardButton::Ok);
    btn_go_to->setText(tr("Atteindre"));
    QPushButton *btn_cancel = ui->buttonBox->addButton(QDialogButtonBox::StandardButton::Cancel);
    btn_cancel->setText(tr("Annuler"));

    connect(this, SIGNAL(accepted()), this, SLOT(onAccepted()));
}

GoToDialog::~GoToDialog()
{
    delete ui;
}

void GoToDialog::onAccepted()
{
    QTextCursor cursor = m_text_edit->textCursor();
    int nLine = ui->spinBox_line_number->value(); // line to reach

    // if line to reach is higher that the number of line, we set the cursor position at the end
    if (nLine > m_text_edit->document()->lineCount())
    {
        cursor.movePosition(QTextCursor::End);
    }
    // else we set the cursor position at the beginning and go down until the line is reached
    else
    {
        cursor.movePosition(QTextCursor::Start);
        while (--nLine != 0)
        {
            cursor.movePosition(QTextCursor::Down);
        }
    }
    m_text_edit->setTextCursor(cursor);
}
