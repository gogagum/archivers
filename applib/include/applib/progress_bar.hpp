#ifndef PROGRESS_BAR_HPP
#define PROGRESS_BAR_HPP

#include <indicators/progress_bar.hpp>
#include <ostream>
#include <string>

////////////////////////////////////////////////////////////////////////////////
/// \brief The ProgressBar class 
class ProgressBar : public indicators::ProgressBar {
 public:
  /**
   * @brief Simplified constructor from exact parameters.
   * 
   * @param width bar width.
   * @param maxProgress max bar progress.
   * @param str message string.
   * @param stream output stream.
   */
  ProgressBar(std::size_t width, std::size_t maxProgress, std::string str,
              std::ostream& stream)
      : indicators::ProgressBar(
            indicators::option::BarWidth{width},
            indicators::option::MaxProgress{maxProgress},
            indicators::option::ShowPercentage{true},
            indicators::option::PostfixText{str},
            indicators::option::Stream{stream}) {}
  
  /**
   * @brief Get the Tick function.
   * 
   * @return callable with no arguments, that ticks.
   */
  auto getTick() {
    return [this]() {
      static_cast<indicators::ProgressBar*>(this)->tick();
    };
  }
};

#endif  // PROGRESS_BAR_HPP
