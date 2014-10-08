#ifndef IO_DIALOG_H
#define IO_DIALOG_H

#include <QWidget>
#include <QDialog>
#include <QGridLayout>
#include <QPushButton>

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
        std::string save_dir;

        void initialise();
        void connect_listeners();
        int ensure_dir(std::string dir);
        void set_waiting(QWidget * window, std::vector<QPushButton *> buttons = {});
        void set_active(QWidget * window, std::vector<QPushButton *> buttons = {});
};

#endif
