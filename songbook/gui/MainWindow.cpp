#include "MainWindow.hpp"
#include "SongbookException.hpp"
#include "SongbookPrinterLatex.hpp"

#include <fstream>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

using namespace std::string_literals;

MainWindow::MainWindow(QWidget *parent): QWidget{parent} {

    converter = songbook::init_converter<songbook::SongbookPrinterLatex>();
    latex_run_i = 0;
    latex_process = new QProcess(this);
    connect(latex_process, &QProcess::finished,
            this, &MainWindow::latex_finished);

    xml_label = new QLabel(tr("XML file:"));
    pdf_label = new QLabel(tr("PDF file:"));
    status_label = new QLabel();

    xml_file_button = new QPushButton(tr("Click to select XML file..."));
    xml_file_button->setFlat(true);
    xml_file_button->setCursor(QCursor(Qt::PointingHandCursor));
    xml_file_button->setStyleSheet("font-weight: bold");
    xml_file_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    connect(xml_file_button, &QPushButton::clicked,
            this, &MainWindow::select_xml_file);

    parse_button = new QPushButton(tr("Parse XML"));
    parse_button->setEnabled(false);
    connect(parse_button, &QPushButton::clicked,
            this, &MainWindow::parse_xml);

    pdf_file_button = new QPushButton();
    pdf_file_button->setFlat(true);
    pdf_file_button->setCursor(QCursor(Qt::PointingHandCursor));
    pdf_file_button->setStyleSheet("font-weight: bold; text-align: left");
    pdf_file_button->setEnabled(false);
    connect(pdf_file_button, &QPushButton::clicked,
            this, &MainWindow::select_pdf_file);

    create_pdf_button = new QPushButton(tr("Create PDF"));
    create_pdf_button->setToolTip("Parse XML and run XeLaTeX to create the PDF.");
    create_pdf_button->setEnabled(false);
    connect(create_pdf_button, &QPushButton::clicked,
            this, &MainWindow::create_pdf);

    twice_checkbox = new QCheckBox(tr("Twice"));
    twice_checkbox->setToolTip("Should XeLaTeX be run twice to correctly produce the table of contents?");

    output_text = new QTextEdit();
    output_text->setReadOnly(true);
    output_text->setStyleSheet("font-family: Consolas, \"Courier New\", monospace");

    clear_button = new QPushButton(tr("Clear output"));
    clear_button->setToolTip("Clears the above text area");
    connect(clear_button, &QPushButton::clicked,
            output_text, &QTextEdit::clear);


    auto grid_layout = new QGridLayout();
    grid_layout->addWidget(xml_label, 0, 0);
    grid_layout->addWidget(xml_file_button, 0, 1);
    grid_layout->addWidget(parse_button, 0, 2);

    grid_layout->addWidget(pdf_label, 1, 0);
    grid_layout->addWidget(pdf_file_button, 1, 1);
    grid_layout->addWidget(create_pdf_button, 1, 2);
    grid_layout->addWidget(twice_checkbox, 1, 3);

    grid_layout->addWidget(status_label, 2, 0, 1, 4);

    auto main_layout = new QVBoxLayout(this);
    main_layout->addLayout(grid_layout);
    main_layout->addWidget(output_text);
    main_layout->addWidget(clear_button);

    setWindowTitle(tr("Songbook Convertor"));
}


void MainWindow::select_xml_file() {
    QString xml_file = QFileDialog::getOpenFileName(this,
                                                tr("Select XML file"),
                                                "",
                                                tr("XML files (*.xml)"));
    if (!xml_file.isEmpty()) {
        xml_file_button->setStyleSheet(xml_file_button->styleSheet().append(
            "; text-align: left"));
        xml_file_button->setText(xml_file);
        parse_button->setEnabled(true);

        // set PDF file name
        QString pdf_basename_new = xml_file.left(xml_file.size() - 4);
        if (pdf_basename_new != pdf_file_basename) {
            auto reply = QMessageBox::Yes;
            // confirm PDF name change when there already was a name
            if (!pdf_file_basename.isEmpty())
                reply = QMessageBox::question(this,
                              "PDF file name change",
                              "Change PDF file name to <b>" + pdf_basename_new + ".pdf</b>?",
                              QMessageBox::Yes|QMessageBox::No,
                              QMessageBox::Yes);

            if (reply == QMessageBox::Yes) {
                set_pdf_file(pdf_basename_new + ".pdf");
            }
        }
        pdf_file_button->setEnabled(true);
        create_pdf_button->setEnabled(true);
    }
}

