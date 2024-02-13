#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <SongbookConverter.hpp>

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QProcess>

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

signals:

private:

    /**
     * Sets `pdf_file_basename` and `pdf_file_button` text according
     * to `file`
     *
     * @param file PDF file (including the ".pdf" extension)
     */
    void set_pdf_file(const QString& file);

    /**
     * Opens a file selection dialog, displays selected file and fills
     * the PDF file field.
     */
    void select_xml_file();

    /**
     * Opens a file selection dialog, and displays the selected PDF file.
     */
    void select_pdf_file();

    /**
     * Parses selected XML file.
     * @return Was parsing successful?
     */
    bool parse_xml();

    /**
     * Adds LaTeX run number to a message when appropriate.
     *
     * @param message message start
     * @return `message`, possibly with LaTeX run number
     */
    QString add_latex_run_suffix(QString message);

    /**
     * Parses XML, creates LaTeX file and produces the final PDF.
     */
    void create_pdf();

    /**
     * Runs XeLaTeX.
     *
     * @param latex_file path to a LaTeX file
     */
    void run_latex();

    /**
     * Performs actions following finishing of a LaTeX process.
     *
     * @param exitCode LaTeX process exit code
     * @param exitStatus exit status
     */
    void latex_finished(int exitCode, QProcess::ExitStatus exitStatus);

    /**
     * Displays given message in the status label; in green when OK, red otherwise.
     *
     * @param message the message
     * @param success Was the process sucessful?
     */
    void display_status(const QString& message, bool success = true);

    QLabel *xml_label;
    QLabel *pdf_label;
    QLabel *status_label;

    QTextEdit *output_text;

    QPushButton *xml_file_button;
    QPushButton *parse_button;
    QPushButton *pdf_file_button;
    QPushButton *create_pdf_button;
    QPushButton *clear_button;

    QCheckBox *twice_checkbox;

    QString pdf_file_basename;  /**< base name for LaTeX and XML files (without the extension) */
    QString latex_file;			/**< LaTeX file name */
    int latex_run_i;  			/**< LaTeX run index. 0 when running only once, 1 or 2 when running twice */
    songbook::SongbookConverter converter;
    QProcess *latex_process;
};

#endif // MAINWINDOW_HPP
