#ifndef _solver_dlgmeta_h_
#define _solver_dlgmeta_h_

#include <CtrlLib/CtrlLib.h>
using namespace Upp;


struct DlgMeta : public TopWindow
{
	typedef DlgMeta CLASSNAME;
	
	Label lblName;
	EditString ebName;
	Label lblNote;
	DocEdit ebNotes;
	Button btnOK;
	Button btnCancel;
	bool bok;
	
	DlgMeta()
	{
		Title("Meta-info");
		Sizeable();
		SetRect(0,0,600,400);
		Add(lblName.SetLabel(t_("Name:")).LeftPosZ(4, 40).TopPosZ(8, 19));
		Add(ebName.HSizePosZ(44, 4).TopPosZ(8, 19));
		Add(lblNote.SetLabel(t_("Notes")).LeftPosZ(4, 44).TopPosZ(32, 19));
		Add(ebNotes.HSizePosZ(4, 4).VSizePosZ(52, 44));
		Add(btnOK.SetLabel(t_("OK")).RightPosZ(84, 60).BottomPosZ(12, 20));
		Add(btnCancel.SetLabel(t_("Cancel")).RightPosZ(12, 60).BottomPosZ(12, 20));
		bok=false;
		btnOK.WhenAction << [&]{ bok=true; Close(); };
		btnCancel.WhenAction << [&]{ bok=false; Close(); };
	}
	virtual ~DlgMeta(){}
	
};

#endif