void MainWindow::set_pdf_file(const QString& file) {

    pdf_file_basename = file.left(file.size() - 4);
    pdf_file_button->setText(file);
}

void MainWindow::select_pdf_file() {
    QString pdf_file = QFileDialog::getSaveFileName(this,
                                                    tr("Select PDF file"),
                                                    pdf_file_button->text(),
                                                    tr("PDF files (*.pdf)"));
    if (!pdf_file.isEmpty())
        set_pdf_file(pdf_file);
}

bool MainWindow::parse_xml() {

    status_label->clear();
    auto xml_file = xml_file_button->text();

    if (!xml_file.isEmpty()) {
        try {
            converter.parse_songbook(xml_file.toStdString());
            display_status("XML file parsed succesfully");
            return true;

        } catch (songbook::SongbookException se) {
            display_status("Error(s) during XML parsing:", false);
            output_text->setText(se.what());

        } catch (std::runtime_error re) {
            display_status("Error: the file \"" + xml_file +
                               "\" could not be opened",
                           false);
        }
    }
    return false;
}

void MainWindow::create_pdf() {

    // always parse XML first and abort when parsing was not successful
    if (!parse_xml())
        return;

    // try to create LaTeX file
    std::ofstream ofs;
    latex_file = pdf_file_basename + ".tex";
    ofs.open(latex_file.toStdString());

    // abort when LaTeX file can't be opened
    if (!ofs.is_open()) {
        display_status("Error: LaTeX file <b>" + latex_file + "</b>" +
               "could not be saved!");
        return;
    }

    // create LaTeX file
    ofs << converter.convert();
    ofs.close();
    display_status("LaTeX file saved to <b>" + latex_file + "</b>");

    // LaTeX will run once when this is 0, twice when 1
    latex_run_i = twice_checkbox->isChecked();
    create_pdf_button->setEnabled(false);
    run_latex();
}

QString MainWindow::add_latex_run_suffix(QString message) {

    if (latex_run_i > 0) {
        QTextStream qts(&message);
        qts << " [" << latex_run_i << "/2]";
    }
    return message;
}

void MainWindow::run_latex() {

    display_status(add_latex_run_suffix("Running XeLaTeX") + "...");
    latex_process->start("xelatex", {"-interaction=nonstopmode", latex_file});
}

void MainWindow::latex_finished(int exitCode, QProcess::ExitStatus exitStatus) {

    if (exitCode == 0) {

        display_status(add_latex_run_suffix("PDF file created"));
        output_text->append(latex_process->readAllStandardOutput());

        // second LaTeX run
        if (latex_run_i > 0 && latex_run_i < 2) {
            ++latex_run_i;
            run_latex();
        } else
            create_pdf_button->setEnabled(true);

    } else {

        display_status("Error(s) while running XeLaTeX!", false);
        output_text->append(latex_process->readAllStandardOutput());
        create_pdf_button->setEnabled(true);
    }
}

void MainWindow::display_status(const QString& message, bool success) {

    if (!status_label->text().isEmpty()) {
        output_text->append(status_label->text());
        output_text->append("<b>=============</b>");
    }

    std::string color{"color: "};
    status_label->setStyleSheet(("color: "s +
                                 (success ? "green" : "red")).c_str());
    status_label->setText(message);
    status_label->repaint();
}

QSize MainWindow::minimumSizeHint() const {
    return QSize(320, 320);
}

QSize MainWindow::sizeHint() const {
    return QSize(500, 500);
}


