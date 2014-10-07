#include "save_dialog.h"

void SaveDialog::initialise() {
    this->setWindowTitle("Save");
    export_button->setText("SAVE");
    heading_label->setText("Save");
    subheading_label->setText("Keep your project for later");
}

/**
 * Slots
 */

int SaveDialog::export_file() {
	this->accept();
	return save();
}
