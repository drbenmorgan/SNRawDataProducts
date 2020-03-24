// This project:
#include <snfee/model/board_builder.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/model/control_board.h>
#include <snfee/model/trigger_board.h>
#include <snfee/model/calorimeter_fe_board.h>
#include <snfee/model/tracker_fe_board.h>

namespace snfee {
  namespace model {

    board_builder::board_builder(const fpga_builder & pgab_)
      : _pgab_(pgab_)
    {
      return;
    }

    // virtual
    board_builder::~board_builder()
    {
      return;
    }

    bool board_builder::is_initialized() const
    {
      return _initialized_;
    }
    
    void board_builder::initialize(const datatools::properties & /* config_ */)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Board builder is already initialized!");

      // Processing configuration parameters:


      // Initialization ops:
      if (_supported_board_models_.size() == 0) {
        // Default to full list of SuperNEMO boards:
        add_supported_board_model(BOARD_CB);
        add_supported_board_model(BOARD_CFEB);
        add_supported_board_model(BOARD_TFEB);
        add_supported_board_model(BOARD_TB);
      }
    
      _initialized_ = true;
      return;
    }
   
    void board_builder::initialize_simple()
    {
      datatools::properties config;
      initialize(config);
      return;
    }

    void board_builder::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Board builder is not initialized!");
      _initialized_ = false;
      _supported_board_models_.clear();
      return;
    }
 
    bool board_builder::is_board_model_supported(const board_model_type model_) const
    {
      return _supported_board_models_.count(model_) > 0;
    }

    void board_builder::add_supported_board_model(const board_model_type model_)
    {
      _supported_board_models_.insert(model_);
      return;
    }
 
    const std::set<board_model_type> & board_builder::get_supported_board_models() const
    {
      return _supported_board_models_;
    }

    board_ptr board_builder::make_board(const board_model_type board_model_,
                                        const datatools::properties & board_config_) const
    {
      DT_LOG_TRACE_ENTERING(logging);
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Board builder is not initialized!");
      DT_THROW_IF(!is_board_model_supported(board_model_),
                  std::logic_error,
                  "Board model '" << board_model_label(board_model_) << "' is not supported!");
      std::shared_ptr<base_board> new_board;

      if (board_model_ == BOARD_TB) {
        new_board.reset(new trigger_board);
        new_board->initialize(board_config_);
      }
 
      if (board_model_ == BOARD_CB) {
        new_board.reset(new control_board);
        new_board->initialize(board_config_);
      }

      if (board_model_ == BOARD_CFEB) {
        new_board.reset(new calorimeter_fe_board);
        new_board->initialize(board_config_);
      }
 
      if (board_model_ == BOARD_TFEB) {
        new_board.reset(new tracker_fe_board);
        new_board->initialize(board_config_);
      }

      DT_LOG_TRACE_EXITING(logging);
      return new_board;
    }

    // virtual
    void board_builder::print_tree(std::ostream & out_,
                                   const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag
           << "Supported board models: " << _supported_board_models_.size() << std::endl;
      std::size_t bt_counter = 0;
      for (std::set<board_model_type>::const_iterator i = _supported_board_models_.begin();
           i != _supported_board_models_.end();
           i++) {
        out_ << popts.indent << skip_tag;
        std::ostringstream bt_indent_ss;
        if (++bt_counter == _supported_board_models_.size()) {
          out_ << last_tag;
        } else {
          out_ << tag;
        }
        out_ << "Board model: '" << board_model_label(*i) << "'" << std::endl;
      }
   
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Initialized : "
           << std::boolalpha << _initialized_ << std::endl;
 
      return;
    }
  
  } // namespace model
} // namespace snfee
