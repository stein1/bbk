#include "ThroughputModePanel.h"

#include "TestMarshall.h"
#include "MainFrame.h"
#include "ResultLog.h"
#include "GraphPanel.h"
#include "ThroughputDetail.h"

enum { wxID_BUTTON_REMOVE_RESULT = wxID_HIGHEST + 1,
	wxID_THROUGHPUT,
	wxID_THROUGHPUT_BOOK,
	wxID_THROUGHPUT_BOOK_PAGE_SETTINGS,
	wxID_THROUGHPUT_BOOK_PAGE_SETTINGS_LIST,
	wxID_THROUGHPUT_BOOK_PAGE_SETTINGS_CHECKBOX_ENABLED,
	wxID_THROUGHPUT_BOOK_PAGE_SETTINGS_CHECKBOX_TPTEST,
	wxID_THROUGHPUT_BOOK_PAGE_SETTINGS_CHECKBOX_HTTP,
	wxID_THROUGHPUT_BOOK_PAGE_SETTINGS_CHECKBOX_FTP,
	wxID_THROUGHPUT_BOOK_PAGE_RESULT_TEXT,
	wxID_THROUGHPUT_BOOK_PAGE_RESULT_TEXT_LIST
 };

BEGIN_EVENT_TABLE( ThroughputModePanel, wxPanel )
	EVT_BUTTON(wxID_START_BUTTON, ThroughputModePanel::OnStartTest)
	EVT_NOTEBOOK_PAGE_CHANGED(wxID_THROUGHPUT_BOOK, ThroughputModePanel::OnPageChanged)
	EVT_LIST_ITEM_SELECTED(wxID_THROUGHPUT_BOOK_PAGE_SETTINGS_LIST, ThroughputModePanel::OnSettingsListItemSelected)
	EVT_CHECKBOX(wxID_THROUGHPUT_BOOK_PAGE_SETTINGS_CHECKBOX_ENABLED, ThroughputModePanel::OnCheckBoxEnable)
	EVT_CHECKBOX(wxID_THROUGHPUT_BOOK_PAGE_SETTINGS_CHECKBOX_TPTEST, ThroughputModePanel::OnCheckBoxTPTest)
	EVT_CHECKBOX(wxID_THROUGHPUT_BOOK_PAGE_SETTINGS_CHECKBOX_HTTP, ThroughputModePanel::OnCheckBoxHTTP)
	EVT_CHECKBOX(wxID_THROUGHPUT_BOOK_PAGE_SETTINGS_CHECKBOX_FTP, ThroughputModePanel::OnCheckBoxFTP)
	EVT_LIST_KEY_DOWN(wxID_THROUGHPUT_BOOK_PAGE_RESULT_TEXT_LIST, ThroughputModePanel::OnResultListKeyDown)
	EVT_BUTTON(wxID_BUTTON_REMOVE_RESULT, ThroughputModePanel::OnRemoveResult)
	EVT_LIST_ITEM_ACTIVATED(wxID_THROUGHPUT_BOOK_PAGE_RESULT_TEXT_LIST, ThroughputModePanel::OnResultDetail)
END_EVENT_TABLE()


