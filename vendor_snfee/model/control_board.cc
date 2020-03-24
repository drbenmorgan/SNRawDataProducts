// This project:
#include <snfee/model/control_board.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/utils.h>
#include <snfee/model/utils.h>
#include <snfee/model/feb_constants.h>

namespace snfee {
  namespace model {

    // static
    std::string control_board::cb_mode_label(const cb_mode mode_)
    {
      switch(mode_) {
      case MODE_CALORIMETER:
        return subdetector_label(SUBDETECTOR_CALORIMETER);
      case MODE_TRACKER:
        return subdetector_label(SUBDETECTOR_TRACKER);
      case MODE_COMMISSIONING:
        return std::string("commissioning");
      default:
        return none_label();
      }
    }
  
    control_board::control_board()
    {
      return;
    }

    // virtual
    control_board::~control_board()
    {
      return;
    }

    void control_board::_construct_()
    {
      set_model(BOARD_CB);
      add_fpga(FPGA_CONTROL);
      for (int i = 0; i < (int) feb_constants::CB_NUMBER_OF_FE_FPGA; i++) {
        add_fpga(FPGA_FRONTEND, i);
      }
      lock_fpga_layout();
      return;
    }
  
    bool control_board::has_mode() const
    {
      return _mode_ != MODE_UNDEF;
    }
  
    control_board::cb_mode control_board::get_mode() const
    {
      return _mode_;
    }

    void control_board::set_mode(const cb_mode mode_)
    {
      _mode_ = mode_;
      return;
    }

    // virtual
    void control_board::_at_initialize_(const datatools::properties & config_)
    {
      _construct_();
      base_board::_common_initialize(config_);
    
      if (config_.has_key("mode")) {
        std::string mode_label = config_.fetch_string("mode");
        if (mode_label == cb_mode_label(MODE_CALORIMETER)) {
          set_mode(MODE_CALORIMETER);
        } else if (mode_label == cb_mode_label(MODE_TRACKER)) {
          set_mode(MODE_TRACKER);
        } else if (mode_label == cb_mode_label(MODE_COMMISSIONING)) {
          set_mode(MODE_COMMISSIONING);
        } else {
          DT_THROW(std::logic_error,
                   "Invalid control board mode '" << mode_label << "'!");
        }
      }
    
      return;
    }

    // virtual
    void control_board::_at_reset_()
    {
      unlock_fpga_layout();
      _mode_ = MODE_UNDEF;
      base_board::_common_reset();
      return;
    }
 
    // virtual
    void control_board::print_tree(std::ostream & out_,
                                   const boost::property_tree::ptree & options_) const
    {
      {
        boost::property_tree::ptree ioptions = base_print_options::force_inheritance(options_);
        this->base_board::print_tree(out_, ioptions);
      }
      base_print_options popts;
      popts.configure_from(options_);
    
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Mode : '" << cb_mode_label(_mode_) << "'" << std::endl;
    
      return;
    }

  } // namespace model
} // namespace snfee
