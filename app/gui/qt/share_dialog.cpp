#include <fstream>
#include <iostream>

#include "share_dialog.h"

void ShareDialog::initialise() {
	export_button->setText("SHARE");
}

/**
 * Slots
 */

int ShareDialog::export_file() {
    if (save() == -1) {
        return -1;
    }

    // Share
    FILE *share_proc;

	std::string filename = save_dir +
		+ title_input->text().toUtf8().constData()
        + std::string(".spi");

    std::string cmd = std::string("/usr/bin/kano-share upload make-music ")
                    + title_input->text().toUtf8().constData()
                    + std::string(" ")
                    + filename.c_str();
    if (!(share_proc = popen(cmd.c_str(), "r"))) {
        return -1;
    }

    // Read STDOUT
    char buff[512];
    std::string std_out;

    while (std::fgets(buff, sizeof(buff), share_proc) != NULL) {
        std_out += buff;
    }

    int rc = pclose(share_proc);
    int status = WEXITSTATUS(rc);

    if (status != 0) {
        // Sharing failed.
        std::cout << "\nShare failed: " << std_out;
        return -1;
    }

	this->accept();
	return 0;
}
