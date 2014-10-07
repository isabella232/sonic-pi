#include <QDir>
#include <fstream>

#include "io_dialog.h"

IODialog::~IODialog()
{
    delete layout;
}

void IODialog::initialise() {
    file_contents = "";

    save_dir = QDir::homePath().toUtf8().constData()
		+ std::string("/Music-content/");
    ensure_dir(save_dir);

    this->setWindowFlags(Qt::Tool
                         | Qt::WindowTitleHint
                         | Qt::WindowCloseButtonHint
                         | Qt::CustomizeWindowHint);
    this->setWindowModality(Qt::ApplicationModal);

    QRect app_geometry = static_cast<QWidget *>(parent())->geometry();
    this->move(app_geometry.center() - rect().center());

    layout = new QGridLayout();

    this->setLayout(layout);

}

int IODialog::ensure_dir(std::string dir) {
    std::string cmd = std::string("mkdir -p ")
        + dir;

    FILE * mkdir_proc;

    if (!(mkdir_proc = popen(cmd.c_str(), "r"))) {
        return -1;
    }

    pclose(mkdir_proc);

    return 0;
}
