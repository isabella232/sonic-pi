#ifndef IO_DIALOG_H
#define IO_DIALOG_H

#include <QDialog>
#include <QGridLayout>

class IODialog : public QDialog {
    Q_OBJECT

    public:
        IODialog(QWidget * parent = 0,
                 Qt::WindowFlags f = 0) :
            QDialog(parent, f) {
                initialise();
            };
        ~IODialog();

    protected:
        QGridLayout * layout;
        std::string file_contents;

        void initialise();
        void connect_listeners();
};

#endif
