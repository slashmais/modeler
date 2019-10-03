
#include "modeler.h"
#include "dbmodeler.h"
#include <utilfuncs/utilfuncs.h>


bool Initialize()
{
	bool bret=false;
	//if (!HoMicile()) { telluser(spf("HoM: ", HoM_Message())); return false; }
	//ModConf.QDPath=path_append(HoM_PATH, "modeler");
	
	if (GetAppHoMConfig(GetExeFilePath(), ModConf))
	{
		if (ModConf.Load())
		{
			if (!ModConf.haskey("modeler")) ModConf.setval("modeler", path_append(HoM_PATH, "modeler"));
			std::string sdbmod=path_append(ModConf.getval("modeler"), "dbmodeler.sqlite3"); //<<<<----check&test
			bool bX=file_exist(sdbmod);
			if ((bret=dbmodeler.Open(sdbmod))) { if (!bX) { bret=dbmodeler.ImplementSchema(); }}
			if (!bret) tellerror("Database: '", sdbmod, "' - failed");
		}
	}
	else if (!bret) tellerror("Config failed: ", HoM_Message());

	return bret;
}

void Terminate()
{
	ModConf.Save();
}

GUI_APP_MAIN
{
	if (Initialize())
	{
		Modeler().Run();
		Terminate();
	}
	else telluser("Cannot initialize modeler");
}
