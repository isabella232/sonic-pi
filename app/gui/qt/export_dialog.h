#ifndef EXPORT_DIALOG_H
#define EXPORT_DIALOG_H

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <string>

#include "io_dialog.h"

#define SAMPLE_TMP_PATH "/tmp/sonic_pi.wav"

class ExportDialog : public IODialog
{
    Q_OBJECT

    public:
        ExportDialog(QWidget * parent = 0,
                     Qt::WindowFlags f = 0) :
            IODialog(parent, f) {
                initialise();
                connect_listeners();
            };
        ~ExportDialog();
        void set_file_contents(std::string contents);

    protected slots:
	    virtual int export_file();

    protected:
        QLabel * heading_label;
        QLabel * subheading_label;
		QLineEdit * title_input;
		QTextEdit * desc_input;
        QPushButton * cancel_button;
        QPushButton * export_button;

        void initialise();
        void connect_listeners();
        int save();
        int save_to_file(std::string filepath, std::string file_contents);
};

#endif