ThroughputModePanel::ThroughputModePanel( wxWindow *parent )
:TestPanel( parent, wxID_THROUGHPUT, wxDefaultPosition, wxDefaultSize )
{
	m_Initialized = false;
	m_SelectedSettingsServer = NULL;
	m_ServerList = new ServerList();

	this->SetSBToolTip( wxString( wxT("Starta ett test som m�ter olika typer av genomstr�mning.\nDe tester som kommer att utf�ras ser du under fliken inst�llningar.") ) 
			    );

	// Create the sizers
	m_SizerPageSettings = new wxBoxSizer( wxVERTICAL );
	m_SizerPageTextResult = new wxBoxSizer( wxVERTICAL );
	m_SizerPageGraphTPTestDownResult = new wxBoxSizer( wxVERTICAL );
	m_SizerPageGraphTPTestUpResult = new wxBoxSizer( wxVERTICAL );
	m_SizerPageGraphHTTPResult = new wxBoxSizer( wxVERTICAL );
	m_SizerPageGraphFTPResult = new wxBoxSizer( wxVERTICAL );

	// Create Result Notebook
	m_BookResult = new wxNotebook( this, wxID_THROUGHPUT_BOOK );

	// Add book to main sizer
	m_SizerMain->Add( m_BookResult, 
				1, 
				wxEXPAND | 
				wxALL |
				wxALIGN_CENTER, 
				0 );

	// Create and add the pages to book
	m_PageSettings = new wxPanel( m_BookResult, wxID_THROUGHPUT_BOOK_PAGE_SETTINGS );
	m_PageTextResult = new wxPanel( m_BookResult, wxID_THROUGHPUT_BOOK_PAGE_RESULT_TEXT );
	m_PageGraphTPTestDownResult = new GraphPanel( m_BookResult, wxID_THROUGHPUT_BOOK_PAGE_RESULT_TPTESTDOWN_GRAPH );
	m_PageGraphTPTestUpResult = new GraphPanel( m_BookResult, wxID_THROUGHPUT_BOOK_PAGE_RESULT_TPTESTUP_GRAPH );
	m_PageGraphHTTPResult = new GraphPanel( m_BookResult, wxID_THROUGHPUT_BOOK_PAGE_RESULT_HTTP_GRAPH );
	m_PageGraphFTPResult = new GraphPanel( m_BookResult, wxID_THROUGHPUT_BOOK_PAGE_RESULT_FTP_GRAPH );

	m_BookResult->AddPage( m_PageGraphTPTestDownResult, wxT("TPTEST nedstr�ms graf") );
	m_BookResult->AddPage( m_PageGraphTPTestUpResult, wxT("TPTEST uppstr�ms graf") );
	m_BookResult->AddPage( m_PageGraphHTTPResult, wxT("HTTP graf") );
	m_BookResult->AddPage( m_PageGraphFTPResult, wxT("FTP graf") );
	m_BookResult->AddPage( m_PageTextResult, wxT("Resultat som lista") );
	m_BookResult->AddPage( m_PageSettings, wxT("Inst�llningar") );


	// Create the Settings List control and create it's columns
	m_ListSettings = new wxListCtrl( 
			m_PageSettings, 
			wxID_THROUGHPUT_BOOK_PAGE_SETTINGS_LIST, 
			wxDefaultPosition, 
			wxDefaultSize, 
			wxLC_REPORT | wxLC_SINGLE_SEL );

	m_ListSettings->InsertColumn(0, wxT(" "), wxLIST_FORMAT_LEFT, 25);
	m_ListSettings->InsertColumn(1, wxT("Adress"), wxLIST_FORMAT_LEFT, 250);
	m_ListSettings->InsertColumn(2, wxT("TPTest"), wxLIST_FORMAT_LEFT, 80);
	m_ListSettings->InsertColumn(3, wxT("HTTP"), wxLIST_FORMAT_LEFT, 80);
	m_ListSettings->InsertColumn(4, wxT("FTP"), wxLIST_FORMAT_LEFT, 80);

	m_SizerPageSettings->Add(	m_ListSettings,
					1,
					wxEXPAND |
					wxALIGN_CENTER |
					wxBOTTOM,
					10 );

	// Create the Result List control and create it's columns
	m_ListResult = new wxListCtrl( 
			m_PageTextResult, 
			wxID_THROUGHPUT_BOOK_PAGE_RESULT_TEXT_LIST, 
			wxDefaultPosition, 
			wxDefaultSize, 
			wxLC_REPORT );

	m_ListResult->InsertColumn(0, wxT("Datum"), wxLIST_FORMAT_LEFT, 150);
	m_ListResult->InsertColumn(1, wxT("TPTEST Nedstr�ms"), wxLIST_FORMAT_LEFT, 100);
	m_ListResult->InsertColumn(2, wxT("TPTEST Upstr�ms"), wxLIST_FORMAT_LEFT, 100);
	m_ListResult->InsertColumn(3, wxT("HTTP"), wxLIST_FORMAT_LEFT, 100);
	m_ListResult->InsertColumn(4, wxT("FTP"), wxLIST_FORMAT_LEFT, 100);

	m_SizerPageTextResult->Add( m_ListResult, 
								1,
								wxEXPAND |
								wxALL |
								wxALIGN_CENTER,
								0 );

	m_SizerPageTextResult->Add( new wxButton( m_PageTextResult, 
						  wxID_BUTTON_REMOVE_RESULT, 
						  wxT("Ta bort valda resultat") ), 
				    0,
				    wxALL |
				    wxALIGN_CENTER,
				    1 );
	
	
	m_SizerPageSettingsEdit = new wxBoxSizer( wxHORIZONTAL );


	m_SizerPageSettingsEdit->Add(
		new wxStaticText( m_PageSettings, wxID_ANY, 
				  wxT("Inst�llningar f�r valt system:") ),
		1,
		wxRIGHT |
		wxLEFT,
		10 );


	// Enabled checkbox
	m_CheckBoxEnable = new wxCheckBox( m_PageSettings, wxID_THROUGHPUT_BOOK_PAGE_SETTINGS_CHECKBOX_ENABLED, wxT("Aktiverad") );

	m_CheckBoxEnable->SetToolTip( wxString( wxT("Aktivera denna server f�r genomstr�mningstest") ) );

	m_SizerPageSettingsEdit->Add(
					m_CheckBoxEnable,
					0,
					wxRIGHT |
					wxBOTTOM,
					1 );

	// TPTest
	m_CheckBoxEnableTPTest = new wxCheckBox( m_PageSettings, 
						 wxID_THROUGHPUT_BOOK_PAGE_SETTINGS_CHECKBOX_TPTEST, 
						 wxT("TPTEST") );

	m_CheckBoxEnableTPTest->SetToolTip( wxString( wxT("K�r TPTEST tester mot denna server") ) );

	m_SizerPageSettingsEdit->Add(
					m_CheckBoxEnableTPTest,
					0,
					wxRIGHT |
					wxBOTTOM,
					1 );

	// HTTP
	m_CheckBoxEnableHTTP = new wxCheckBox( m_PageSettings, 
					       wxID_THROUGHPUT_BOOK_PAGE_SETTINGS_CHECKBOX_HTTP, 
					       wxT("HTTP") );

	m_CheckBoxEnableHTTP->SetToolTip( wxString( wxT("K�r nedstr�ms HTTP tester mot denna server") ) );

	m_SizerPageSettingsEdit->Add(
					m_CheckBoxEnableHTTP,
					0,
					wxRIGHT |
					wxBOTTOM,
					1 );

	// FTP
	m_CheckBoxEnableFTP = new wxCheckBox( m_PageSettings, wxID_THROUGHPUT_BOOK_PAGE_SETTINGS_CHECKBOX_FTP, wxT("FTP") );
	m_CheckBoxEnableFTP->SetToolTip( wxString( wxT("K�r nedstr�ms FTP tester mot denna server") ) );

	m_SizerPageSettingsEdit->Add(
					m_CheckBoxEnableFTP,
					0,
					wxRIGHT |
					wxBOTTOM,
					1 );

	m_SizerPageSettings->Add( m_SizerPageSettingsEdit, 0, wxALIGN_LEFT | wxFIXED_MINSIZE );

	m_SizerMain->SetSizeHints( this );
      	this->SetSizer( m_SizerMain );

	m_PageSettings->SetSizer( m_SizerPageSettings );
	m_PageTextResult->SetSizer( m_SizerPageTextResult );
	m_PageGraphTPTestDownResult->SetSizer( m_SizerPageGraphTPTestDownResult );
	m_PageGraphTPTestUpResult->SetSizer( m_SizerPageGraphTPTestUpResult );
	m_PageGraphHTTPResult->SetSizer( m_SizerPageGraphHTTPResult );
	m_PageGraphFTPResult->SetSizer( m_SizerPageGraphFTPResult );

	m_SizerPageSettings->SetSizeHints( m_PageSettings );
	m_SizerPageTextResult->SetSizeHints( m_PageTextResult );


	// --------
	m_Initialized = true;
	RefreshGUI();

}

