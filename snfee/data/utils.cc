// This project:
#include <snfee/data/utils.h>

namespace snfee {
  namespace data {

    std::string
    clock_label(const clock_type clk_)
    {
      if (clk_ == CLOCK_40MHz)
        return std::string("40MHz");
      if (clk_ == CLOCK_80MHz)
        return std::string("80MHz");
      if (clk_ == CLOCK_160MHz)
        return std::string("160MHz");
      if (clk_ == CLOCK_2560MHz)
        return std::string("2560MHz");
      return std::string("");
    }

  } // namespace data
} // namespace snfee
