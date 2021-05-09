#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDateTime>
#include <QFileDialog>
#include <QFontDialog>
#include <QMessageBox>
#include <QLabel>
#include <QPainter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QPushButton>
#include <QClipboard>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextStream>

#include "finddialog.h"
#include "gotodialog.h"
#include "replacetextdialog.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), m_filename(tr("Sans titre")), m_filepath(""),
    m_printer(new QPrinter()), find_dialog(new FindDialog(this)),
    m_settings("Ludovic Nicolau", "Notepad", this)
{
    ui->setupUi(this);

    setupStatusBar();

    setFormattedWindowTitle();

    connect(find_dialog, &FindDialog::findLaunched, this, &MainWindow::findText);

    connect(ui->textEdit, &QPlainTextEdit::textChanged, this, &MainWindow::onTextEditTextChanged);
    connect(ui->textEdit, &QPlainTextEdit::selectionChanged, this, &MainWindow::onTextEditSelectionChanged);

    // File menu
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onActionNewTriggered);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::save);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveAs);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::open);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionPrint, SIGNAL(triggered()), this, SLOT(onActionPrintTriggered()));

    // Edit menu
    connect(ui->actionDelete, &QAction::triggered, this, &MainWindow::deleteSelectedText);

    connect(ui->actionFind, &QAction::triggered, this, &MainWindow::onActionFindTriggered);
    connect(ui->actionFindNext, &QAction::triggered, this, &MainWindow::onActionFindNextTriggered);

    connect(ui->actionReplace, SIGNAL(triggered()), this, SLOT(onActionReplaceTriggered()));

    connect(ui->actionGoTo, SIGNAL(triggered()), this, SLOT(onActionGoToTriggered()));
    connect(ui->action_date, SIGNAL(triggered()), this, SLOT(onActionDateTriggered()));

    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::onClipboardDataChanged);
    onClipboardDataChanged();

    // Format menu
    connect(ui->actionAutomaticBreakline, &QAction::toggled, this, &MainWindow::manageBreakline);
    connect(ui->actionFont, &QAction::triggered, this, &MainWindow::changeFont);

    // Display menu
    connect(ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));

    replace_text_dialog = new ReplaceTextDialog(ui->textEdit, this);
    go_to_dialog = new GoToDialog(ui->textEdit, this);

    loadSettings();
}

MainWindow::~MainWindow()
{
    delete find_dialog;
    delete replace_text_dialog;
    delete go_to_dialog;
    delete m_printer;
    delete ui;
}

void MainWindow::open()
{
    bool bCanChange = true;
    if (m_text_at_save != ui->textEdit->toPlainText())
    {
       bCanChange = saveAndCloseFileConfirmation();
    }

    if (!bCanChange)
        return;

    QString filepath = QFileDialog::getOpenFileName(this, tr("Ouvrir un fichier"), QString(), tr("Fichiers texte (*.txt)"));
    // If the user did not choose any file, we stop the function here
    if (filepath == "")
    {
        return;
    }

    m_filepath = filepath;

    findFilenameFromFilepath(m_filepath);

    setFormattedWindowTitle();

    QFile file(m_filepath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream in(&file);

    QString fileText = in.readAll();

    ui->textEdit->setPlainText(fileText);

    m_text_at_save = ui->textEdit->toPlainText();
}

void MainWindow::onTextEditTextChanged()
{
    QTextDocument *document = ui->textEdit->document();
    if (document->isEmpty())
    {
        ui->actionFind->setDisabled(true);
        ui->actionFindNext->setDisabled(true);
    }
    else
    {
        ui->actionFind->setEnabled(true);
        ui->actionFindNext->setEnabled(true);
    }

}

void MainWindow::onTextEditSelectionChanged()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    if (cursor.selection().isEmpty())
    {
        ui->actionCopy->setEnabled(false);
        ui->actionCut->setEnabled(false);
        ui->actionDelete->setEnabled(false);
    }
    else
    {
        ui->actionCopy->setEnabled(true);
        ui->actionCut->setEnabled(true);
        ui->actionDelete->setEnabled(true);
    }
}

void MainWindow::onActionNewTriggered()
{
    bool bCanChange = true;
    if (m_text_at_save != ui->textEdit->toPlainText())
    {
       bCanChange = saveAndCloseFileConfirmation();
    }

    if (bCanChange)
    {
        initializeDocument();
    }
}

void MainWindow::save()
{
    if (m_filepath == "")
        saveAs();
    else
        saveToFile();
}

void MainWindow::saveAs()
{
    m_filepath = QFileDialog::getSaveFileName(this, tr("Enregistrer un fichier"), QString(), tr("Fichiers texte (*.txt)"));

    findFilenameFromFilepath(m_filepath);

    setFormattedWindowTitle();

    saveToFile();
}

void MainWindow::saveToFile()
{
    QFile file(m_filepath);
    if ( !file.open(QIODevice::WriteOnly | QIODevice::Text) )
    {
        return;
    }

    QTextStream stream(&file);
    stream << ui->textEdit->toPlainText();

    file.close();

    m_text_at_save = ui->textEdit->toPlainText();
}

