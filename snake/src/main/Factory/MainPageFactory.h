// Copyright ii887522

#ifndef SNAKE_SRC_MAIN_FACTORY_MAINPAGEFACTORY_H_
#define SNAKE_SRC_MAIN_FACTORY_MAINPAGEFACTORY_H_

#ifndef TEST

#include <ViewGroup/Page.h>
#include <Struct/Point.h>
#include <Struct/Color.h>
#include <Struct/Paint.h>
#include <Struct/Size.h>
#include <Any/Reactive.h>
#include <Image/Text.h>
#include <View/Button.h>
#include <View/RectView.h>
#include <View/BorderView.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdexcept>
#include "../Any/Enums.h"

using std::runtime_error;
using ii887522::viewify::Page;
using ii887522::viewify::Point;
using ii887522::viewify::Color;
using ii887522::viewify::Paint;
using ii887522::viewify::Size;
using ii887522::nitro::Reactive;
using ii887522::viewify::Text;
using ii887522::viewify::Button;
using ii887522::viewify::RectView;
using ii887522::viewify::BorderView;

namespace ii887522::snake {

// Not Thread Safe: it must only be used in main thread
// See also View.h for more details
template <unsigned int viewCount> class MainPageFactory final {
  // remove copy semantics
  MainPageFactory(const MainPageFactory&) = delete;
  MainPageFactory& operator=(const MainPageFactory&) = delete;

  // remove move semantics
  MainPageFactory(MainPageFactory&&) = delete;
  MainPageFactory& operator=(MainPageFactory&&) = delete;

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
    const Size<int> size;
    Reactive<Path>& currentPath;
    int cellSize;
    bool hasSetCellSize;
    TTF_Font* headFont;
    bool hasSetHeadFont;
    TTF_Font* bodyFont;
    bool hasSetBodyFont;
    unsigned int buttonDuration;  // button animation duration
    bool hasSetButtonDuration;  // has set button animation duration
    SDL_Cursor*const pointer;
    Size<int> buttonPadding;
    bool hasSetButtonPadding;

   public:
    // Param renderer: it must not be assigned to integer
    // Param pointer: it must not be assigned to integer
    explicit constexpr Builder(SDL_Renderer*const renderer, const Size<int>& size, Reactive<Path>*const currentPath,
      SDL_Cursor*const pointer) : renderer{ renderer }, size{ size }, currentPath{ *currentPath }, cellSize{ 0 },
      hasSetCellSize{ false }, headFont{ nullptr }, hasSetHeadFont{ false }, bodyFont{ nullptr }, hasSetBodyFont{ false },
      buttonDuration{ 0u }, hasSetButtonDuration{ false }, pointer{ pointer }, hasSetButtonPadding{ false } { }

    // Must Call Time(s): At least 1
    constexpr Builder& setCellSize(const int value) {
      cellSize = value;
      hasSetCellSize = true;
      return *this;
    }

    // Param value: it must not be assigned to integer
    // Must Call Time(s): At least 1
    constexpr Builder& setHeadFont(TTF_Font*const value) {
      headFont = value;
      hasSetHeadFont = true;
      return *this;
    }

    // Param value: it must not be assigned to integer
    // Must Call Time(s): At least 1
    constexpr Builder& setBodyFont(TTF_Font*const value) {
      bodyFont = value;
      hasSetBodyFont = true;
      return *this;
    }

    // Button animation duration
    // Must Call Time(s): At least 1
    constexpr Builder& setButtonDuration(const unsigned int value) {
      buttonDuration = value;
      hasSetButtonDuration = true;
      return *this;
    }

    // Must Call Time(s): At least 1
    constexpr Builder& setButtonPadding(const Size<int>& value) {
      buttonPadding = value;
      hasSetButtonPadding = true;
      return *this;
    }

    MainPageFactory* build() {
      if (!hasSetCellSize) throw runtime_error{ "MainPageFactory cellSize is required!" };
      if (!hasSetHeadFont) throw runtime_error{ "MainPageFactory headFont is required!" };
      if (!hasSetBodyFont) throw runtime_error{ "MainPageFactory bodyFont is required!" };
      if (!hasSetButtonDuration) throw runtime_error{ "MainPageFactory buttonDuration is required!" };
      if (!hasSetButtonPadding) throw runtime_error{ "MainPageFactory buttonPadding is required!" };
      return new MainPageFactory{ *this };
    }

    friend class MainPageFactory;
  };

 private:
  SDL_Renderer*const renderer;
  const Size<int> size;
  Reactive<Path>& currentPath;
  const int cellSize;
  TTF_Font*const headFont;
  TTF_Font*const bodyFont;
  const unsigned int buttonDuration;  // button animation duration
  SDL_Cursor*const pointer;
  const Size<int> buttonPadding;

