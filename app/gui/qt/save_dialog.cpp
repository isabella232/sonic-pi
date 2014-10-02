#include <fstream>
#include <QDir>

#include "save_dialog.h"

void SaveDialog::initialise() {
	export_button->setText("SAVE");
}

/**
 * Slots
 */

int SaveDialog::export_file() {
	std::string filename = QDir::homePath().toUtf8().constData()
		+ std::string("/Music-content/")
		+ title_input->text().toUtf8().constData()
        + std::string(".spi");

    if (title_input->text().isEmpty()) {
        return -1;
    }

	std::ofstream file(filename.c_str());
    file << file_contents;
	file.close();

	this->accept();
	return 0;
}
