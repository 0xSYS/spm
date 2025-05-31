#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/toolbar.h>
#include <wx/artprov.h>
#include <wx/spinctrl.h>
#include <wx/wizard.h>

#include <vector>
#include <string>


#include "../spm_list.hpp"
#include "../spm.hpp"
#include "../utils.hpp"
#include "../sckt_io.hpp"
#include "../wol.hpp"
#include "../config.hpp"
#include "../dev_detect.hpp"
#include "wx/event.h"





//SPMList spmList;
//SPM spm;
//SPMWakeOnLan wol;
//SPM_SocketIO sckt_io;



int selrow;
int selroutine;

wxRadioButton* diag_check;
wxRadioButton* probe_check;
wxRadioButton* scan_check;
wxRadioButton* info_check;
wxRadioButton* activity_check;

wxBitmap bitmap1(wxT("banner.png"), wxBITMAP_TYPE_PNG);
class MyApp : public wxApp {
public:
    virtual bool OnInit();

};

class MyFrame : public wxFrame {
public:
    wxTextCtrl* text = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE | wxTE_RICH);

   

    MyFrame(const wxString& title);

private:
    void CreateDeviceGrid();
    void OnRebootClick(wxCommandEvent& event);
    void OnPoweroffClick(wxCommandEvent& event);
    void OnListening(wxCommandEvent&event);
    void GetRow(wxCommandEvent& event);
    void OnDiagnosis(wxCommandEvent& event);
    void OnWizardClick(wxCommandEvent& event);
    void SendOnSignal(wxCommandEvent& event);
    void LogsToFile(wxCommandEvent& event);
    void ExportDeviceList(wxCommandEvent& event);
    void AddNewDevice(wxCommandEvent& event);
    void RemoveDevice(wxCommandEvent& event);
    void Exit(wxCommandEvent& event);
    void Settings(wxCommandEvent& event);
    
    wxPanel* panel;
    wxGrid* deviceGrid;

   /* 
    struct Device {
        std::string name;
        std::string type;
        std::string status;
    };

    std::vector<Device> devices = {
        {"Device1", "TypeA", "Active"},
        {"Device2", "TypeB", "Inactive"},
        {"Device3", "TypeA", "Active"},
        
    };*/
  
    std::vector<SPMList::device> devices;

};

class MyWizard : public wxWizard
{
public:
    
   MyWizard(const wxString& title);

private:
void Routine(wxWizardEvent& event);
    void OnFinish(wxWizardEvent& event);
};

class MyWizardPage1 : public wxWizardPageSimple
{
public:
    MyWizardPage1(wxWizard* parent);
};

class MyWizardPage2 : public wxWizardPageSimple
{
public:
    MyWizardPage2(wxWizard* parent);
};

class MyWizardPage3 : public wxWizardPageSimple
{
public:
    MyWizardPage3(wxWizard* parent);
};

class MyWizardPage4 : public wxWizardPageSimple
{
public:
    MyWizardPage4(wxWizard* parent);
};

class MyWizardPage5 : public wxWizardPageSimple
{
public:
    MyWizardPage5(wxWizard* parent);
};

class MyWizardPage6 : public wxWizardPageSimple
{
public:
    MyWizardPage6(wxWizard* parent);
};
MyWizardPage1* page1;
MyWizardPage2* page2;
MyWizardPage3* page3;
MyWizardPage4* page4;

