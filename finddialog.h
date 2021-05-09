#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>

namespace Ui {
class SearchDialog;
}

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);
    ~FindDialog();

    QString researchedText() const;
    bool isCaseSensitive() const;

    void findNext(class QPlainTextEdit * const textEdit);

protected:
    virtual void showEvent(QShowEvent *event) override;

signals:
    void findLaunched();

private slots:
    void onFindClick();
    void onTextChanged(const QString& text);
    void onDialogRejected();

private:
    Ui::SearchDialog *ui;
    QString m_text_when_opened;

    class QPushButton *m_btn_next;
    class QPushButton *m_btn_cancel;
};

#endif // SEARCHDIALOG_H
