#include <fstream>
#include <iostream>

#include "export_dialog.h"

ExportDialog::~ExportDialog()
{
    delete heading_label;
    delete subheading_label;
    delete cancel_button;
    delete export_button;
	delete title_input;
	delete desc_input;
}

void ExportDialog::initialise() {
    heading_label = new QLabel("Save where?");
    subheading_label = new QLabel("Do you want to load from your Kano or the Internet?");
    cancel_button = new QPushButton("&CANCEL", this);
    export_button = new QPushButton(this);

	title_input = new QLineEdit();
	title_input->setPlaceholderText("Title");
	desc_input = new QTextEdit();
	// desc_input->setPlaceholderText("Description");

    layout->addWidget(heading_label, 0, 0, 1, 2);
    layout->addWidget(subheading_label, 1, 0, 1, 2);
	layout->addWidget(title_input, 2, 0, 1, 2);
	layout->addWidget(desc_input, 3, 0, 1, 2);
    layout->addWidget(cancel_button, 4, 0);
    layout->addWidget(export_button, 4, 1);
}

void ExportDialog::connect_listeners() {
    connect(cancel_button, SIGNAL(clicked()), this, SLOT(reject()));
    connect(export_button, SIGNAL(clicked()), this, SLOT(export_file()));
}

/**
 * Slots
 */

int ExportDialog::export_file() {
	return 0;
};

/**
 * Public methods
 */

void ExportDialog::set_file_contents(std::string contents) {
    file_contents = contents;
}