ThroughputModePanel::~ThroughputModePanel(void)
{
	delete m_ServerList;
}

void ThroughputModePanel::OnTestCompleted_Wrapper(void* obj, bool arg)
{
	ThroughputModePanel *panel = (ThroughputModePanel*)obj;
	panel->OnTestCompleted( arg );
}

void ThroughputModePanel::OnTestCompleted(bool abort)
{
	TestMarshall *tm = TestMarshall::GetInstance();

	if( !abort )
	{
		ResultLog *rlog = ResultLog::GetInstance();

		StringValueList *row = new StringValueList();
		row->Append( new wxString(wxDateTime::Now().Format( wxT("%Y-%m-%d %H:%M:%S") ) ) );

		// TPTest stuff
/*
		TPEngine *d_engp = tm->GetDownstreamTPEngineStruct();
		TPEngine *u_engp = tm->GetUpstreamTPEngineStruct();

		long	d_TimeSec = d_engp->stats.StopRecv.tv_sec - d_engp->stats.StartRecv.tv_sec;
		long	u_TimeSec = u_engp->stats.StopSend.tv_sec - u_engp->stats.StartSend.tv_sec;

		long	d_TimeuSec = d_engp->stats.StopRecv.tv_usec - d_engp->stats.StartRecv.tv_usec;
		long	u_TimeuSec = u_engp->stats.StopSend.tv_usec - u_engp->stats.StartSend.tv_usec;

		LONG_LONG	d_Bytes = d_engp->stats.BytesRecvd;
		LONG_LONG	u_Bytes	= u_engp->stats.BytesSent;

		double d_Time = ((double)d_TimeSec) + (((double)d_TimeuSec) / (double)1000000);
		double u_Time = ((double)u_TimeSec) + (((double)u_TimeuSec) / (double)1000000);

		double d_Rate = d_Bytes / d_Time;
		double u_Rate = u_Bytes / u_Time;


		wxString *D_TP_Bandwidth = new wxString();
		*D_TP_Bandwidth << wxString::Format( "%f", d_Rate );
		
		wxString *U_TP_Bandwidth = new wxString();
		*U_TP_Bandwidth << wxString::Format( "%f", u_Rate );

		row->Append( D_TP_Bandwidth );
		row->Append( U_TP_Bandwidth );
*/
		// Curl stuff

		curl_get_arg_struct *http	= tm->GetCurlHTTPStruct();
		curl_get_arg_struct *ftp	= tm->GetCurlFTPStruct();

		wxString *HTTP_AvgSpeed = new wxString();
		*HTTP_AvgSpeed << wxString::Format( wxT("%f"), http->avg_speed );

		wxString *FTP_AvgSpeed = new wxString();
		*FTP_AvgSpeed << wxString::Format( wxT("%f"), ftp->avg_speed );



		// Server specific valuelists

		// TPTEST downstream Values
		wxString strVal;
		double d_Avg = 0;
		double d_Count = 0;
		for( int i = 0 ; i < tm->GetTPEngineCount() ; i++ )
		{
			d_Count++;
			TPEngine *d_engp = tm->GetDownstreamTPEngineStruct( i );
			Server *srv = (Server*)d_engp->userdata;
			
			double	d_Bytes = d_engp->stats.BytesRecvd;

			long	d_TimeuSec = d_engp->stats.StopRecv.tv_usec - d_engp->stats.StartRecv.tv_usec;
			long	d_TimeSec = d_engp->stats.StopRecv.tv_sec - d_engp->stats.StartRecv.tv_sec;
			double	d_Time = ((double)d_TimeSec) + (((double)d_TimeuSec) / (double)1000000);
			double	d_Rate = d_Bytes / d_Time;
			d_Avg += d_Rate;
			strVal << srv->Host 
			       << wxT("|") 
			       << wxString::Format( wxT("%f"), d_Rate ) 
			       << wxT("|");
		}

		wxString *strD_TPValues = new wxString( strVal.BeforeLast( wxT('|') ) );

		// TPTEST upstream Values
		strVal.Clear();
		double u_Avg = 0;
		double u_Count = 0;
		for( int i = 0 ; i < tm->GetTPEngineCount() ; i++ )
		{
			u_Count++;
			TPEngine *u_engp = tm->GetUpstreamTPEngineStruct( i );
			Server *srv = (Server*)u_engp->userdata;

			double	u_Bytes = u_engp->stats.BytesSent;

			long	u_TimeuSec = u_engp->stats.StopRecv.tv_usec - u_engp->stats.StartRecv.tv_usec;
			long	u_TimeSec = u_engp->stats.StopRecv.tv_sec - u_engp->stats.StartRecv.tv_sec;
			double	u_Time = ((double)u_TimeSec) + (((double)u_TimeuSec) / (double)1000000);
			double	u_Rate = u_Bytes / u_Time;
			u_Avg += u_Rate;
			strVal << srv->Host << wxT("|") << wxString::Format( wxT("%f"), u_Rate ) << wxT("|");
		}

		wxString *strU_TPValues = new wxString( strVal.BeforeLast('|') );

		wxString *strD_TP_Avg = new wxString();
		wxString *strU_TP_Avg = new wxString();	

		strD_TP_Avg->Printf( wxT("%f"), (d_Avg/d_Count) );
		strU_TP_Avg->Printf( wxT("%f"), (u_Avg/u_Count) );

		row->Append( strD_TP_Avg );
		row->Append( strU_TP_Avg );

		row->Append( HTTP_AvgSpeed );
		row->Append( FTP_AvgSpeed );

		row->Append( strD_TPValues );
		row->Append( strU_TPValues );



		// HTTP Values
		ServerList *http_slist = (ServerList*)http->userdata;
		wxString httpValues;
		for( unsigned int i = 0 ; i < http->number_urls ; i++ )
		{
			Server *s = http_slist->Item(i)->GetData();

			httpValues << s->Host 
				   << wxT("|") 
				   << wxString::Format( wxT("%f"), http->values[i].value ) 
				   << wxT("|");

		}
		wxString *strHttpValues = new wxString( httpValues.BeforeLast( wxT('|') ) );
		row->Append( strHttpValues );

		// FTP Values
		ServerList *ftp_slist = (ServerList*)ftp->userdata;
		wxString ftpValues;
		for( unsigned int i = 0 ; i < ftp->number_urls ; i++ )
		{
			Server *s = ftp_slist->Item(i)->GetData();

			ftpValues << s->Host 
				  << wxT("|") 
				  << wxString::Format( wxT("%f"), ftp->values[i].value ) 
				  << wxT("|");

		}
		wxString *strFtpValues = new wxString( ftpValues.BeforeLast( wxT('|') ) );
		row->Append( strFtpValues );


		rlog->AddResult( wxString( wxT("throughput") ), *row );

		RefreshResultList();
		this->m_PageGraphFTPResult->Refresh();
		this->m_PageGraphHTTPResult->Refresh();
		this->m_PageGraphTPTestDownResult->Refresh();
		this->m_PageGraphTPTestUpResult->Refresh();
	}

	tm->DeinitTestEngine( abort );
	PostTest(abort);
}

