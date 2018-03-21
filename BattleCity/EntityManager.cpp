#include "EntityManager.h"

void EntityManager::Update(const sf::Time& time) {
  GameEntity::Entity_ptr_list::iterator support;
  for (auto first = GameEntity::g_entity_list.begin(); first != GameEntity::g_entity_list.end(); )
  {
    if ((*first)->isDestroyed()) {
      support = first;   // because list iterator will be lost
      ++support;
      GameEntity::g_entity_list.erase(first);
      first = support;
    }
    else {
      (*first)->Update(time);
      ++first;
    }
  }
}

void EntityManager::Draw(sf::RenderWindow& window) {
  for (auto& item : GameEntity::g_entity_list) {
    item->Draw(window);
  }
  // отображение элементов верхнего уровня (деревьев)
  for (auto& item : Barrier::factory::s_current_set) {
    if(item.isTopDrawLayer()) item.Draw(window);
  }
}

void EntityManager::Interaction() {
  sf::Clock cl;

  SingleTipeCombine<Tank>();
  DoubleTypeCombine<Tank, Bullet>();
  DoubleTypeCombine<Tank, Barrier>();
  DoubleTypeCombine<Tank, Bonus>();
  SingleTipeCombine<Bullet>();
  DoubleTypeCombine<Bullet, Barrier>();
  DoubleTypeCombine<Bullet, Bonus>();
}
