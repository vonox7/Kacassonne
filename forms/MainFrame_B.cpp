///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Mar 24 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "TablePanel.h"

#include "MainFrame_B.h"

///////////////////////////////////////////////////////////////////////////

MainFrame_B::MainFrame_B( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	m_menubar1 = new wxMenuBar( 0 );
	m_menu1 = new wxMenu();
	wxMenuItem* restart_menu_item_;
	restart_menu_item_ = new wxMenuItem( m_menu1, wxID_ANY, wxString( wxT("Restart") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( restart_menu_item_ );

	wxMenuItem* new_game_menu_item_;
	new_game_menu_item_ = new wxMenuItem( m_menu1, wxID_ANY, wxString( wxT("New Game") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( new_game_menu_item_ );

	wxMenuItem* quit_menu_item_;
	quit_menu_item_ = new wxMenuItem( m_menu1, wxID_ANY, wxString( wxT("Quit") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( quit_menu_item_ );

	m_menubar1->Append( m_menu1, wxT("File") );

	m_menu2 = new wxMenu();
	m_menubar1->Append( m_menu2, wxT("Edit") );

	help_menu_ = new wxMenu();
	m_menubar1->Append( help_menu_, wxT("?") );

	this->SetMenuBar( m_menubar1 );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );

	table_panel_ = new TablePanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL );
	bSizer1->Add( table_panel_, 1, wxEXPAND | wxALL, 0 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	bSizer3->SetMinSize( wxSize( 200,-1 ) );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	next_button_ = new wxButton( this, wxID_ANY, wxT("Next"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( next_button_, 0, wxALL, 5 );

	back_button_ = new wxButton( this, wxID_ANY, wxT("Back"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( back_button_, 0, wxALL, 5 );

	shuffle_button_ = new wxButton( this, wxID_ANY, wxT("Shuffle"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( shuffle_button_, 0, wxALL, 5 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Name, Left Stones, Points"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer4->Add( m_staticText2, 0, wxALL, 5 );


	bSizer3->Add( bSizer4, 0, wxEXPAND, 5 );

	info_sizer_ = new wxBoxSizer( wxVERTICAL );

	info_sizer_->SetMinSize( wxSize( 200,-1 ) );

	bSizer3->Add( info_sizer_, 1, wxEXPAND, 5 );

	preview_bitmap_ = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 160,160 ), 0 );
	bSizer3->Add( preview_bitmap_, 0, wxALIGN_CENTER|wxALL, 5 );


	bSizer1->Add( bSizer3, 0, wxEXPAND, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_menu1->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_B::restart ), this, restart_menu_item_->GetId());
	m_menu1->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_B::newGame ), this, new_game_menu_item_->GetId());
	m_menu1->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_B::quit ), this, quit_menu_item_->GetId());
	next_button_->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame_B::next ), NULL, this );
	back_button_->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame_B::back ), NULL, this );
	shuffle_button_->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame_B::shuffle ), NULL, this );
}

MainFrame_B::~MainFrame_B()
{
	// Disconnect Events
	next_button_->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame_B::next ), NULL, this );
	back_button_->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame_B::back ), NULL, this );
	shuffle_button_->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame_B::shuffle ), NULL, this );

}
