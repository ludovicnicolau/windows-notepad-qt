#include "finddialog.h"
#include "ui_finddialog.h"

#include <QTextCursor>
#include <QTextDocument>
#include <QPlainTextEdit>

#include <QMessageBox>
#include <QPushButton>

#include <QDebug>

FindDialog::FindDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Tool);
    setFixedSize(335, 98);

    m_btn_next = ui->buttonBox->addButton(QDialogButtonBox::Yes);
    m_btn_next->setText(tr("Suivant"));
    m_btn_cancel = ui->buttonBox->addButton(QDialogButtonBox::Cancel);
    m_btn_cancel->setText(tr("Annuler"));

    connect(m_btn_next, SIGNAL(clicked()), this, SLOT(onFindClick()));
    connect(ui->lineEdit_search, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged(const QString&)));

    // connect(ui->pushButton_Next, SIGNAL(clicked), this, SLOT(onFindClick));
    // connect(ui->lineEdit_search, SIGNAL(textChanged), this, SLOT(onTextChanged));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(this, SIGNAL(rejected()), this, SLOT(onDialogRejected()));
}

FindDialog::~FindDialog()
{
    delete ui;
}

QString FindDialog::researchedText() const
{
    return ui->lineEdit_search->text();
}

bool FindDialog::isCaseSensitive() const
{
    return ui->checkBox_case_sensitive->isChecked();
}

void FindDialog::findNext(QPlainTextEdit * const textEdit)
{
    m_text_when_opened = ui->lineEdit_search->text();

    QTextDocument* document = textEdit->document();
    QTextCursor cursor = textEdit->textCursor();

    QString textToSearch = ui->lineEdit_search->text();

    if (ui->checkBox_case_sensitive->isChecked())
    {
        if (ui->radioButton_down->isChecked())
            cursor = document->find(textToSearch, cursor, QTextDocument::FindFlag::FindCaseSensitively);
        else
            cursor = document->find(textToSearch, cursor, QTextDocument::FindFlag::FindBackward | QTextDocument::FindFlag::FindCaseSensitively);
    }
    else
    {
        if (ui->radioButton_down->isChecked())
            cursor = document->find(textToSearch, cursor);
        else
            cursor = document->find(textToSearch, cursor, QTextDocument::FindFlag::FindBackward);
    }

    if (!cursor.isNull())
    {
        textEdit->setTextCursor(cursor);
        textEdit->setFocus();
        textEdit->activateWindow();
    }
    else
    {
        QMessageBox::information(this, tr("Bloc-notes"), tr("Impossible de trouver \"") + textToSearch + "\"");
    }
}

void FindDialog::showEvent(QShowEvent *event)
{
    m_text_when_opened = ui->lineEdit_search->text();
    QDialog::showEvent(event);
}

void FindDialog::onFindClick()
{
    emit findLaunched();
}

void FindDialog::onTextChanged(const QString &text)
{
    if (text.isEmpty())
        m_btn_next->setEnabled(false);
    else
        m_btn_next->setEnabled(true);
}

void FindDialog::onDialogRejected()
{
    ui->lineEdit_search->setText(m_text_when_opened);
}