void ThroughputModePanel::RefreshResultList(void)
{
	ResultLog *rlog = ResultLog::GetInstance();

	Result *result = rlog->GetResults( wxString( wxT("throughput") ) );

	m_ListResult->DeleteAllItems();

	for( int i = result->GetRowCount()-1 ; i > -1  ; i-- )
	{
		StringValueList *row = result->GetRow( i );

		wxListItem item;
		int id = m_ListResult->GetItemCount();
		item.SetMask(wxLIST_MASK_TEXT);
		item.SetId(id);
		item.SetData( i );
		
		item.SetText( *row->Item(0)->GetData() );
		m_ListResult->InsertItem( item );

		int iD_TP_Bandwidth;
		row->Item(1)->GetData()->ToLong( (long*)&iD_TP_Bandwidth );
		wxString D_TP_Bandwidth;
		if( iD_TP_Bandwidth > 0 )
		  D_TP_Bandwidth << FROMCSTR(Int32ToString(iD_TP_Bandwidth*8));
		else
		  D_TP_Bandwidth << wxT("Inget v�rde");

		m_ListResult->SetItem( id, 1, D_TP_Bandwidth );


		int iU_TP_Bandwidth;
		row->Item(2)->GetData()->ToLong( (long*)&iU_TP_Bandwidth );
		wxString U_TP_Bandwidth;
		if( iU_TP_Bandwidth > 0 )
		  U_TP_Bandwidth << FROMCSTR(Int32ToString(iU_TP_Bandwidth*8));
		else
		  U_TP_Bandwidth << wxT("Inget v�rde");

		m_ListResult->SetItem( id, 2, U_TP_Bandwidth );


		int iHTTP_Bandwidth;
		row->Item(3)->GetData()->ToLong( (long*)&iHTTP_Bandwidth );
		wxString HTTP_Bandwidth;
		if( iHTTP_Bandwidth > 0 )
		  HTTP_Bandwidth << FROMCSTR(Int32ToString(iHTTP_Bandwidth));
		else
		  HTTP_Bandwidth << wxT("Inget v�rde");

		m_ListResult->SetItem( id, 3, HTTP_Bandwidth );

		int iFTP_Bandwidth;
		row->Item(4)->GetData()->ToLong( (long*)&iFTP_Bandwidth );
		wxString FTP_Bandwidth;
		if( iFTP_Bandwidth > 0 )
		  FTP_Bandwidth << FROMCSTR(Int32ToString(iFTP_Bandwidth));
		else
		  FTP_Bandwidth << wxT("Inget v�rde");

		m_ListResult->SetItem( id, 4, FTP_Bandwidth );
	}

	return;
}



