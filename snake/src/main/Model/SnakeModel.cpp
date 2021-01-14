// Copyright ii887522

#include "SnakeModel.h"
#include <Struct/Point.h>
#include <Struct/Vector.h>
#include <Struct/Size.h>
#include <Any/AnimatedAny.h>
#include <Any/Reactive.h>
#include <Functions/queue_ext.h>
#include <Any/Map.h>
#include <SDL.h>
#include <functional>
#include <random>
#include "Any/Enums.h"

using std::function;
using std::default_random_engine;
using ii887522::viewify::Point;
using ii887522::nitro::Vector;
using ii887522::viewify::Size;
using ii887522::nitro::AnimatedAny;
using ii887522::nitro::Reactive;
using ii887522::nitro::clear;
using ii887522::viewify::Map;

namespace ii887522::snake {

SnakeModel::SnakeModel(const Size<int>& wallSize, const int cellSize, default_random_engine& randomEngine, Map<CellType>& map,
  Reactive<bool>*const hasEatFood, const function<void()>& onHit) : wallSize{ wallSize }, cellSize{ cellSize }, duration{ 62u },
  directions{ 0u, static_cast<unsigned int>(Direction::COUNT) - 1u }, randomEngine{ randomEngine }, isDead{ false }, map{ map },
  hasEatFood{ *hasEatFood }, prevTailSpeed{ 0, 0 }, onHit{ onHit } {
  reborn();
}

void SnakeModel::reactEatFood(const unsigned int dt) {
  grow(dt);
  hasEatFood.set(true);
}

void SnakeModel::grow(const unsigned int dt) {
  if (isTailMoveUpPreviously()) growWhenTailMoveUpPreviously();
  else if (isTailMoveRightPreviously()) growWhenTailMoveRightPreviously();
  else if (isTailMoveDownPreviously()) growWhenTailMoveDownPreviously();
  else if (isTailMoveLeftPreviously()) growWhenTailMoveLeftPreviously();
  positions.back().set(positions.back().get() + speeds.back());
  positions.back().step(dt);
}

void SnakeModel::fixPositions() {
  for (auto i{ 0u }; i != positions.size(); ++i) {
    if (isMoveUp(i))
      positions[i].teleport(Point{ positions[i].get().x, static_cast<float>(static_cast<int>(positions[i].get().y + 1.f)) });
    else if (isMoveRight(i))
      positions[i].teleport(Point{ static_cast<float>(static_cast<int>(positions[i].get().x)), positions[i].get().y });
    else if (isMoveDown(i))
      positions[i].teleport(Point{ positions[i].get().x, static_cast<float>(static_cast<int>(positions[i].get().y)) });
    else if (isMoveLeft(i))
      positions[i].teleport(Point{ static_cast<float>(static_cast<int>(positions[i].get().x + 1.f)), positions[i].get().y });
  }
}

void SnakeModel::stop() {
  for (auto& speed : speeds) speed = Vector{ 0, 0 };
  prevTailSpeed = Vector{ 0, 0 };
}

bool SnakeModel::isHitSelf() const {
  return isMoveUp(0u) ? map.get(static_cast<Point<unsigned int>>(positions.front().get())) == CellType::SNAKE_BODY :
    isMoveRight(0u) ? map.get(static_cast<Point<unsigned int>>(positions.front().get() + Vector{ .999f, 0.f })) == CellType::SNAKE_BODY :
    isMoveDown(0u) ? map.get(static_cast<Point<unsigned int>>(positions.front().get() + Vector{ 0.f, .999f })) == CellType::SNAKE_BODY :
    isMoveLeft(0u) ? map.get(static_cast<Point<unsigned int>>(positions.front().get())) == CellType::SNAKE_BODY : false;
}

bool SnakeModel::isHitTopWall() const {
  return isMoveUp(0u) && map.get(static_cast<Point<unsigned int>>(positions.front().get())) == CellType::WALL;
}

bool SnakeModel::isHitRightWall() const {
  return isMoveRight(0u) && map.get(static_cast<Point<unsigned int>>(positions.front().get() + Vector{ .999f, 0.f })) == CellType::WALL;
}

bool SnakeModel::isHitBottomWall() const {
  return isMoveDown(0u) && map.get(static_cast<Point<unsigned int>>(positions.front().get() + Vector{ 0.f, .999f })) == CellType::WALL;
}

bool SnakeModel::isHitLeftWall() const {
  return isMoveLeft(0u) && map.get(static_cast<Point<unsigned int>>(positions.front().get())) == CellType::WALL;
}

void SnakeModel::reactHit() {
  fixPositions();
  stop();
  isDead = true;
  onHit();
}

void SnakeModel::reactNextKeyCode() {
  if (keyCodes.empty()) return;
  switch (keyCodes.front()) {
    case SDLK_w: reactWKey();
      break;
    case SDLK_s: reactSKey();
      break;
    case SDLK_a: reactAKey();
      break;
    case SDLK_d: reactDKey();
  }
  keyCodes.pop();
}

void SnakeModel::clearSnakeInMap() {
  for (const auto& position : positions) map.set(static_cast<Point<unsigned int>>(position.get()), CellType::BACKGROUND);
}

void SnakeModel::setRandomSpeed() {
  switch (static_cast<Direction>(directions(randomEngine))) {
    case Direction::UP: speeds.push_back(Vector{ 0, -1 });
      break;
    case Direction::RIGHT: speeds.push_back(Vector{ 1, 0 });
      break;
    case Direction::DOWN: speeds.push_back(Vector{ 0, 1 });
      break;
    case Direction::LEFT: speeds.push_back(Vector{ -1, 0 });
  }
  prevTailSpeed = speeds.front();
}

void SnakeModel::reborn() {
  clearSnakeInMap();
  positions.clear();
  speeds.clear();
  clear(&keyCodes);
  positions.push_back(AnimatedAny<Point<float>>::Builder{
    static_cast<Point<float>>(Point{ wallSize.w / cellSize >> 1u, wallSize.h / cellSize >> 1u }), [this]() {
      if (isMoveUp(0u))
        map.set(static_cast<Point<unsigned int>>(positions.front().get() + Vector{ 0.f, 1.f }), CellType::BACKGROUND);
      else if (isMoveRight(0u))
        map.set(static_cast<Point<unsigned int>>(positions.front().get() - Vector{ 1.f, 0.f }), CellType::BACKGROUND);
      else if (isMoveDown(0u))
        map.set(static_cast<Point<unsigned int>>(positions.front().get() - Vector{ 0.f, 1.f }), CellType::BACKGROUND);
      else if (isMoveLeft(0u))
        map.set(static_cast<Point<unsigned int>>(positions.front().get() + Vector{ 1.f, 0.f }), CellType::BACKGROUND);
      if (isDead) return;
      prevTailSpeed = speeds.back();
      reactNextKeyCode();
      positions.front().set(positions.front().get() + speeds.front());
    }
  }.setDuration(duration).build());
  map.set(static_cast<Point<unsigned int>>(positions.front().get()), CellType::SNAKE_HEAD);
  setRandomSpeed();
  positions.front().set(positions.front().get() + speeds.front());
}

bool SnakeModel::isMoveUp(const unsigned int i) const {
  return speeds[i].x == 0 && speeds[i].y < 0;
}

bool SnakeModel::isMoveRight(const unsigned int i) const {
  return speeds[i].x > 0 && speeds[i].y == 0;
}

bool SnakeModel::isMoveDown(const unsigned int i) const {
  return speeds[i].x == 0 && speeds[i].y > 0;
}

bool SnakeModel::isMoveLeft(const unsigned int i) const {
  return speeds[i].x < 0 && speeds[i].y == 0;
}

void SnakeModel::followPrevCell(const unsigned int i) {
  if (positions[i].get().y > positions[i - 1].get().y) {
    speeds[i] = Vector{ 0, -1 };
  } else if (positions[i].get().x < positions[i - 1].get().x) {
    speeds[i] = Vector{ 1, 0 };
  } else if (positions[i].get().y < positions[i - 1].get().y) {
    speeds[i] = Vector{ 0, 1 };
  } else if (positions[i].get().x > positions[i - 1].get().x) {
    speeds[i] = Vector{ -1, 0 };
  }
  if (isDead) return;
  positions[i].set(positions[i].get() + speeds[i]);
}

bool SnakeModel::isTailMoveUpPreviously() const {
  return prevTailSpeed.x == 0 && prevTailSpeed.y < 0;
}

void SnakeModel::growWhenTailMoveUpPreviously() {
  const auto i{ static_cast<unsigned int>(positions.size()) };
  if (isMoveRight(i - 1)) {
    positions.push_back(AnimatedAny<Point<float>>::Builder{
      static_cast<Point<float>>(static_cast<Point<int>>(positions.back().get() + Vector{ 0.f, 1.f })), [this, i]() {
        if (isMoveUp(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveRight(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        else if (isMoveDown(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveLeft(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        followPrevCell(i);
      }
    }.setDuration(duration).build());
  } else if (isMoveLeft(i - 1)) {
    positions.push_back(AnimatedAny<Point<float>>::Builder{
      static_cast<Point<float>>(static_cast<Point<int>>(positions.back().get() + Vector{ 1.f, 1.f })), [this, i]() {
        if (isMoveUp(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveRight(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        else if (isMoveDown(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveLeft(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        followPrevCell(i);
      }
    }.setDuration(duration).build());
  } else {
    positions.push_back(AnimatedAny<Point<float>>::Builder{
      static_cast<Point<float>>(static_cast<Point<int>>(positions.back().get() + Vector{ 0.f, 2.f })), [this, i]() {
        if (isMoveUp(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveRight(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        else if (isMoveDown(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveLeft(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        followPrevCell(i);
      }
    }.setDuration(duration).build());
  }
  map.set(static_cast<Point<unsigned int>>(positions.back().get()), CellType::SNAKE_BODY);
  speeds.push_back(Vector{ 0, -1 });
}

bool SnakeModel::isTailMoveRightPreviously() const {
  return prevTailSpeed.x > 0 && prevTailSpeed.y == 0;
}

void SnakeModel::growWhenTailMoveRightPreviously() {
  const auto i{ static_cast<unsigned int>(positions.size()) };
  if (isMoveUp(i - 1)) {
    positions.push_back(AnimatedAny<Point<float>>::Builder{
      static_cast<Point<float>>(static_cast<Point<int>>(positions.back().get() + Vector{ -1.f, 1.f })), [this, i]() {
        if (isMoveUp(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveRight(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        else if (isMoveDown(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveLeft(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        followPrevCell(i);
      }
    }.setDuration(duration).build());
  } else {
    positions.push_back(AnimatedAny<Point<float>>::Builder{
      static_cast<Point<float>>(static_cast<Point<int>>(positions.back().get() - Vector{ 1.f, 0.f })), [this, i]() {
        if (isMoveUp(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveRight(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        else if (isMoveDown(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveLeft(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        followPrevCell(i);
      }
    }.setDuration(duration).build());
  }
  map.set(static_cast<Point<unsigned int>>(positions.back().get()), CellType::SNAKE_BODY);
  speeds.push_back(Vector{ 1, 0 });
}

bool SnakeModel::isTailMoveDownPreviously() const {
  return prevTailSpeed.x == 0 && prevTailSpeed.y > 0;
}

void SnakeModel::growWhenTailMoveDownPreviously() {
  const auto i{ static_cast<unsigned int>(positions.size()) };
  if (isMoveLeft(i - 1)) {
    positions.push_back(AnimatedAny<Point<float>>::Builder{
      static_cast<Point<float>>(static_cast<Point<int>>(positions.back().get() + Vector{ 1.f, -1.f })), [this, i]() {
        if (isMoveUp(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveRight(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        else if (isMoveDown(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveLeft(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        followPrevCell(i);
      }
    }.setDuration(duration).build());
  } else {
    positions.push_back(AnimatedAny<Point<float>>::Builder{
      static_cast<Point<float>>(static_cast<Point<int>>(positions.back().get() - Vector{ 0.f, 1.f })), [this, i]() {
        if (isMoveUp(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveRight(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        else if (isMoveDown(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveLeft(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        followPrevCell(i);
      }
    }.setDuration(duration).build());
  }
  map.set(static_cast<Point<unsigned int>>(positions.back().get()), CellType::SNAKE_BODY);
  speeds.push_back(Vector{ 0, 1 });
}

bool SnakeModel::isTailMoveLeftPreviously() const {
  return prevTailSpeed.x < 0 && prevTailSpeed.y == 0;
}

void SnakeModel::growWhenTailMoveLeftPreviously() {
  const auto i{ static_cast<unsigned int>(positions.size()) };
  if (isMoveUp(i - 1)) {
    positions.push_back(AnimatedAny<Point<float>>::Builder{
      static_cast<Point<float>>(static_cast<Point<int>>(positions.back().get() + Vector{ 1.f, 1.f })), [this, i]() {
        if (isMoveUp(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveRight(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        else if (isMoveDown(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveLeft(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        followPrevCell(i);
      }
    }.setDuration(duration).build());
  } else if (isMoveDown(i - 1)) {
    positions.push_back(AnimatedAny<Point<float>>::Builder{
      static_cast<Point<float>>(static_cast<Point<int>>(positions.back().get() + Vector{ 1.f, 0.f })), [this, i]() {
        if (isMoveUp(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveRight(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        else if (isMoveDown(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveLeft(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        followPrevCell(i);
      }
    }.setDuration(duration).build());
  } else {
    positions.push_back(AnimatedAny<Point<float>>::Builder{
      static_cast<Point<float>>(static_cast<Point<int>>(positions.back().get() + Vector{ 2.f, 0.f })), [this, i]() {
        if (isMoveUp(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveRight(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        else if (isMoveDown(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() - Vector{ 0.f, 1.f }), CellType::BACKGROUND);
        else if (isMoveLeft(i))
          map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 1.f, 0.f }), CellType::BACKGROUND);
        followPrevCell(i);
      }
    }.setDuration(duration).build());
  }
  map.set(static_cast<Point<unsigned int>>(positions.back().get()), CellType::SNAKE_BODY);
  speeds.push_back(Vector{ -1, 0 });
}

void SnakeModel::reactKeyDown(const SDL_Keycode keyCode) {
  keyCodes.push(keyCode);
}

void SnakeModel::step(const unsigned int dt) {
  for (auto& position : positions) position.step(dt);
}

void SnakeModel::checkAndReactHits(const unsigned int dt) {
  if (isEatFood()) reactEatFood(dt);
  else if (isHitSelf() || isHitTopWall() || isHitRightWall() || isHitBottomWall() || isHitLeftWall()) reactHit();
  if (isMoveUp(0u)) {
    map.set(static_cast<Point<unsigned int>>(positions.front().get()), CellType::SNAKE_HEAD);
  } else if (isMoveRight(0u)) {
    map.set(static_cast<Point<unsigned int>>(positions.front().get() + Vector{ .999f, 0.f }), CellType::SNAKE_HEAD);
  } else if (isMoveDown(0u)) {
    map.set(static_cast<Point<unsigned int>>(positions.front().get() + Vector{ 0.f, .999f }), CellType::SNAKE_HEAD);
  } else if (isMoveLeft(0u)) {
    map.set(static_cast<Point<unsigned int>>(positions.front().get()), CellType::SNAKE_HEAD);
  }
  for (auto i{ 1u }; i != positions.size(); ++i) {
    if (isMoveUp(i)) {
      map.set(static_cast<Point<unsigned int>>(positions[i].get()), CellType::SNAKE_BODY);
    } else if (isMoveRight(i)) {
      map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ .999f, 0.f }), CellType::SNAKE_BODY);
    } else if (isMoveDown(i)) {
      map.set(static_cast<Point<unsigned int>>(positions[i].get() + Vector{ 0.f, .999f }), CellType::SNAKE_BODY);
    } else if (isMoveLeft(i)) {
      map.set(static_cast<Point<unsigned int>>(positions[i].get()), CellType::SNAKE_BODY);
    }
  }
}

}  // namespace ii887522::snake
