#include "State_BattleReport.h"
#include "State_LevelSelection.h"


State_BattleReport::State_BattleReport(StateManager* l_stateManager) 
    : BaseState(l_stateManager), 
      m_report_table(l_stateManager->GetContext()->m_wind->GetWindowSize()) {
  std::cout << "State_BattleReport  Constructor :" << std::endl;
}

void State_BattleReport::OnCreate() {
  EventManager* mngr = m_stateMgr->GetContext()->m_eventManager;
  mngr->AddCallback(StateType::BattleReport, "Return", 
    &State_BattleReport::NextMode, this);
}

void State_BattleReport::OnDestroy() {
}

void State_BattleReport::Activate() {
}

void State_BattleReport::Deactivate() {
}

void State_BattleReport::Update(const sf::Time& l_time) {
  //std::cout << " down  1" << std::endl;
}

void State_BattleReport::Draw() {
  sf::RenderWindow* window = m_stateMgr->GetContext()->m_wind->GetRenderWindow();
  window->clear(sf::Color(10,155,50));
  m_report_table.Draw(*window);
}

void State_BattleReport::NextMode(EventDetails* l_details) {
  using namespace std;
  auto file = m_stateMgr->GetContext()->CurrentStageFile();
  auto file_list = m_stateMgr->GetContext()->getStageList();
  if (file != file_list.front().second) {
    m_stateMgr->SwitchTo(StateType::Battle);
  }
  else {
    m_stateMgr->GetContext()->ResetStage();
    m_stateMgr->SwitchTo(StateType::TanksMenu);
  }
}


const sf::Vector2u ReportTable::s_table_size = {10, 28};
const sf::Vector2f ReportTable::s_sz = {16, 32};


/// \brief класс 
ReportTable::ReportTable(sf::Vector2u sz) 
    : m_window_sz(sz), m_display(s_table_size, s_sz) {
  UpdateInfo(BattleReportData());
  sf::Vector2f d_sz = m_display.getBoundsSize();
  m_display.setPosition(sz.x/2 - d_sz.x/2, sz.y / 2 - d_sz.y / 2);
}


void ReportTable::UpdateInfo(const BattleReportData & data) {
  using namespace std;
  m_display.Clear();

  ostringstream os(report_string_);
  os << setw(s_sz.y / 2) << "hi-score" << setw(6) << data.hi_score << endl;
  os << setw(s_sz.y / 2) << "stage" << setw(4) << data.stage << endl;
  os << setw(8) <<"i-player " << endl;
  os << setw(8) << data.player1 << endl;
  for (int i = 0; i < 4; ++i) {
    os  << setw(4) << data.pts[i] * EnemiIdReward[i] 
      << " pts" << setw(3) << data.pts[i] << "<-" << endl;
  }
  os  << "   total" << setw(3) 
    << data.pts[0] + data.pts[1] + data.pts[2] + data.pts[3] << endl;
  m_display << os.str();
  cout << os.str() << endl;
}


void ReportTable::Draw(sf::RenderWindow & window) {
  m_display.Draw(window);
}


void ReportTable::Update(const sf::Time & l_time) {
}