void ThroughputModePanel::RefreshSettingsList(void)
{
	m_ListSettings->DeleteAllItems();
	m_ServerList->Clear();

	for ( ServerList::Node *node = g_ServerList->GetFirst(); node; node = node->GetNext() )
	{
		Server *current = node->GetData();
		
		if( current->TPTest_Port.Trim().Trim(false).Length() == 0 &&
			current->FTP_Path.Trim().Trim(false).Length() == 0 && 
			current->HTTP_Path.Trim().Trim(false).Length() == 0 )
		{
			continue;
		}

		m_ServerList->Append( current );

        wxListItem item;
		int id = m_ListSettings->GetItemCount();
		item.SetMask(wxLIST_MASK_TEXT);
		item.SetId(id);

		wxString Enabled = wxT("X");

		if( !current->EnableThroughput )
		{
			item.SetTextColour( wxColour(180,180,180) );
			Enabled = wxT("");
		}

		
		item.SetText( Enabled );
		item.SetData( current );

		m_ListSettings->InsertItem( item );

		m_ListSettings->SetItem( id, 1, current->Host );

		if( current->EnableThroughputTPTest )
		  m_ListSettings->SetItem( id, 2, wxT("X") );

		if( current->EnableThroughputHTTP )
		  m_ListSettings->SetItem( id, 3, wxT("X") );

		if( current->EnableThroughputFTP )
		  m_ListSettings->SetItem( id, 4, wxT("X") );
	}
}

