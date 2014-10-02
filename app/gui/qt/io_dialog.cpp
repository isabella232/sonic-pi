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


    layout = new QGridLayout();

    this->setLayout(layout);
}
