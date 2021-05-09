#ifndef GOTODIALOG_H
#define GOTODIALOG_H

#include <QDialog>

namespace Ui {
class GoToDialog;
}

class QPlainTextEdit;

class GoToDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GoToDialog(QPlainTextEdit *textEdit, QWidget *parent = nullptr);
    ~GoToDialog();

private slots:
    void onAccepted();

private:
    Ui::GoToDialog *ui;

    QPlainTextEdit *m_text_edit;
};

#endif // GOTODIALOG_H
