
#include "journal.h"
#include "dbjournal.h"
#include <utilfuncs/utilfuncs.h>
//#include "modpics.h"
#include "modeler.h"

const std::string NEW_ITEM{"*New Item*"};

struct ARPicDisplay : public Display
{
	virtual void Paint(Draw& w, const Rect& r, const Value& q, Color ink, Color paper, dword style) const
	{
		w.DrawRect(r, paper);
		Image pic=GetPic(q);
		w.DrawImage(r, pic);
	}
};

JItem::JItem() { hdts=h_dt_stamp(); flag=MID_NOTE; name=NEW_ITEM; note.clear(); }

Journal::Journal()
{
	pModeler=nullptr;
	
	split.Horz(arItem, wordproc);
	split.SetPos(3000);
	Add(split);
	
	wordproc.WhenSave << [&]{ check_save(); };

	pCurItem=nullptr;
	bHistory=false;

	arItem.AddColumn("");
	arItem.AddColumn("Date").Sorting();
	arItem.AddColumn("!").SetDisplay(Single<ARPicDisplay>());
	arItem.AddColumn("Title").Sorting();
	arItem.ColumnWidths("0 75 28 200");
	arItem.ColumnAt(1).HeaderTab().MinMax(75, 75);
	arItem.ColumnAt(2).HeaderTab().MinMax(28, 28);
	
	arItem.WhenBar << [&](Bar &bar){ armenu(bar); };
	arItem.WhenAction << [&]{ OnARClick(); };

}

Journal::~Journal() { check_save(); }

bool Journal::Initialize(Modeler *pMod)
{
	pModeler=pMod;
	FillAR();
	if (arItem.GetCount()) arItem.SetCursor(0);
	wordproc.SetFocus();
	return true;
}

void Journal::Terminate(){}

void Journal::GotFocus()
{
	wordproc.SetFocus();
}

void Journal::armenu(Bar &bar)
{
	check_save();
	bar.Add("New..", [&]{ NewItem(); });
	int y=arItem.GetCursor();
	if (y>=0)
	{
		bar.Separator();
		JItem &I=get_item(arItem.Get(y,0).ToString().ToStd());
		if (I.flag==MID_HISTORY) { bar.Add("Make current", [&]{ SetFlag(I, MID_NOTE); }); }
		else
		{
			bar.Add("Edit Title", [&]{ EditItem(I); });
			bar.Add("Move to History", [&]{ SetFlag(I, MID_HISTORY); });
		}
		bar.Separator();
		bar.Add("Delete", [&]{ DeleteItem(I); });
		bar.Separator();
		bar.Add(((bHistory)?"Hide History":"Show History"), [&]{ bHistory=!bHistory; FillAR(); });
	}
}

void Journal::FillAR()
{
	auto ymd=[](const std::string &hdt)->std::string{ std::string s=hdt.substr(0,6); s.insert(2,"-"); s.insert(5,"-"); return s; };
	items.clear();
	arItem.Clear();
	dbjournal.Load(items);
	for (auto i:items)
	{
		if (i.flag==MID_HISTORY) { if (bHistory) arItem.Add(i.hdts.c_str(), ymd(i.hdts).c_str(), i.flag, i.name.c_str()); }
		else arItem.Add(i.hdts.c_str(), ymd(i.hdts).c_str(), i.flag, i.name.c_str());
	}
	if (pCurItem) SelectARItem(*pCurItem);
}

void Journal::OnARClick()
{
	if (check_save()) return; //cursor invalidated because new item was added
	wordproc.editor.Clear(false);
	pCurItem=nullptr;
	int y=arItem.GetCursor(); //keep click-cursor
	if (y>=0)
	{
		JItem &I=get_item(arItem.Get(y,0).ToString().ToStd());
		set_note(I.note);
		pCurItem=&I;
	}
}

void Journal::SelectARItem(const JItem &I)
{
	int y=arItem.Find(I.hdts.c_str());
	if (y>=0)
	{
		arItem.SetCursor(y);
		pCurItem=&get_item(arItem.Get(y,0).ToString().ToStd());
	}
	else pCurItem=nullptr;
	wordproc.editor.Activate();//.SetFocus();
}

JItem& Journal::get_item(const std::string &hdts)
{
	auto it=items.begin();
	while (it!=items.end()) { if (seqs(it->hdts, hdts)) return (*it); it++; }
	throw "ERROR-BUG-ERROR-BUG LOLMFAO!";
}

bool Journal::edit_item(JItem &I)
{
	EditJI dlg;
	dlg.ebTitle.SetData(I.name.c_str());
	dlg.radFlag.SetData(I.flag);
	dlg.Execute();
	if (dlg.bok)
	{
		I.name=dlg.ebTitle.GetData().ToString().ToStd();
		I.flag=~dlg.radFlag;
		return true;
	}
	return false;
}

void Journal::get_note(std::string &s)
{
	s=wordproc.editor.GetQTF().ToStd();
}

void Journal::set_note(std::string &s)
{
	wordproc.editor.SetQTF(s.c_str());
	wordproc.editor.ClearModify();
	wordproc.editor.SetFocus();
}

bool Journal::check_save()
{
	if (wordproc.editor.IsModified())
	{
		if (pCurItem) { get_note(pCurItem->note); Save(*pCurItem); }
		else
		{
			JItem I;
			get_note(I.note);
			if (!I.note.empty())
			{
				pModeler->Tabs.Set(*this);
				if (PromptYesNo("Do you want to save the new Journal data?"))
				{
					if (edit_item(I)) { get_note(I.note); Save(I); FillAR(); SelectARItem(I); return true; }
				}
			}
		}
		wordproc.editor.ClearModify();
	}
	return false;
}

//void Journal::do_save()
//{
//	int y=arItem.GetCursor();
//	if (y>=0)
//	{
//		JItem &I=get_item(arItem.Get(y,0).ToString().ToStd());
//		Save(I);
//	}
//	else
//	{
//		JItem I;
//		edit_item(I); TRIM(I.name);
//		if (I.name.empty()) I.name=NEW_ITEM;
//		Save(I);
//		FillAR();
//	}
//}

bool Journal::Save(JItem &I)
{
	return dbjournal.Save(I);
}

void Journal::AddItem(int flag, const std::string &caption, const std::string &notes) //i/f for use from tabs
{
	check_save();
	JItem I;
	I.flag=((flag>=MID_NOTE)&&(flag<=MID_HISTORY))?flag:MID_NOTE;
	I.name=(caption.empty())?NEW_ITEM:caption;
	I.note=notes;
	if (dbjournal.Save(I)) FillAR();
}

void Journal::NewItem()
{
	check_save();
	wordproc.editor.Clear(false);
	JItem I;
	if (edit_item(I)) { Save(I); FillAR(); SelectARItem(I); }
}

void Journal::EditItem(JItem &I)
{
	check_save();
	if (edit_item(I)) { Save(I); FillAR(); }
}

void Journal::SetFlag(JItem &I, int flag)
{
	check_save();
	I.flag=flag;
	Save(I);
	FillAR();
}
 
void Journal::DeleteItem(JItem &I)
{
	if (pCurItem != &I)  check_save();
	else pCurItem=nullptr;
	if (dbjournal.Delete(I))
	{
		FillAR();
		if (pCurItem) SelectARItem(*pCurItem);
		else if (arItem.GetCount()>0) arItem.SetCursor(0);
	}
}


