// Copyright ii887522

#ifndef TEST

#include "Snake.h"
#include <Any/View.h>
#include <Struct/Rect.h>
#include <Any/Reactive.h>
#include <Any/Map.h>
#include <SDL.h>
#include <functional>
#include <random>
#include <stdexcept>
#include <Any/Enums.h>
#include "../Any/Enums.h"

using std::function;
using std::default_random_engine;
using std::runtime_error;
using ii887522::viewify::Action;

namespace ii887522::snake {

Snake::Builder::Builder(SDL_Renderer*const renderer, const Rect<int>& wallRect, const int cellSize, default_random_engine& randomEngine,
  Map<CellType>& map, const function<void()>& onHit) : renderer{ renderer }, wallRect{ wallRect }, cellSize{ cellSize },
  isDead{ nullptr }, hasSetDead{ false }, randomEngine{ randomEngine }, map{ map }, hasEatFood{ nullptr }, hasSetHasEatFood{ false },
  onHit{ onHit } { }

Snake* Snake::Builder::build() {
  if (!hasSetDead) throw runtime_error{ "Snake isDead is required!" };
  if (!hasSetHasEatFood) throw runtime_error{ "Snake hasEatFood is required!" };
  return new Snake{ *this };
}

Snake::Snake(const Builder& builder) : View{ builder.renderer },
  model{ builder.wallRect.size, builder.cellSize, builder.randomEngine, builder.map, builder.hasEatFood, builder.onHit },
  wallPosition{ builder.wallRect.position } {
  builder.isDead->watch([this](const bool& value, const int) {
    model.setDead(value);
  });
}

Action Snake::reactKeyDown(const SDL_KeyboardEvent& keyEvent) {
  model.reactKeyDown(keyEvent.keysym.sym);
  return Action::QUIT;
}

void Snake::step(const unsigned int dt) {
  model.step(dt);
}

void Snake::checkAndReactHits(const unsigned int dt) {
  model.checkAndReactHits(dt);
}

void Snake::render() {
  SDL_SetRenderDrawColor(getRenderer(), 0u, 255u, 0u, 255u);
  for (const auto& modelPosition : model.getPositions()) {
    const SDL_Rect rect{
      static_cast<int>(wallPosition.x + modelPosition.get().x * model.getCellSize()),
      static_cast<int>(wallPosition.y + modelPosition.get().y * model.getCellSize()), model.getCellSize(), model.getCellSize()
    };
    SDL_RenderFillRect(getRenderer(), &rect);
  }
}

}  // namespace ii887522::snake

#endif
