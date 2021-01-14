// Copyright ii887522

#ifndef SNAKE_SRC_MAIN_MODEL_SNAKEMODEL_H_
#define SNAKE_SRC_MAIN_MODEL_SNAKEMODEL_H_

#include <Struct/Point.h>
#include <Struct/Size.h>
#include <Struct/Vector.h>
#include <Any/AnimatedAny.h>
#include <Functions/math_ext.h>
#include <Any/Reactive.h>
#include <Any/Map.h>
#include <SDL.h>
#include <queue>
#include <functional>
#include <random>
#include <vector>
#include "../Any/Enums.h"

using std::queue;
using std::function;
using std::default_random_engine;
using std::uniform_int_distribution;
using std::vector;
using ii887522::viewify::Point;
using ii887522::viewify::Size;
using ii887522::nitro::Vector;
using ii887522::nitro::AnimatedAny;
using ii887522::nitro::Reactive;
using ii887522::viewify::Map;

namespace ii887522::snake {

// Not Thread Safe
class SnakeModel final {
  // remove copy semantics
  SnakeModel(const SnakeModel&) = delete;
  SnakeModel& operator=(const SnakeModel&) = delete;

  // remove move semantics
  SnakeModel(SnakeModel&&) = delete;
  SnakeModel& operator=(SnakeModel&&) = delete;

  enum class Direction : unsigned int {
    UP, RIGHT, DOWN, LEFT, COUNT
  };

  const Size<int> wallSize;
  const int cellSize;
  const unsigned int duration;  // animation duration
  uniform_int_distribution<unsigned int> directions;
  default_random_engine& randomEngine;
  vector<AnimatedAny<Point<float>>> positions;
  vector<Vector<int>> speeds;
  queue<SDL_Keycode> keyCodes;
  bool isDead;
  Map<CellType>& map;
  Reactive<bool>& hasEatFood;
  Vector<int> prevTailSpeed;
  const function<void()> onHit;

  constexpr bool isEatFood() const {
    return isMoveUp(0u) ? map.get(static_cast<Point<unsigned int>>(positions.front().get())) == CellType::FOOD :
      isMoveRight(0u) ? map.get(static_cast<Point<unsigned int>>(positions.front().get() + Vector{ .999f, 0.f })) == CellType::FOOD :
      isMoveDown(0u) ? map.get(static_cast<Point<unsigned int>>(positions.front().get() + Vector{ 0.f, .999f })) == CellType::FOOD :
      isMoveLeft(0u) ? map.get(static_cast<Point<unsigned int>>(positions.front().get())) == CellType::FOOD : false;
  }

  void reactEatFood(const unsigned int dt);
  void grow(const unsigned int dt);
  void fixPositions();
  void stop();
  bool isHitSelf() const;
  bool isHitTopWall() const;
  bool isHitRightWall() const;
  bool isHitBottomWall() const;
  bool isHitLeftWall() const;
  void reactHit();

  constexpr void reactWKey() {
    if (!isDead && speeds.front().y <= 0) speeds.front() = Vector{ 0, -1 };
  }

  constexpr void reactAKey() {
    if (!isDead && speeds.front().x <= 0) speeds.front() = Vector{ -1, 0 };
  }

  constexpr void reactSKey() {
    if (!isDead && speeds.front().y >= 0) speeds.front() = Vector{ 0, 1 };
  }

  constexpr void reactDKey() {
    if (!isDead && speeds.front().x >= 0) speeds.front() = Vector{ 1, 0 };
  }

  void reactNextKeyCode();
  void clearSnakeInMap();
  void setRandomSpeed();
  void reborn();

  // Param i: it must be less than the size of positions and speeds
  bool isMoveUp(const unsigned int i) const;

  // Param i: it must be less than the size of positions and speeds
  bool isMoveRight(const unsigned int i) const;

  // Param i: it must be less than the size of positions and speeds
  bool isMoveDown(const unsigned int i) const;

  // Param i: it must be less than the size of positions and speeds
  bool isMoveLeft(const unsigned int i) const;

  // Param i: it must be greater than 0 and less than the size of positions and speeds
  void followPrevCell(const unsigned int i);

  bool isTailMoveUpPreviously() const;
  void growWhenTailMoveUpPreviously();
  bool isTailMoveRightPreviously() const;
  void growWhenTailMoveRightPreviously();
  bool isTailMoveDownPreviously() const;
  void growWhenTailMoveDownPreviously();
  bool isTailMoveLeftPreviously() const;
  void growWhenTailMoveLeftPreviously();

 public:
  explicit SnakeModel(const Size<int>& wallSize, const int cellSize, default_random_engine&, Map<CellType>&, Reactive<bool>*const hasEatFood,
    const function<void()>& onHit);

  constexpr int getCellSize() const {
    return cellSize;
  }

  constexpr const vector<AnimatedAny<Point<float>>>& getPositions() const {
    return positions;
  }

  constexpr void setDead(const bool value) {
    isDead = value;
    if (!value) reborn();
  }

  void reactKeyDown(const SDL_Keycode);
  void step(const unsigned int dt);
  void checkAndReactHits(const unsigned int dt);
};

}  // namespace ii887522::snake

#endif  // SNAKE_SRC_MAIN_MODEL_SNAKEMODEL_H_
