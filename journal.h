#ifndef _modeler_journal_h_
#define _modeler_journal_h_

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

#include <string>
#include <vector>
#include <WordProcCtrl/wordprocctrl.h>
#include "modpics.h"

struct Modeler;

struct EditJI : public TopWindow
{
	typedef EditJI CLASSNAME;
	
	Label lblName;
	EditString ebTitle;
	Switch radFlag;
	Button btnOK;
	Button btnCancel;
	bool bok;
	
	EditJI()
	{
		Title("Journal Item");
		SetRect(0,0,400,150);
		//Sizeable();
	
		Add(lblName.SetLabel(t_("Title:")).LeftPosZ(4, 32).TopPosZ(8, 19));
		Add(ebTitle.HSizePosZ(40, 8).TopPosZ(8, 19));
		Add(radFlag.SetLabel(t_("Note\nInfo\nTo do\nAttention\nIssue\nHistory")).LeftPosZ(40, 92).TopPosZ(36, 92));
		Add(btnOK.SetLabel(t_("OK")).RightPosZ(40, 60).TopPosZ(52, 20));
		Add(btnCancel.SetLabel(t_("Cancel")).RightPosZ(40, 60).TopPosZ(88, 20));

		bok=false;

		btnOK.WhenAction << [&]{ bok=true; Close(); };
		btnCancel.WhenAction << [&]{ bok=false; Close(); };

		radFlag.SetData(0);
	}
	virtual ~EditJI(){}

};

struct JItem
{
	std::string hdts; //human-readable dtstamp set on create
	int flag;
	std::string name;
	std::string note;
	
	JItem();
	JItem(const JItem &I) { *this=I; }
	virtual ~JItem() {}
	JItem& operator=(const JItem &I) { hdts=I.hdts; flag=I.flag; name=I.name; note=I.note; return *this; }
};

typedef std::vector<JItem> ItemList;

struct Journal : public Ctrl
{
	using CLASSNAME=Journal;

	ArrayCtrl arItem;
	WordProcCtrl wordproc;
	JItem *pCurItem;
	Splitter split;
	
	Modeler *pModeler;
	
	bool bHistory; //show in ar..
	ItemList items;
	
	Journal();
	~Journal();

	bool Initialize(Modeler *pMod);
	void Terminate();

	virtual void GotFocus();
	virtual void LostFocus() { check_save(); }

	void armenu(Bar &bar);

	void FillAR();
	void OnARClick();
	void SelectARItem(const JItem &I);
	
	JItem& get_item(const std::string &hdts);
	bool edit_item(JItem &I);
	void get_note(std::string &s);
	void set_note(std::string &s);

	bool check_save(); //returns true if new item added
//	void do_save();
	bool Save(JItem &I);
	
	void AddItem(int flag, const std::string &caption, const std::string &notes); //i/f for use from tabs
	void NewItem();
	void EditItem(JItem &I);
	void SetFlag(JItem &I, int flag);
	void DeleteItem(JItem &I);

	
};



#endif
