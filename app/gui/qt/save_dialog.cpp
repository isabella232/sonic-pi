#include "save_dialog.h"

void SaveDialog::initialise() {
	export_button->setText("SAVE");
}

/**
 * Slots
 */

int SaveDialog::export_file() {
	this->accept();
	return save();
}