void ThroughputModePanel::RefreshGUI(void)
{
	RefreshSettingsList();
	RefreshResultList();
}


void ThroughputModePanel::OnPageChanged(wxNotebookEvent& event)
{
	if( m_Initialized )
	{
		RefreshGUI();
	}
}

void ThroughputModePanel::OnCheckBoxEnable(wxCommandEvent &event)
{
	if( m_SelectedSettingsServer != NULL )
	{
		m_SelectedSettingsServer->EnableThroughput = m_CheckBoxEnable->GetValue();
		RefreshSettingsList();
		FocusList();
	}
}

void ThroughputModePanel::OnCheckBoxTPTest(wxCommandEvent &event)
{
	if( m_SelectedSettingsServer != NULL )
	{
		if( m_SelectedSettingsServer->TPTest_Port.Trim().Trim(false).Length() == 0 )
		{
			m_CheckBoxEnableTPTest->SetValue(false);
		}

		m_SelectedSettingsServer->EnableThroughputTPTest = m_CheckBoxEnableTPTest->GetValue();
		RefreshSettingsList();
		FocusList();
	}
}

void ThroughputModePanel::OnCheckBoxHTTP(wxCommandEvent &event)
{
	if( m_SelectedSettingsServer != NULL )
	{
		if( m_SelectedSettingsServer->HTTP_Path.Trim().Trim(false).Length() == 0 )
		{
			m_CheckBoxEnableHTTP->SetValue(false);
		}

		m_SelectedSettingsServer->EnableThroughputHTTP = m_CheckBoxEnableHTTP->GetValue();
		RefreshSettingsList();
		FocusList();
	}
}

