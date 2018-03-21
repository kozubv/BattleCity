/// \file 
/// \ingroup battle_city_game_classes
#pragma once

#include <SFML\Graphics.hpp>

#include "EntityBase.h"
#include "Units.h"


///  \brief ќсуществл¤ет централизованное управление экземпл¤рами GameEntity 
///
/// јбстрагирует процедуры, касающие¤ каждого существующего на текущем шаге 
/// экземпл¤ра GameEntity, от непосредственного обращени¤ к этому экземпл¤ру.
/// 
/// ¬ то врем¤, как Factory отвечает за создание и учет объектов GameEntity, EntityManager
/// отвечает за обновление их состо¤ний и удаление.
/// 
/// –еализаци¤ методов использует хранилища классов GameEntity::Entity_ptr_list 
/// и Factory::CurrentSet дл¤ доступа к наборам экземпл¤ров.
/// \see interaction_processing_algorithms, Factory, BattleCity::Update
class EntityManager {
 public:
  /// обновляет состо¤ние всех игровых единиц
  /// \param time врем¤ от начала старта
  void Update(const sf::Time& time);

  /// ¬ыводит графическое представление в окне программы
  void Draw(sf::RenderWindow&);

  /// ќбрабатывает попарные взаимодействи¤ всех существующих на данном шаге объектов
  /// при столкновении (когда становитс¤ не пустой область пересечени¤ их геометрических форм)
  void Interaction();
};


/// \defgroup interaction_processing_algorithms Алгоритмы проверки взаимодействий
/// \brief Алгоритмы, предназначенные для перебора можеств взаимодействующих в игре элементов

/// \ingroup interaction_processing_algorithms
/// \brief BinaryOperation вызываетс¤ по правилам комбинаторных сочетаний (k = 2)
/// дл¤ элементов переданной последовательности длины \a n.
/// —ложность : \f[ (n^2 + n) / 2 \f]
template < typename ForwardIt, typename BinaryOperation>
void unique_combination(ForwardIt first, ForwardIt last, BinaryOperation Operation) {
  using namespace std;
  for (; first != last; ++first) {
    ForwardIt iter(first);
    ++iter;
    for (; iter != last; ++iter) {
      Operation(*first, *iter);
    }
  }
}

/// \ingroup interaction_processing_algorithms
/// \brief BinaryOperation вызываетс¤ дл¤ каждого сочетани¤ 
/// элементов ForwardIt1 с элементами ForwardIt2.
/// сложность : \f[ n_{ForwardIt1} * n_{ForwardIt2} \f]
template < typename ForwardIt1, typename ForwardIt2, typename BinaryOperation>
void combination(ForwardIt1 first1, ForwardIt1 last1,
                 ForwardIt2 first2, ForwardIt2 last2,
                 BinaryOperation Operation) {
  using namespace std;
  for (; first1 != last1; ++first1) {
    ForwardIt2 iter(first2);
    for (; iter != last2; ++iter) {
      Operation(*first1, *iter);
    }
  }
}

/// \ingroup interaction_processing_algorithms
/// \brief ќсуществл¤ет все парные взаимодействи¤ юнитов одного типа
/// \tparam T1 должен использовать в качестве фабрики класс Factory и 
///    определ¤ть перегрузку функции EntityInteraction
template <typename T1>
void SingleTipeCombine() {
  void(*fun_inter)(T1&, T1&) = EntityInteraction;
  unique_combination(T1::factory::s_current_set.begin(),
                     T1::factory::s_current_set.end(),
                     fun_inter);
}

/// \ingroup interaction_processing_algorithms
/// \brief ќсуществл¤ет все парные взаимодействи¤ юнитов разных типов
/// \tparam T1,T2 должны использовать в качестве фабрики класс Factory и 
///   определ¤ть перегрузку функции EntityInteraction(T1, T2)
template <typename T1, typename T2>
void DoubleTypeCombine() {
  void(*fun_inter)(T1&, T2&) = EntityInteraction;
  combination(T1::factory::s_current_set.begin(),
              T1::factory::s_current_set.end(),
              T2::factory::s_current_set.begin(),
              T2::factory::s_current_set.end(),
              fun_inter);
}

