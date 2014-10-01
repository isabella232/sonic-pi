#ifndef LOAD_SOURCE_DIALOG_H
#define LOAD_SOURCE_DIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <string>

class LoadSourceDialog : public QDialog
{
    Q_OBJECT

    public:
        LoadSourceDialog(QWidget * parent = 0,
                         Qt::WindowFlags f = 0) :
            QDialog(parent, f) {
                initialise();
                connect_listeners();
            };
        ~LoadSourceDialog();
        std::string get_file_contents();

    private slots:
        int load_from_local();
        int load_from_internet();

    private:
        QGridLayout * layout;
        QLabel * heading_label;
        QLabel * subheading_label;
        QPushButton * local_button;
        QPushButton * remote_button;
        std::string file_contents;

        void initialise();
        void connect_listeners();
        int read_file(std::string filename);
};

#endif
