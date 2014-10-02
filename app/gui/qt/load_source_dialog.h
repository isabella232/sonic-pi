#ifndef LOAD_SOURCE_DIALOG_H
#define LOAD_SOURCE_DIALOG_H

#include <QLabel>
#include <QPushButton>
#include <string>

#include "io_dialog.h"

class LoadSourceDialog : public IODialog
{
    Q_OBJECT

    public:
        LoadSourceDialog(QWidget * parent = 0,
                         Qt::WindowFlags f = 0) :
            IODialog(parent, f) {
                initialise();
                connect_listeners();
            };
        ~LoadSourceDialog();
        std::string get_file_contents();

    protected slots:
        int load_from_local();
        int load_from_internet();

    protected:
        QLabel * heading_label;
        QLabel * subheading_label;
        QPushButton * local_button;
        QPushButton * remote_button;

        void initialise();
        void connect_listeners();
        int read_file(std::string filename);
};

#endif