void ThroughputModePanel::OnCheckBoxFTP(wxCommandEvent &event)
{
	if( m_SelectedSettingsServer != NULL )
	{
		if( m_SelectedSettingsServer->FTP_Path.Trim().Trim(false).Length() == 0 )
		{
			m_CheckBoxEnableFTP->SetValue(false);
		}

		m_SelectedSettingsServer->EnableThroughputFTP = m_CheckBoxEnableFTP->GetValue();
		RefreshSettingsList();
		FocusList();
	}
}

void ThroughputModePanel::OnSettingsListItemSelected(wxListEvent& event)
{
	wxListItem item = event.GetItem();

	long l = item.GetData();
	Server *s = *((Server**)&l);

	m_SelectedSettingsServer = s;
	m_SelectedSettingsServerIndex = item.GetId();

	m_CheckBoxEnable->SetValue( s->EnableThroughput );
	m_CheckBoxEnableTPTest->SetValue( s->EnableThroughputTPTest );
	m_CheckBoxEnableHTTP->SetValue( s->EnableThroughputHTTP );
	m_CheckBoxEnableFTP->SetValue( s->EnableThroughputFTP );
}

void ThroughputModePanel::FocusList(void)
{
		this->m_ListSettings->SetFocus();
		this->m_ListSettings->SetItemState(m_SelectedSettingsServerIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

bool ThroughputModePanel::ExecuteTest(void)
{
	TestMarshall *tm = TestMarshall::GetInstance();

	MainFrame::GetInstance()->PreStartTest((void*)this, (ThroughputModePanel::OnTestCompleted_Wrapper), NULL );

	if( ! tm->InitTestEngine( THROUGHPUT ) )
	  {
	    printf("Failed to initiate test engine\n");
	    return false;
	  }

	if( tm->SetupThroughputest(this->m_ServerList) )
	{
		tm->Execute();
		MainFrame::GetInstance()->StartTest();
		return true;
	}
	else
	{
	  wxMessageBox( wxT("Inga servrar hittade eller inst�llda f�r aktuellt test.") );
		tm->DeinitTestEngine();
		PostTest(true);
		return false;
	}
}

bool ThroughputModePanel::ExecuteTest_Wrapper(void* obj)
{
	ThroughputModePanel *tmp = (ThroughputModePanel*)obj;
	return tmp->ExecuteTest();
}

void ThroughputModePanel::OnRemoveResult(wxCommandEvent& event)
{
	ResultLog *rl = ResultLog::GetInstance();

	Result *r = rl->GetResults( wxString( wxT("throughput") ) );

    long id = m_ListResult->GetNextItem(-1, wxLIST_NEXT_ALL,
                                        wxLIST_STATE_SELECTED);
	while( id != -1 )
	{	
		wxListItem item;
		item.m_mask = wxLIST_MASK_DATA;
		item.SetId( id );
		m_ListResult->GetItem( item );

		int index = (int)item.GetData();

		r->DeleteRow( index );

		id = m_ListResult->GetNextItem(id, wxLIST_NEXT_ALL,
                                       wxLIST_STATE_SELECTED);
	}
	RefreshResultList();
}

void ThroughputModePanel::OnResultDetail(wxListEvent& event)
{
	long id = -1;
                                        
	id = m_ListResult->GetNextItem(id, wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED);

	if( id == -1 )
		return;

	wxListItem item;
	item.SetId( id );
	m_ListResult->GetItem( item );

	wxString s;
	s << id;

	ThroughputDetail *rd = new ThroughputDetail(this, m_ListResult->GetItemCount() - 1 - id);
	rd->ShowModal();
//		delete rd;
}

