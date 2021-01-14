// Copyright ii887522

#ifndef SNAKE_SRC_MAIN_VIEW_FOOD_H_
#define SNAKE_SRC_MAIN_VIEW_FOOD_H_

#ifndef TEST

#include <Any/View.h>
#include <Struct/Rect.h>
#include <Any/Map.h>
#include <Any/Reactive.h>
#include <Struct/Point.h>
#include <SDL.h>
#include <random>
#include "../Any/Enums.h"

using std::uniform_int_distribution;
using std::default_random_engine;
using ii887522::viewify::View;
using ii887522::viewify::Rect;
using ii887522::viewify::Map;
using ii887522::nitro::Reactive;
using ii887522::viewify::Point;

namespace ii887522::snake {

// Not Thread Safe: it must only be used in main thread
class Food final : public View {
  // remove copy semantics
  Food(const Food&) = delete;
  Food& operator=(const Food&) = delete;

  // remove move semantics
  Food(Food&&) = delete;
  Food& operator=(Food&&) = delete;

  uniform_int_distribution<unsigned int> xs;
  uniform_int_distribution<unsigned int> ys;
  default_random_engine& randomEngine;
  Reactive<Point<unsigned int>> position;
  Map<CellType>& map;
  const int cellSize;

  Point<unsigned int> getRandomSpawnablePosition();
  void spawn();

 public:
  // Param renderer: it must not be assigned to integer
  explicit Food(SDL_Renderer*const renderer, const Rect<int>& wallRect, const int cellSize, default_random_engine&, Map<CellType>&,
    Reactive<bool>*const isEaten);

  void render() override;
};

}  // namespace ii887522::snake

#endif
#endif  // SNAKE_SRC_MAIN_VIEW_FOOD_H_
