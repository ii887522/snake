// Copyright ii887522

#ifndef SNAKE_SRC_MAIN_VIEW_SNAKE_H_
#define SNAKE_SRC_MAIN_VIEW_SNAKE_H_

#ifndef TEST

#include <Any/View.h>
#include <Struct/Rect.h>
#include <Any/Reactive.h>
#include <Any/Map.h>
#include <Struct/Point.h>
#include <SDL.h>
#include <functional>
#include <random>
#include "../Model/SnakeModel.h"
#include "../Any/Enums.h"

using std::function;
using std::default_random_engine;
using ii887522::viewify::View;
using ii887522::viewify::Rect;
using ii887522::nitro::Reactive;
using ii887522::viewify::Map;
using ii887522::viewify::Point;

namespace ii887522::snake {

// Not Thread Safe: it must only be used in main thread
// See also ../Any/View.h for more details
class Snake final : public View {
  // remove copy semantics
  Snake(const Snake&) = delete;
  Snake& operator=(const Snake&) = delete;

  // remove move semantics
  Snake(Snake&&) = delete;
  Snake& operator=(Snake&&) = delete;

 public:
  // Not Thread Safe
  class Builder final {
    // remove copy semantics
    Builder(const Builder&) = delete;
    Builder& operator=(const Builder&) = delete;

    // remove move semantics
    Builder(Builder&&) = delete;
    Builder& operator=(Builder&&) = delete;

    SDL_Renderer*const renderer;
    const Rect<int> wallRect;
    const int cellSize;
    Reactive<bool>* isDead;
    bool hasSetDead;
    default_random_engine& randomEngine;
    Map<CellType>& map;
    Reactive<bool>* hasEatFood;
    bool hasSetHasEatFood;
    const function<void()> onHit;

   public:
    // Param renderer: it must not be assigned to integer
    explicit Builder(SDL_Renderer*const renderer, const Rect<int>& wallRect, const int cellSize, default_random_engine&,
      Map<CellType>&, const function<void()>& onHit);

    // Must Call Time(s): At least 1
    constexpr Builder& setDead(Reactive<bool>*const value) {
      isDead = value;
      hasSetDead = true;
      return *this;
    }

    // Must Call Time(s): At least 1
    constexpr Builder& setHasEatFood(Reactive<bool>*const value) {
      hasEatFood = value;
      hasSetHasEatFood = true;
      return *this;
    }

    // Must Call Time(s): At least 1
    Snake* build();

    friend class Snake;
  };

 private:
  SnakeModel model;
  const Point<int> wallPosition;

  explicit Snake(const Builder&);

 public:
  void reactKeyDown(const SDL_KeyboardEvent&) override;
  void step(const unsigned int dt) override;
  void checkAndReactHits(const unsigned int dt) override;
  void render() override;
};

}  // namespace ii887522::snake

#endif
#endif  // SNAKE_SRC_MAIN_VIEW_SNAKE_H_