  explicit constexpr MainPageFactory(const Builder& builder) : renderer{ builder.renderer }, size{ builder.size },
    currentPath{ builder.currentPath }, cellSize{ builder.cellSize }, headFont{ builder.headFont }, bodyFont{ builder.bodyFont },
    buttonDuration{ builder.buttonDuration }, pointer{ builder.pointer }, buttonPadding{ builder.buttonPadding } { }

 public:
  Page<Path, viewCount>* make() {
    constexpr Point playButtonPosition{ 324, 592 };
    constexpr Size keySize{ 62, 62 };
    constexpr Size keyMargin{ 16, 16 };
    constexpr Point wKeyPosition{ 272, 208 };
    constexpr Point aKeyPosition{ wKeyPosition.x, wKeyPosition.y + keySize.h + keyMargin.h };
    constexpr Point sKeyPosition{ aKeyPosition.x, aKeyPosition.y + keySize.h + keyMargin.h };
    constexpr Point dKeyPosition{ sKeyPosition.x, sKeyPosition.y + keySize.h + keyMargin.h };
    constexpr Size wKeyPadding{ 16, 14 };
    constexpr Size aKeyPadding{ 20, 14 };
    constexpr Size sKeyPadding{ 20, 14 };
    constexpr Size dKeyPadding{ 20, 14 };
    return new Page<Path, viewCount>{ renderer, Point{ 0, 0 }, Path::MAIN, &currentPath, {
      Text::Builder{ renderer, headFont, Point{ 288, 80 }, "Snake", Color{ 0u, 255u, 0u } }.setDuration(1u).build(),
      new RectView{ renderer, wKeyPosition, Paint{ keySize, Color{ 255u, 255u, 255u } } },
      Text::Builder{ renderer, bodyFont, wKeyPosition + wKeyPadding, "W" }.setDuration(1u).build(),
      Text::Builder{
        renderer, bodyFont, Point{ wKeyPosition.x + keySize.w + keyMargin.w, wKeyPosition.y + wKeyPadding.h }, "Move up",
        Color{ 255u, 255u, 255u }
      }
        .setDuration(1u)
        .build(),
      new RectView{ renderer, aKeyPosition, Paint{ keySize, Color{ 255u, 255u, 255u } } },
      Text::Builder{ renderer, bodyFont, aKeyPosition + aKeyPadding, "A" }.setDuration(1u).build(),
      Text::Builder{
        renderer, bodyFont, Point{ aKeyPosition.x + keySize.w + keyMargin.w, aKeyPosition.y + aKeyPadding.h }, "Move left",
        Color{ 255u, 255u, 255u }
      }
        .setDuration(1u)
        .build(),
      new RectView{ renderer, sKeyPosition, Paint{ keySize, Color{ 255u, 255u, 255u } } },
      Text::Builder{ renderer, bodyFont, sKeyPosition + sKeyPadding, "S" }.setDuration(1u).build(),
      Text::Builder{
        renderer, bodyFont, Point{ sKeyPosition.x + keySize.w + keyMargin.w, sKeyPosition.y + sKeyPadding.h }, "Move down",
        Color{ 255u, 255u, 255u }
      }
        .setDuration(1u)
        .build(),
      new RectView{ renderer, dKeyPosition, Paint{ keySize, Color{ 255u, 255u, 255u } } },
      Text::Builder{ renderer, bodyFont, dKeyPosition + dKeyPadding, "D" }.setDuration(1u).build(),
      Text::Builder{
        renderer, bodyFont, Point{ dKeyPosition.x + keySize.w + keyMargin.w, dKeyPosition.y + dKeyPadding.h }, "Move right",
        Color{ 255u, 255u, 255u }
      }
        .setDuration(1u)
        .build(),
      Button::Builder{ renderer, playButtonPosition, Paint{ Size{ 102, 64 }, Color{ 0u, 0u, 255u } } }
        .setADuration(1u)
        .setLightnessDuration(buttonDuration)
        .setOnMouseMove([this]() {
          SDL_SetCursor(pointer);
        })
        .setOnMouseOver([]() { })
        .setOnMouseOut([]() {
          SDL_SetCursor(SDL_GetDefaultCursor());
        })
        .setOnClick([this]() {
          currentPath.set(Path::GAME);
          SDL_SetCursor(SDL_GetDefaultCursor());
        })
        .build(),
      Text::Builder{ renderer, bodyFont, playButtonPosition + buttonPadding, "Play", Color{ 255u, 255u, 255u } }
        .setDuration(1u)
        .build()
    } };
  }
};

}  // namespace ii887522::snake

#endif
#endif  // SNAKE_SRC_MAIN_FACTORY_MAINPAGEFACTORY_H_
