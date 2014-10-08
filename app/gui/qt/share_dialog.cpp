#include <fstream>
#include <iostream>

#include "share_dialog.h"

void ShareDialog::initialise() {
    this->setWindowTitle("Share");
    export_button->setText("SHARE");
    heading_label->setText("Share");
    subheading_label->setText("Share your project with the world");
}

/**
 * Slots
 */

int ShareDialog::export_file() {
    std::vector<QPushButton *> buttons = {cancel_button, export_button};
    set_waiting(this, buttons);

    if (save() == -1) {
        this->setCursor(QCursor(Qt::ArrowCursor));
        return -1;
    }

    // Share
    FILE *share_proc;

    std::string filename = title_input->text().toUtf8().constData();
    std::replace(filename.begin(), filename.end(), ' ', '_');
	std::string filepath = save_dir
		+ filename
        + std::string(".spi");

    std::string cmd = std::string("/usr/bin/kano-share upload make-music ")
                    + filename.c_str()
                    + std::string(" ")
                    + filepath.c_str();
    if (!(share_proc = popen(cmd.c_str(), "r"))) {
        set_active(this, buttons);
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
        set_active(this, buttons);
        return -1;
    }

	this->accept();
	return 0;
}
