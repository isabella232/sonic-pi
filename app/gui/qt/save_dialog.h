#ifndef SAVE_DIALOG_H
#define SAVE_DIALOG_H

#include "export_dialog.h"

class SaveDialog : public ExportDialog
{
	Q_OBJECT

	public:
        SaveDialog(QWidget * parent = 0,
                   Qt::WindowFlags f = 0) :
            ExportDialog(parent, f) {
				initialise();
            };

    protected slots:
	    int export_file();

	protected:
		void initialise();
};

#endif
