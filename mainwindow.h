#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void open();

    void onTextEditTextChanged();
    void onTextEditSelectionChanged();

    void onActionNewTriggered();
    void save();
    void saveAs();
    void saveToFile();
    void onActionPrintTriggered();
    void printPreview(class QPrinter*);

    void manageBreakline(bool automaticBreakline);
    void changeFont();
    void deleteSelectedText();

    void onActionFindTriggered(); // Open the dialog for finding text
    void onActionFindNextTriggered(); // Find next occurence of text, if no text was specified, open the dialog for finding text
    void findText();
    void onActionReplaceTriggered();
    void onActionGoToTriggered();
    void onActionDateTriggered();

    void onClipboardDataChanged();

    void onCursorPositionChanged();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void findFilenameFromFilepath(const QString& filepath);
    bool saveAndCloseFileConfirmation(); // Return true if the user decided to continue changing file
    void setFormattedWindowTitle();

    void initializeDocument();

    void setupStatusBar();

    void loadSettings();
    void saveSettings();

    Ui::MainWindow *ui;
    QString m_filename;
    QString m_filepath;

    QString m_text_at_save; // The complete text when it has been saved or loaded    

    class QPrinter* m_printer;
    class FindDialog* find_dialog;
    class ReplaceTextDialog *replace_text_dialog;
    class GoToDialog *go_to_dialog;

    class QLabel *m_status_bar_label;

    QSettings m_settings;
};

#endif // MAINWINDOW_H
