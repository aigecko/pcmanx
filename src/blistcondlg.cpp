//
// C++ Implementation: blistcondlg
//
// Description:
//
// Modified from emoticondlg.cpp
// Origin Author: Hong Jen Yee (PCMan) <pcman.tw@gmail.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifdef __GNUG__
  #pragma implementation "blistcondlg.h"
#endif

#include <cstring>
#include "blistcondlg.h"
#include "inputdialog.h"
#include "appconfig.h"

static const char blacklist_file_name[] = "blacklist";

CBlistconDlg::CBlistconDlg(CWidget* parent) : m_IsModified(false)
// : CDialog( parent, _("Blacklist"), true )
{
	m_Widget = gtk_dialog_new_with_buttons(_("Blacklist"),
		GTK_WINDOW(parent->m_Widget),
		GtkDialogFlags(GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT), GTK_STOCK_CLOSE, GTK_RESPONSE_CANCEL, NULL);

	gtk_window_set_type_hint (GTK_WINDOW (m_Widget), GDK_WINDOW_TYPE_HINT_DIALOG);

	PostCreate();

	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *blacklist_scrl;
	GtkWidget *blacklist;
	GtkWidget *vbtn_box;
	GtkWidget *add_btn;
	GtkWidget *edit_btn;
	GtkWidget *remove_btn;
	GtkWidget *up_btn;
	GtkWidget *down_btn;

	vbox = GTK_DIALOG (m_Widget)->vbox;
	gtk_widget_show (vbox);

	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_end (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);

	blacklist_scrl = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (blacklist_scrl);
	gtk_box_pack_start (GTK_BOX (hbox), blacklist_scrl, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (blacklist_scrl), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (blacklist_scrl), GTK_SHADOW_IN);

	vbtn_box = gtk_vbutton_box_new ();
	gtk_widget_show (vbtn_box);
	gtk_box_pack_start (GTK_BOX (hbox), vbtn_box, FALSE, TRUE, 0);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (vbtn_box), GTK_BUTTONBOX_START);

	add_btn = gtk_button_new_from_stock ("gtk-add");
	gtk_widget_show (add_btn);
	gtk_container_add (GTK_CONTAINER (vbtn_box), add_btn);
	GTK_WIDGET_SET_FLAGS (add_btn, GTK_CAN_DEFAULT);

	edit_btn = gtk_button_new_from_stock ("gtk-edit");
	gtk_widget_show (edit_btn);
	gtk_container_add (GTK_CONTAINER (vbtn_box), edit_btn);
	GTK_WIDGET_SET_FLAGS (edit_btn, GTK_CAN_DEFAULT);

	remove_btn = gtk_button_new_from_stock ("gtk-remove");
	gtk_widget_show (remove_btn);
	gtk_container_add (GTK_CONTAINER (vbtn_box), remove_btn);
	GTK_WIDGET_SET_FLAGS (remove_btn, GTK_CAN_DEFAULT);

	g_signal_connect ((gpointer) add_btn, "clicked",
					G_CALLBACK (CBlistconDlg::OnAdd),
					this);
	g_signal_connect ((gpointer) edit_btn, "clicked",
					G_CALLBACK (CBlistconDlg::OnEdit),
					this);
	g_signal_connect ((gpointer) remove_btn, "clicked",
					G_CALLBACK (CBlistconDlg::OnRemove),
					this);

	gtk_window_set_default_size((GtkWindow*)m_Widget, 512, 400);

	m_List = new CListBox;
	blacklist = m_List->m_Widget;
	gtk_widget_show (blacklist);
	gtk_container_add (GTK_CONTAINER (blacklist_scrl), blacklist);

	g_signal_connect(G_OBJECT(m_Widget), "response", G_CALLBACK(CDialog::OnResponse), this);

	g_signal_connect(G_OBJECT(m_List->m_Widget), "row-activated", G_CALLBACK(CBlistconDlg::OnListRowActivated), this );

	LoadBlacklists();
}



