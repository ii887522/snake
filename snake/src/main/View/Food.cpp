// Copyright ii887522

#ifndef TEST

#include "Food.h"
#include <Any/View.h>
#include <Struct/Rect.h>
#include <Struct/Point.h>
#include <Any/Map.h>
#include <Any/Reactive.h>
#include <SDL.h>
#include <random>
#include "Any/Enums.h"

using std::default_random_engine;
using std::uniform_int_distribution;
using ii887522::viewify::View;
using ii887522::viewify::Rect;
using ii887522::viewify::Point;
using ii887522::viewify::Map;
using ii887522::nitro::Reactive;

namespace ii887522::snake {

Food::Food(SDL_Renderer*const renderer, const Rect<int>& wallRect, const int cellSize, default_random_engine& randomEngine,
  Map<CellType>& map, Reactive<bool>*const isEaten) : View{ renderer }, xs{ 1u, wallRect.size.w / cellSize - 2u },
  ys{ 1u, wallRect.size.h / cellSize - 2u }, randomEngine{ randomEngine }, position{ Point{ 0u, 0u } }, map{ map }, cellSize{ cellSize } {
  isEaten->watch([this](const bool& value, const int) {
    if (!value) return;
    spawn();
  });
  position.watch([this, wallRect, cellSize](const Point<unsigned int>& value, const int) {
    getPosition().set(wallRect.position + static_cast<Point<int>>(value * cellSize));
  });
  spawn();
}

Point<unsigned int> Food::getRandomSpawnablePosition() {
  Point l_position{ xs(randomEngine), ys(randomEngine) };
  while (true) {
    if (map.get(l_position) != CellType::BACKGROUND) {
      ++l_position.x;
    } else {
      break;
    }
    if (l_position.x > xs.max()) {
      l_position.x = xs.min();
      ++l_position.y;
    }
    if (l_position.y > ys.max()) l_position.y = ys.min();
  }
  return l_position;
}

void Food::spawn() {
  position.set(getRandomSpawnablePosition());
  map.set(position.get(), CellType::FOOD);
}

void Food::render() {
  SDL_SetRenderDrawColor(getRenderer(), 255u, 255u, 0u, 255u);
  const SDL_Rect rect{ getPosition().get().x, getPosition().get().y, cellSize, cellSize };
  SDL_RenderFillRect(getRenderer(), &rect);
}

}  // namespace ii887522::snake

#endif