MyWizardPage4::MyWizardPage4(wxWizard* parent)
    : wxWizardPageSimple(parent)
{  
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* text2 = new wxStaticText(this, wxID_ANY, "The routine has been added to the list!");
    wxStaticText* text = new wxStaticText(this, wxID_ANY, "Thank you for using the Server Power Management Routine Wizard.");
    wxFont font(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    //  text1->SetFont(font);
    text2->SetFont(font);
    sizer->Add(text2, 0, wxLEFT);
    sizer->Add(text, 0, wxLEFT);
    SetSizer(sizer);
}


MyWizardPage3::MyWizardPage3(wxWizard* parent)
    : wxWizardPageSimple(parent)
{  
   /* wxStaticText* text1 = new wxStaticText(this, wxID_ANY, "What do you want to do with the diagnostic?");
    wxRadioButton* conect_check= new wxRadioButton(this, wxID_ANY, "Check for conectivity");
    wxRadioButton* integrity_check = new wxRadioButton(this, wxID_ANY, "Check SPM integrity");
    wxRadioButton* hash_check = new wxRadioButton(this, wxID_ANY, "Check hash code validity");
    wxRadioButton* repair_check = new wxRadioButton(this, wxID_ANY, "Repair system(minimal, for simple problems)");
*/
    wxStaticText* text2 = new wxStaticText(this, wxID_ANY, "What do you want to do with the probe?");
    scan_check= new wxRadioButton(this, wxID_ANY, "Scan network for IP's");
    info_check = new wxRadioButton(this, wxID_ANY, "Return system information");
    activity_check = new wxRadioButton(this, wxID_ANY, "Look at the system activity");



    wxFont font(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
  //  text1->SetFont(font);
    text2->SetFont(font);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

   
        sizer->Add(text2, 0, wxALL | wxLEFT, 10);
        sizer->Add(scan_check, 0, wxALL | wxLEFT, 10);
        sizer->Add(info_check, 0, wxALL | wxLEFT , 10);
        sizer->Add(activity_check, 0, wxALL | wxLEFT , 10);

    SetSizer(sizer);
}


MyWizardPage1::MyWizardPage1(wxWizard* parent)
    : wxWizardPageSimple(parent)
{
    
    wxStaticText* text = new wxStaticText(this, wxID_ANY, "Welcome to the SPM Routine Wizard(SPMRW)!");      
    wxFont font(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    text->SetFont(font);
    wxStaticText* text2 = new wxStaticText(this, wxID_ANY, "This wizard will guide you into making a routine for all your managed devices.\nUse the button from the bottom of the screen to navigate");
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(text, 0, wxALL | wxCENTER, 10);
    sizer->Add(text2, 0, wxALL | wxCENTER);
    SetSizer(sizer);
}

MyWizardPage2::MyWizardPage2(wxWizard* parent)
    : wxWizardPageSimple(parent)
{
    wxStaticText* text = new wxStaticText(this, wxID_ANY, "Choose the type of the routine.");
    diag_check= new wxRadioButton(this, wxID_ANY, "Diagnostic type routine");
    probe_check = new wxRadioButton(this, wxID_ANY, "Probe type routine");
    wxFont font(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    text->SetFont(font);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(text, 0, wxALL | wxLEFT, 10);
    sizer->Add(diag_check, 0, wxALL | wxLEFT , 20);
    sizer->Add(probe_check, 0, wxALL | wxLEFT , 10);
    SetSizer(sizer);
   
}




MyWizard::MyWizard(const wxString& title) : wxWizard(NULL, wxID_ANY, title, bitmap1)
{
    page1 = new MyWizardPage1(this);
    page2 = new MyWizardPage2(this);
    page3 = new MyWizardPage3(this);
    page4 = new MyWizardPage4(this);
    // Set the next and previous pages
    page1->SetNext(page2);
    page2->SetPrev(page1);
    page2->SetNext(page3);
    page3->SetPrev(page2);
    page3->SetNext(page4);
    // Set the initial page
    GetPageAreaSizer()->Add(page1);
    GetPageAreaSizer()->Layout();
    RunWizard(page1);

    // Connect the finish event
    Bind(wxEVT_WIZARD_FINISHED, &MyWizard::OnFinish, this);
    Bind(wxEVT_WIZARD_PAGE_CHANGING,  &MyWizard::Routine, this);
}

void MyWizard::OnFinish(wxWizardEvent& event)
{
    wxMessageBox("Wizard Finished!", "Info", wxOK | wxICON_INFORMATION);
}
void MyWizard::Routine(wxWizardEvent&event){
    std::cout<<"Am intrat";
    if(diag_check->GetValue())selroutine = 1;
    if(probe_check->GetValue()) selroutine = 2;

}

class ListeningDialog : public wxDialog
{
public:
    ListeningDialog(wxWindow* parent);
private:
class MyThread : public wxThread {
    public:
        MyThread(ListeningDialog* frame) : wxThread(wxTHREAD_JOINABLE), m_frame(frame) {}

        virtual ExitCode Entry() wxOVERRIDE;

    private:
        ListeningDialog* m_frame;
};

};
ListeningDialog::ListeningDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Listening Window", wxDefaultPosition, wxSize(600, 400), wxBORDER_THEME){
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        wxStaticText* text = new wxStaticText(this, wxID_ANY, "SPM is now listening for signals");     
        wxStaticText* text2 = new wxStaticText(this, wxID_ANY, "Close this window if you want to exit out of the listening mode");     
        wxFont font(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        text->SetFont(font);
        sizer->Add(text, 0, wxALL | wxCENTER , 10);
        sizer->Add(text2, 0, wxALL | wxCENTER , 10);
       SetSizer(sizer);
       MyThread* thread = new MyThread(this);
       thread->Create();
       thread->Run();
        
    }


class DiagnosisDialog : public wxDialog
{
public:
    DiagnosisDialog(wxWindow* parent);
};
DiagnosisDialog::DiagnosisDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Settings", wxDefaultPosition, wxSize(600, 400), wxBORDER_THEME){
        wxStaticText* text = new wxStaticText(this, wxID_ANY, "Routine list");      
        wxFont font(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        text->SetFont(font);
        wxListBox* listBox = new wxListBox(this, wxID_ANY);

        listBox->Append("Item 1");
        listBox->Append("Item 2");
        listBox->Append("Item 3");

        wxTextCtrl* log = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE);

        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* sizer2 = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer* sizer3 = new wxBoxSizer(wxVERTICAL);
        sizer3->Add(text, 0, wxALL | wxLEFT , 10);
        sizer3->Add(listBox, 0, wxALL | wxLEFT, 10);
        sizer->Add(log, 1, wxEXPAND, 10);
        sizer2->Add(sizer3, 0, wxALL | wxLEFT, 10);
        sizer2->Add(sizer, 1, wxEXPAND, 10);
        SetSizer(sizer2);
}


class SettingsDialog : public wxDialog
{
public:
    SettingsDialog(wxWindow* parent);
};
SettingsDialog::SettingsDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, "Settings", wxDefaultPosition, wxSize(600, 400))
{
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* size_title = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer* size_cli = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer*  size_time = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer* size_debug = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer* size_light = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer* size_btn = new wxBoxSizer(wxHORIZONTAL);

        wxStaticText* title = new wxStaticText(this, wxID_ANY, "Settings"); 
        wxStaticText* cli = new wxStaticText(this, wxID_ANY, "Open application on CLI mode(needs restart)");
        wxStaticText* time = new wxStaticText(this, wxID_ANY, "Set ping intervals");
        wxStaticText* debug = new wxStaticText(this, wxID_ANY, "Developer logs");
        wxStaticText* light = new wxStaticText(this, wxID_ANY, "Enable light theme");
        
        
        wxFont font(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        title->SetFont(font);

 
 /* wxEmptyString,
		const wxPoint &  	pos = wxDefaultPosition,
		const wxSize &  	size = wxDefaultSize,
		long  	style = wxSP_ARROW_KEYS */
        wxCheckBox* cli_check = new wxCheckBox(this, wxID_ANY, "");
        wxSpinCtrl* time_check = new wxSpinCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 1000, 0);
        wxCheckBox* debug_check = new wxCheckBox(this, wxID_ANY, "");
        wxCheckBox* light_check = new wxCheckBox(this, wxID_ANY, "");
        wxButton* save_exit = new wxButton(this, wxID_ANY, "Save and Exit");
        wxButton* nosave_exit = new wxButton(this, wxID_ANY, "Exit without saving");
        wxButton* save = new wxButton(this, wxID_ANY, "Save");
        
        sizer->Add(size_title, 0, wxALL, 10);
        sizer->Add(size_cli, 0);
        sizer->Add(size_time, 0);
        sizer->Add(size_debug, 0);
        sizer->Add(size_light, 0);
        sizer->Add(size_btn, 0, wxTOP | wxCENTER, 130);

        
        size_btn->Add(save, 0, wxALL, 5);
        size_btn->Add(save_exit, 0, wxALL, 5);
        size_btn->Add(nosave_exit, 0, wxALL, 5);
        size_title->Add( new wxStaticBitmap(this, wxID_ANY, wxArtProvider::GetBitmap("wxART_EDIT", wxART_OTHER, wxSize(32, 32))), 0);
        size_title->Add(title, 0, wxLEFT, 10);
        size_cli->Add(cli, 0, wxALL, 10);
        size_cli->Add(cli_check, 0, wxALL, 10); 
      

        size_time->Add(time, 0, wxALL, 10);
        size_time->Add(time_check, 0, wxALL, 10); 
       
        size_debug->Add(debug, 0, wxALL, 10);
        size_debug->Add(debug_check, 0, wxALL, 10); 
        
        size_light->Add(light, 0, wxALL, 10);
        size_light->Add(light_check, 0, wxALL, 10); 
     
        
    // Expand and add padding
        SetSizer(sizer);
    }
wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
 
    MyFrame* frame = new MyFrame("Server Power Management");
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) {
    wxMenuBar* menuBar = new wxMenuBar();
    wxStreamToTextRedirector redirect(text);
    SPM::Init(nullptr);
    std::cout<<text;
    // Create menus
    wxMenu* signalMenu = new wxMenu();
    wxMenu* fileMenu = new wxMenu();
    wxMenu* diagMenu = new wxMenu();
    wxMenu* setMenu = new wxMenu();
    wxMenu* exitMenu = new wxMenu();

    signalMenu->Append(1, wxT("Send power on signal to selected"));
    signalMenu->Append(6, wxT("Send reboot signal to selected"));
    signalMenu->Append(5, wxT("Send poweroff signal to selected"));
    signalMenu->Append(7, wxT("Open listening screen."));
    diagMenu->Append(wxID_YES, wxT("Routine to selected"));
    diagMenu->Append(wxID_HELP, wxT("Run RWSPM"));

    fileMenu->Append(8, wxT("Add new device"));
    fileMenu->Append(9, wxT("Remove device"));
    fileMenu->Append(wxID_OPEN, wxT("Import device list"));
    fileMenu->Append(wxID_SAVEAS, wxT("Export device list"));
    fileMenu->Append(wxID_SAVE, wxT("Write logs to file"));
    

    setMenu->Append(wxID_ABOUT, wxT("Settings"));

    exitMenu->Append(wxID_EXIT, wxT("Exit application"));

    // Add the server menu to the menu bar
    menuBar->Append(signalMenu, wxT("Signals"));
    menuBar->Append(diagMenu, wxT("Routines"));
    menuBar->Append(fileMenu, wxT("File menu"));
    menuBar->Append(setMenu, wxT("Settings"));
    menuBar->Append(exitMenu, wxT("Exit"));


    // Set the menu bar for the frame
    SetMenuBar(menuBar);
    Bind(wxEVT_TOOL, &MyFrame::SendOnSignal, this, 1);
    Bind(wxEVT_TOOL, &MyFrame::OnRebootClick, this, 6);
    Bind(wxEVT_TOOL, &MyFrame::OnPoweroffClick, this, 5);
    Bind(wxEVT_TOOL, &MyFrame::OnListening, this, 7);
    Bind(wxEVT_TOOL, &MyFrame::OnWizardClick, this, wxID_HELP);
    Bind(wxEVT_TOOL, &MyFrame::LogsToFile, this, wxID_SAVE);
    Bind(wxEVT_TOOL, &MyFrame::ExportDeviceList, this, wxID_SAVEAS);
    Bind(wxEVT_TOOL, &MyFrame::AddNewDevice, this, 8);
    Bind(wxEVT_TOOL, &MyFrame::RemoveDevice, this, 9);
    Bind(wxEVT_TOOL, &MyFrame::Settings, this, wxID_ABOUT);
    Bind(wxEVT_TOOL, &MyFrame::Exit, this, wxID_EXIT);
    Bind(wxEVT_TOOL, &MyFrame::OnDiagnosis, this, wxID_YES);
    CreateDeviceGrid();
   
   

}

void MyFrame::CreateDeviceGrid() {
   // std::streambuf *sbOld = std::cout.rdbuf();

    /* wxEmptyString,
		const wxPoint &  	pos = wxDefaultPosition,
		const wxSize &  	size = wxDefaultSize,  */
    
    devices = SPMList::ReadDevList();
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  
    deviceGrid = new wxGrid(this, wxID_ANY);
    deviceGrid->CreateGrid(devices.size(), 8); // 6 columns for name, type, status

    // Set column labels
    deviceGrid->SetColLabelValue(0, "Name");
    deviceGrid->SetColLabelValue(1, "IP");
    deviceGrid->SetColLabelValue(2, "Broadcast IP");
    deviceGrid->SetColLabelValue(3, "Mac Addres");
    deviceGrid->SetColLabelValue(4, "Status");
    deviceGrid->SetColLabelValue(5, "Notes");

    deviceGrid->SetColSize(0, 150);
    deviceGrid->SetColSize(1, 150);
    deviceGrid->SetColSize(2, 150);
    deviceGrid->SetColSize(3, 210);
    deviceGrid->SetColSize(4, 90);
    deviceGrid->SetColSize(5, 480);

    // Populate the grid with device data
    for (size_t i = 0; i < devices.size(); ++i) {
        deviceGrid->SetCellValue(i, 0, devices[i].name);
        deviceGrid->SetCellValue(i, 1, devices[i].os_ip);
        deviceGrid->SetCellValue(i, 2, devices[i].broadcast_ip);
        deviceGrid->SetCellValue(i, 3, devices[i].mac_addr);
        deviceGrid->SetCellValue(i, 5, devices[i].notes);
    }

    // Set the text color for the entire grid   
    deviceGrid->EnableEditing(false);
    // Set the grid to resize with the frame
    deviceGrid->SetSize(this->GetSize());
    sizer->Add(deviceGrid, 2, wxEXPAND);
    sizer->Add(text, 1, wxEXPAND); // Expand and add padding
    std::cout<<"weew";
     deviceGrid->Bind(wxEVT_GRID_CELL_LEFT_CLICK, &MyFrame::GetRow, this);
    //sizer->Add(deviceGrid, 1, wxEXPAND | wxCENTER, 10); // Expand and add padding
    //sizer->Add(text, 1, wxEXPAND | wxCENTER, 10); // Expand and add padding
    // Set the sizer for the frame
    SetSizer(sizer);
}
/*void OnRebootClick(wxCommandEvent& event);
    void OnWizardClick(wxCommandEvent& event); */
void MyFrame::OnListening(wxCommandEvent& event) {
    text->AppendText("Started listening on port 8008");
    ListeningDialog dialog(this);
    dialog.ShowModal();
    dialog.Destroy();
}

void MyFrame::SendOnSignal(wxCommandEvent& event)
{
    wxString broadcast_addr = deviceGrid->GetCellValue(selrow, 2);
    wxString mac_addr = deviceGrid->GetCellValue(selrow, 3); 
    SPMWakeOnLan::SndMagicPack(mac_addr.ToStdString(), broadcast_addr.ToStdString());
    wxMessageBox("Signal has been sent succesfully!", "Signal", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnPoweroffClick(wxCommandEvent& event)
{
    wxString dev_ip = deviceGrid->GetCellValue(selrow, 1);
    SPM_SocketIO::SndPowerAction(SPM_SocketIO::Poweroff, dev_ip.ToStdString());
    // wxMessageBox("Signal has been sent successfully !", "Signal poweroff", wxOK || wxICON_INFORMATION);
}

void MyFrame::OnRebootClick(wxCommandEvent& event)
{
    wxString dev_ip = deviceGrid->GetCellValue(selrow, 1);
    std::cout << "IP: " << dev_ip.ToStdString() << "\n";
    SPM_SocketIO::SndPowerAction(SPM_SocketIO::Reboot, dev_ip.ToStdString());
    // wxMessageBox("Signal has been sent succesfully!", "Signal reboot", wxOK | wxICON_INFORMATION);
}

void MyFrame::AddNewDevice(wxCommandEvent& event)
{
    std::cout << "Add new Device...\n";
}

void MyFrame::RemoveDevice(wxCommandEvent& event)
{
    std::cout << "Remove Device...\n";
}

void MyFrame::OnWizardClick(wxCommandEvent& event){
    MyWizard* wizard = new MyWizard("SPMRW");
    wizard->ShowModal();
    wizard->Destroy();
}
void MyFrame::OnDiagnosis(wxCommandEvent& event){
    DiagnosisDialog dialog(this);
    dialog.ShowModal();
    dialog.Destroy();

}
void MyFrame::GetRow(wxCommandEvent& event){
    selrow = deviceGrid->GetGridCursorRow();
    wxString msg;
    msg<<selrow;
    // wxMessageBox(msg, "Signal reboot", wxOK | wxICON_INFORMATION);

    
}

void MyFrame::LogsToFile(wxCommandEvent& event) {
    wxFileDialog saveFileDialog(this, "Save File", "", "", 
        "Text Files (*.txt)|*.txt|All Files (*.*)|*.*", wxFD_OPEN);
        saveFileDialog.ShowModal();
}

void MyFrame::ExportDeviceList(wxCommandEvent& event) {
    wxFileDialog saveFileDialog(this, "Save File", "", "", 
        "Text Files (*.txt)|*.txt|All Files (*.*)|*.*", wxFD_OPEN);
        saveFileDialog.ShowModal();
}
void MyFrame::Exit(wxCommandEvent& event) {
    Close();
}
 void MyFrame::Settings(wxCommandEvent& event){
    SettingsDialog dialog(this);
    dialog.ShowModal();
    dialog.Destroy();

 }

 ListeningDialog::MyThread::ExitCode ListeningDialog::MyThread::Entry() {
    // startServer();
    return (wxThread::ExitCode)0;
}
