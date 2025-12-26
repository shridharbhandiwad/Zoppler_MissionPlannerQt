#include "csavescenariodialog.h"
#include "ui_csavescenariodialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QFile>

CSaveScenarioDialog::CSaveScenarioDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CSaveScenarioDialog)
{
    ui->setupUi(this);
    setWindowTitle("Save Scenario");
    
    // Set default directory
    _m_selectedDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/scenarios";
    ui->lineEdit_SaveLocation->setText(_m_selectedDirectory);
    
    // Disable save button initially
    ui->pushButton_Save->setEnabled(false);
}

CSaveScenarioDialog::~CSaveScenarioDialog()
{
    delete ui;
}

QString CSaveScenarioDialog::getScenarioName() const
{
    return ui->lineEdit_Name->text().trimmed();
}

QString CSaveScenarioDialog::getScenarioDescription() const
{
    return ui->textEdit_Description->toPlainText().trimmed();
}

QString CSaveScenarioDialog::getFilePath() const
{
    QString name = getScenarioName();
    if (name.isEmpty()) {
        return QString();
    }
    
    // Replace spaces with underscores and add .json extension
    name.replace(" ", "_");
    if (!name.endsWith(".json")) {
        name += ".json";
    }
    
    return _m_selectedDirectory + "/" + name;
}

bool CSaveScenarioDialog::validateInput()
{
    QString name = getScenarioName();
    
    if (name.isEmpty()) {
        ui->label_Status->setText("Please enter a scenario name");
        ui->label_Status->setStyleSheet("color: red;");
        return false;
    }
    
    // Check for invalid characters in filename
    QRegularExpression invalidChars("[<>:\"/\\\\|?*]");
    if (invalidChars.match(name).hasMatch()) {
        ui->label_Status->setText("Scenario name contains invalid characters");
        ui->label_Status->setStyleSheet("color: red;");
        return false;
    }
    
    ui->label_Status->setText("Ready to save");
    ui->label_Status->setStyleSheet("color: green;");
    return true;
}

void CSaveScenarioDialog::on_lineEdit_Name_textChanged(const QString &text)
{
    Q_UNUSED(text);
    bool valid = validateInput();
    ui->pushButton_Save->setEnabled(valid);
}

void CSaveScenarioDialog::on_pushButton_Save_clicked()
{
    if (!validateInput()) {
        QMessageBox::warning(this, "Invalid Input", "Please provide a valid scenario name.");
        return;
    }
    
    QString filePath = getFilePath();
    
    // Check if file already exists
    if (QFile::exists(filePath)) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "File Exists",
            "A scenario with this name already exists. Do you want to overwrite it?",
            QMessageBox::Yes | QMessageBox::No
        );
        
        if (reply != QMessageBox::Yes) {
            return;
        }
    }
    
    accept();
}

void CSaveScenarioDialog::on_pushButton_Cancel_clicked()
{
    reject();
}

void CSaveScenarioDialog::on_pushButton_BrowseLocation_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(
        this,
        "Select Save Location",
        _m_selectedDirectory,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    if (!directory.isEmpty()) {
        _m_selectedDirectory = directory;
        ui->lineEdit_SaveLocation->setText(directory);
    }
}
