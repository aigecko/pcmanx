//
// C++ Interface: blistcondlg
//
// Description:
//
// Modified from emoticondlg.h
// Origin Author: Hong Jen Yee (PCMan) <pcman.tw@gmail.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BLISTCONDLG_H
#define BLISTCONDLG_H

#ifdef __GNUG__
  #pragma interface "blistcondlg.h"
#endif

#include "dialog.h"
#include "listbox.h"
#include <string>
#include <cstdio>

using namespace std;

/**
@author Hong Jen Yee (PCMan)
*/
class CBlistconDlg : public CDialog
{
public:
    CBlistconDlg(CWidget* parent);
    string GetSelectedStr(){	return m_SelStr;	}
    static void OnAdd(GtkWidget* btn, CBlistconDlg* _this);
    static void OnEdit(GtkWidget* btn, CBlistconDlg* _this);
    static void OnRemove(GtkWidget* btn, CBlistconDlg* _this);
    bool OnOK();
    void LoadBlacklists();
    void SaveBlacklists();
    bool OnCancel();
    static void OnListRowActivated(GtkTreeView* view, GtkTreePath* path, GtkTreeViewColumn* cols, CBlistconDlg* _this);
protected:
    string m_SelStr;
    bool m_IsModified;
    CListBox* m_List;
protected:
    static gboolean SaveBlacklist(GtkTreeModel* model, GtkTreePath* path, GtkTreeIter* iter, FILE* file);
};

#endif
