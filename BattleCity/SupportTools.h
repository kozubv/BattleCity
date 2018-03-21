#pragma once
#include <iostream>
#include <SFML\Graphics\Rect.hpp>
#include <SFML\Graphics\Sprite.hpp>

template < typename T>
std::ostream& operator<<(std::ostream& os, const sf::Rect<T> & r) {
  std::cout << "rect : " << " x :" << r.left
    << " y :" << r.top
    << " w :" << r.width
    << " h :" << r.height;
  return os;
}


inline void toCenter(sf::Sprite& sp) {
  sf::FloatRect rect = sp.getLocalBounds();
  sp.setOrigin(rect.width / 2, rect.height / 2);
  return;
}