void MainWindow::onActionPrintTriggered()
{
    QPrintPreviewDialog dialog(m_printer, this);
    connect(&dialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printPreview(QPrinter*)));
    dialog.exec();
}

void MainWindow::printPreview(QPrinter* printer)
{
    ui->textEdit->document()->print(printer);
}

void MainWindow::manageBreakline(bool automaticBreakline)
{
    if (automaticBreakline)
    {
        ui->textEdit->setLineWrapMode(QPlainTextEdit::LineWrapMode::WidgetWidth);
        ui->actionGoTo->setEnabled(false);
        ui->action_status_bar->setEnabled(false);
        ui->action_status_bar->setChecked(false);
    }
    else
    {
        ui->textEdit->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
        ui->actionGoTo->setEnabled(true);
        ui->action_status_bar->setEnabled(true);
    }
}

void MainWindow::changeFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, ui->textEdit->font());

    if (ok)
        ui->textEdit->setFont(font);
}

void MainWindow::deleteSelectedText()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.removeSelectedText();
}

void MainWindow::onActionFindTriggered()
{
    find_dialog->show();
    find_dialog->activateWindow();
}

void MainWindow::onActionFindNextTriggered()
{
    if (find_dialog->researchedText().isEmpty())
        onActionFindTriggered();
    else
        findText();
}

void MainWindow::findText()
{
    find_dialog->findNext(ui->textEdit);
}

void MainWindow::onActionReplaceTriggered()
{
    QString textToReplace = find_dialog->researchedText();
    if (textToReplace != "")
    {
        replace_text_dialog->setTextToReplace(textToReplace);
    }
    replace_text_dialog->show();
    replace_text_dialog->activateWindow();
}

void MainWindow::onActionGoToTriggered()
{
    go_to_dialog->open();
    go_to_dialog->activateWindow();
}

void MainWindow::onActionDateTriggered()
{
    ui->textEdit->insertPlainText(QDateTime::currentDateTime().toString("hh:mm dd/MM/yyyy"));
}

void MainWindow::onClipboardDataChanged()
{
    QClipboard *clipboard = QApplication::clipboard();
    if (clipboard && clipboard->text().isEmpty())
        ui->actionPaste->setEnabled(false);
    else
        ui->actionPaste->setEnabled(true);
}

void MainWindow::onCursorPositionChanged()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    int line = cursor.blockNumber() + 1;
    int column = cursor.columnNumber() + 1;
    m_status_bar_label->setText("Ln " + QString::number(line) + ", Col " + QString::number(column));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_text_at_save != ui->textEdit->toPlainText())
    {
        if (saveAndCloseFileConfirmation())
            event->accept();
        else
        {
            event->ignore();
            return;
        }
    }
    else
    {
        event->accept();
    }
    saveSettings();
}

void MainWindow::findFilenameFromFilepath(const QString& filepath)
{
    QStringList list = filepath.split("/");
    m_filename = list[list.size() - 1];
}

bool MainWindow::saveAndCloseFileConfirmation()
{
    QMessageBox confirmationDialog(this);
    confirmationDialog.setWindowTitle(tr("Bloc-notes"));
    confirmationDialog.setText(tr("Voulez-vous enregistrer les modifications de ") + m_filename);

    QAbstractButton* saveButton = confirmationDialog.addButton(tr("Enregistrer"), QMessageBox::YesRole);
    QAbstractButton* noSaveButton = confirmationDialog.addButton(tr("Ne pas enregistrer"), QMessageBox::NoRole);
    QAbstractButton* cancelButton = confirmationDialog.addButton(tr("Annuler"), QMessageBox::RejectRole);

    confirmationDialog.exec();

    if (confirmationDialog.clickedButton() == saveButton)
    {
        save();
        return true;
    }
    else if (confirmationDialog.clickedButton() == noSaveButton)
    {
        return true;
    }
    else if (confirmationDialog.clickedButton() == cancelButton)
    {
        return false;
    }
    else
    {
        return false;
    }
}

void MainWindow::setFormattedWindowTitle()
{
    setWindowTitle(m_filename + " - Bloc-notes");
}

void MainWindow::initializeDocument()
{
    ui->textEdit->clear();
    m_filename = tr("Sans titre");
    setFormattedWindowTitle();
}

void MainWindow::setupStatusBar()
{
    m_status_bar_label = new QLabel("Ln 1, Col 1", ui->statusBar);
    ui->statusBar->addPermanentWidget(m_status_bar_label, 0);
    ui->statusBar->hide();
}

void MainWindow::loadSettings()
{
    setGeometry(m_settings.value("mainwindow/geometry", QRect(0, 0, 800, 600)).toRect());

    ui->textEdit->setFont( m_settings.value("mainwindow/text_edit/font").value<QFont>());

    ui->actionAutomaticBreakline->setChecked(m_settings.value("mainwindow/text_edit/breakline").toBool());
}

void MainWindow::saveSettings()
{
    m_settings.setValue("mainwindow/geometry", geometry());
    m_settings.setValue("mainwindow/text_edit/font", ui->textEdit->font());    
    m_settings.setValue("mainwindow/text_edit/breakline", ui->actionAutomaticBreakline->isChecked());
}
