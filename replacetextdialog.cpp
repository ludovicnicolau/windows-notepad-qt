#include "replacetextdialog.h"
#include "ui_replacetextdialog.h"

#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextCursor>

#include <QDebug>

ReplaceTextDialog::ReplaceTextDialog(QPlainTextEdit *textEdit, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReplaceTextDialog),
    m_text_edit(textEdit)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Tool);

    QPushButton *btn_nxt = ui->buttonBox->addButton(tr("Suivant"), QDialogButtonBox::ActionRole);
    QPushButton *btn_replace = ui->buttonBox->addButton(tr("Remplacer"), QDialogButtonBox::ActionRole);
    QPushButton *btn_replace_all = ui->buttonBox->addButton(tr("Remplacer tout"), QDialogButtonBox::ActionRole);
    QPushButton *btn_cancel = ui->buttonBox->addButton(QDialogButtonBox::Cancel);
    btn_cancel->setText(tr("Annuler"));

    connect(btn_nxt, SIGNAL(clicked()), this, SLOT(onPushButtonFindNextClicked()));
    connect(btn_replace, SIGNAL(clicked()), this, SLOT(onPushButtonReplaceClicked()));
    connect(btn_replace_all, SIGNAL(clicked()), this, SLOT(onPushButtonReplaceAllClicked()));
}

ReplaceTextDialog::~ReplaceTextDialog()
{
    delete ui;
}

void ReplaceTextDialog::setTextToReplace(const QString &textToReplace)
{
    ui->lineEdit_find->setText(textToReplace);
}

bool ReplaceTextDialog::findNext()
{
    QString textToReplace = ui->lineEdit_find->text();
    if (ui->checkBox_case_sensitive->isChecked())
    {
        return m_text_edit->find(textToReplace, QTextDocument::FindCaseSensitively);
    }
    else
    {
        return m_text_edit->find(textToReplace);
    }
}

void ReplaceTextDialog::onPushButtonFindNextClicked()
{
    bool found = findNext();
    if (!found)
    {
        QMessageBox::information(this, tr("Bloc-notes"), tr("Impossible de trouver \"") + ui->lineEdit_find->text() + "\"");
    }
}

void ReplaceTextDialog::onPushButtonReplaceClicked()
{
    QString textToReplace = ui->lineEdit_find->text();
    if (!textToReplace.isEmpty() && m_text_edit->textCursor().selectedText() == textToReplace)
    {
        m_text_edit->insertPlainText(ui->lineEdit_replace_by->text());
    }
    else
    {
        bool found = findNext();
        if (!found)
        {
            QMessageBox::information(this, tr("Bloc-notes"), tr("Impossible de trouver \"") + textToReplace + "\"");
        }
    }
}

void ReplaceTextDialog::onPushButtonReplaceAllClicked()
{
    m_text_edit->moveCursor(QTextCursor::Start);
    while (findNext())
    {
        m_text_edit->insertPlainText(ui->lineEdit_replace_by->text());
    }
}