void CBlistconDlg::OnAdd(GtkWidget* btn UNUSED, CBlistconDlg* _this)
{
	int i = _this->m_List->GetCurSel() + 1;

	CInputDialog* dlg = new CInputDialog(_this, _("New Blacklist"), _("Input New Blacklist"));
	if( dlg->ShowModal() == GTK_RESPONSE_OK )
	{
		_this->m_List->Insert( i, dlg->GetText().c_str() );
		_this->m_List->SetCurSel( i );
		_this->m_IsModified = true;
	}
	dlg->Destroy();
}


void CBlistconDlg::OnEdit(GtkWidget* btn UNUSED, CBlistconDlg* _this)
{
	int sel = _this->m_List->GetCurSel();
	if( sel != -1 )
	{
		string text = _this->m_List->GetItemText(sel);
		CInputDialog* dlg = new CInputDialog(_this, _("Edit Blacklist"), _("Input New Blacklist"), text.c_str());
		if( dlg->ShowModal() == GTK_RESPONSE_OK )
		{
			_this->m_List->SetItemText( sel, dlg->GetText() );
			_this->m_IsModified = true;
		}
		dlg->Destroy();
	}
}


void CBlistconDlg::OnRemove(GtkWidget* btn UNUSED, CBlistconDlg* _this)
{
	int sel = _this->m_List->GetCurSel();
	if( sel >= 0 )
	{
		_this->m_List->Delete(sel);
		if( sel >= (_this->m_List->Count()-1) )
			sel--;
		_this->m_List->SetCurSel(sel);
		_this->m_IsModified = true;
	}
}


bool CBlistconDlg::OnOK()
{
	int sel = m_List->GetCurSel();
	if( sel >= 0 )
		m_SelStr = m_List->GetItemText( sel );

	if( m_IsModified )
		SaveBlacklists();
	return true;
}


void CBlistconDlg::LoadBlacklists()
{
	string fpath = AppConfig.GetConfigPath( blacklist_file_name );
	if( !g_file_test( fpath.c_str(), G_FILE_TEST_EXISTS ) )
		fpath = AppConfig.GetDataPath( blacklist_file_name );
	FILE* fi = fopen( fpath.c_str() ,"r" );
	if( fi )
	{
		char line[1024];
		while( fgets( line, sizeof(line), fi ) )
		{
			char* blacklist = strtok(line, "\r\n");
			m_List->Append( blacklist );
		}
		fclose(fi);
	}
}


void CBlistconDlg::SaveBlacklists()
{
	FILE* fo = fopen( AppConfig.GetConfigPath( blacklist_file_name ).c_str() , "w" );
	if( fo )
	{
		GtkTreeModel* model = m_List->GetTreeModel();
		gtk_tree_model_foreach( model, (GtkTreeModelForeachFunc)CBlistconDlg::SaveBlacklist, fo );
		fclose(fo);
	}
}


bool CBlistconDlg::OnCancel()
{
	if( m_IsModified )
		SaveBlacklists();

	return CDialog::OnCancel();
}


gboolean CBlistconDlg::SaveBlacklist(GtkTreeModel* model,
                                    GtkTreePath* path UNUSED,
				    GtkTreeIter* iter, FILE* file)
{
	gchar* text = NULL;
	gtk_tree_model_get( model, iter, 0, &text, -1 );
	if( text )
	{
		fprintf(file, "%s\n", text);
		g_free(text);
	}
	return false;
}


void CBlistconDlg::OnListRowActivated(GtkTreeView* view UNUSED,
                                      GtkTreePath* path UNUSED,
				      GtkTreeViewColumn* cols UNUSED,
				      CBlistconDlg* _this)
{
	gtk_dialog_response(GTK_DIALOG(_this->m_Widget), GTK_RESPONSE_OK);
}
