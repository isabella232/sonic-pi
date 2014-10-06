#include "io_dialog.h"

IODialog::~IODialog()
{
    delete layout;
}

void IODialog::initialise() {
    file_contents = "";

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
