#ifndef REPLACETEXTDIALOG_H
#define REPLACETEXTDIALOG_H

#include <QDialog>

namespace Ui {
class ReplaceTextDialog;
}

class QPlainTextEdit;

class ReplaceTextDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceTextDialog(QPlainTextEdit *textEdit, QWidget *parent = nullptr);
    ~ReplaceTextDialog();

    void setTextToReplace(const QString& textToReplace);

private slots:
    bool findNext();
    void onPushButtonFindNextClicked();
    void onPushButtonReplaceClicked();
    void onPushButtonReplaceAllClicked();

private:
    Ui::ReplaceTextDialog *ui;

    QPlainTextEdit *m_text_edit;
};

#endif // REPLACETEXTDIALOG_H
