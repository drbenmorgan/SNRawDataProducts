// This project:
#include <snfee/model/crate.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/model/utils.h>
#include <snfee/model/control_board.h>

namespace snfee {
  namespace model {

    // static
    std::string crate::build_crate_name(const crate_model_type model_,
                                        const int id_)
    {
      std::ostringstream namess;
      switch (model_) {
      case CRATE_CALORIMETER:
        namess << "CaloCrate";
        break;
      case CRATE_TRACKER:
        namess << "TrackerCrate";
        break;
      case CRATE_COMMISSIONING:
        namess << "TestCrate";
        break;
      default:
        DT_THROW(std::logic_error, "Invalid crate model!");
      }
      namess << '_' << id_;
      return namess.str();
    }

    crate::crate()
    {
      return;
    }

    // virtual
    crate::~crate()
    {
      return;
    }

    bool crate::has_name() const
    {
      return !_name_.empty();
    }
   
    void crate::set_name(const std::string & name_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Crate is initialized and locked!");
      _name_ = name_;
      return;
    }

    const std::string & crate::get_name() const
    {
      return _name_;
    }

    bool crate::has_model() const
    {
      return _model_ != CRATE_UNDEF;
    }
 
    void crate::set_model(const crate_model_type model_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Crate is initialized and locked!");
      _model_ = model_;
      return;
    }

    crate_model_type crate::get_model() const
    {
      return _model_;
    }
 
    bool crate::is_calorimeter_model() const
    {
      return _model_ == CRATE_CALORIMETER;
    }
  
    bool crate::is_tracker_model() const
    {
      return _model_ == CRATE_TRACKER;
    }
                  
    bool crate::is_commissioning_model() const
    {
      return _model_ == CRATE_COMMISSIONING;
    }

    bool crate::has_id() const
    {
      return _id_ >= 0;
    }
    
    void crate::set_id(const int id_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Crate is initialized and locked!");
      DT_THROW_IF(id_ > 2, std::range_error,
                  "Invalid ID=" << id_ << "!");
      _id_ = id_ < 0 ? -1 : id_;
      return;
    }

    int crate::get_id() const
    {
      return _id_;
    }

    bool crate::has_board_at_slot(const int slot_) const
    {
      return _boards_.count(slot_);
    }

    bool crate::is_board_layout_locked() const
    {
      return _board_layout_locked_;
    }

    void crate::lock_board_layout()
    {
      _board_layout_locked_ = true;
      return;
    }

    void crate::unlock_board_layout()
    {
      _board_layout_locked_ = false;
      return;
    }
  
    void crate::add_board(const board_ptr & board_, const int slot_)
    {
      DT_THROW_IF(is_board_layout_locked(), std::logic_error,
                  "Board layout is locked!");
      DT_THROW_IF(slot_ < 0 || slot_ > MAX_SLOT,
                  std::range_error,
                  "Invalid slot " << slot_ << "!");
      DT_THROW_IF(has_board_at_slot(slot_),
                  std::range_error,
                  "Slot " << slot_ << " is already used!");
    
      if (typeid(*board_.get()) == typeid(control_board) && slot_ != CONTROL_BOARD_SLOT) {
        DT_THROW(std::logic_error,
                 "Control board can only be plugged at slot " << CONTROL_BOARD_SLOT << "!");
      }
      if (slot_ == CONTROL_BOARD_SLOT && typeid(*board_.get()) != typeid(control_board)) {
        DT_THROW(std::logic_error,
                 "Only control board can be plugged at slot " << CONTROL_BOARD_SLOT << "!");
      }
      _boards_[slot_] = board_;
      return;
    }

    void crate::remove_board(const int slot_)
    {
      DT_THROW_IF(is_board_layout_locked(), std::logic_error,
                  "Board layour is locked!");
      _boards_.erase(slot_);
      return;
    }
  
    const base_board & crate::get_board(const std::string & name_) const
    {
      for (const auto & p : _boards_) {
        const std::string & bname = p.second->get_name();
        if (bname == name_) {
          return *p.second.get();
        }
      }
      DT_THROW(std::logic_error, "No board with name '" << name_ << "'!");
    }
 
    base_board & crate::grab_board(const std::string & name_)
    {
      for (auto & p : _boards_) {
        const std::string & bname = p.second->get_name();
        if (bname == name_) {
          return *p.second.get();
        }
      }
      DT_THROW(std::logic_error, "No board with name '" << name_ << "'!");
    }

    bool crate::has_board(const std::string & name_) const
    {
      for (const auto & p : _boards_) {
        const std::string & bname = p.second->get_name();
        if (bname == name_) {
          return true;
        }
      }
      return false;
    }

    const base_board & crate::get_board(const int slot_) const
    {
      std::map<int, board_ptr>::const_iterator found = _boards_.find(slot_);
      DT_THROW_IF(found != _boards_.end(),
                  std::logic_error,
                  "No board at slot " << slot_ << "'!");
      return *found->second.get();
    }

    base_board & crate::grab_board(const int slot_)
    {
      std::map<int, board_ptr>::iterator found = _boards_.find(slot_);
      DT_THROW_IF(found != _boards_.end(),
                  std::logic_error,
                  "No board at slot " << slot_ << "'!");
      return *found->second.get();
    }

    std::set<std::string> crate::get_board_names() const
    {
      std::set<std::string> names;
      for (const auto & p : _boards_) {
        const std::string & bname = p.second->get_name();
        names.insert(bname);
      }
      return names;
    }

    void crate::flash_fpga(const firmware_builder & fwb_,
                           const std::string & fwid_,
                           const std::string & board_name_,
                           const std::string & fpga_name_)
    {
      base_board & board = grab_board(board_name_);
      board.flash_fpga(fwb_, fwid_, fpga_name_);
      return;
    }
 
    void crate::flash_fpga(const firmware_builder & fwb_,
                           const std::string & fwid_,
                           const int slot_,
                           const std::string & fpga_name_)
    {
      base_board & board = grab_board(slot_);
      board.flash_fpga(fwb_, fwid_, fpga_name_);
      return;
    }

    bool crate::is_initialized() const
    {
      return _initialized_;
    }
    
    void crate::initialize(const datatools::properties & config_,
                           const board_builder * bb_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Crate is already initialized!");

      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      // logging = datatools::logger::PRIO_DEBUG;
    
      if (!has_name()) {
        if (config_.has_key("name")) {
          std::string name = config_.fetch_string("name");
          set_name(name);
        }
      }
    
      if (!has_model()) {
        if (config_.has_key("model")) {
          std::string model_label = config_.fetch_string("model");
          if (model_label == crate_model_label(CRATE_CALORIMETER)) {
            set_model(CRATE_CALORIMETER);
          } else if (model_label == crate_model_label(CRATE_TRACKER)) {
            set_model(CRATE_TRACKER);
          } else if (model_label == crate_model_label(CRATE_COMMISSIONING)) {
            set_model(CRATE_COMMISSIONING);
          } else {
            DT_THROW(std::logic_error,
                     "Invalid model label '" << model_label << "'!");
          }
        }
      }

      if (!has_id()) {
        if (config_.has_key("id")) {
          int id = config_.fetch_positive_integer("id");
          set_id(id);
        }
      }
    
      if (!has_name()) {
        set_name(build_crate_name(_model_, _id_));
      }
 
      if (bb_ != nullptr) {
        for (int islot = 0; islot <= MAX_SLOT; islot++) {
          board_model_type bmodel = BOARD_UNDEF;
          DT_LOG_DEBUG(logging, "Parsing board infos for slot #" << islot);
          std::ostringstream board_slot_model_key;
          board_slot_model_key << "board." << islot << ".model";
          if (config_.has_key(board_slot_model_key.str())) {
            std::string board_model_repr = config_.fetch_string(board_slot_model_key.str());
            DT_LOG_DEBUG(logging, "Board at slot #" << islot << " is a '" << board_model_repr << "'");
            bmodel = board_model_from(board_model_repr);
            DT_LOG_DEBUG(logging, "TEST-1");
            DT_THROW_IF(bmodel == BOARD_UNDEF, std::logic_error,
                        "Invalid board model '" << board_model_repr << "'!");
            datatools::properties board_config;
            std::ostringstream board_slot_config_path_key;
            board_slot_config_path_key << "board." << islot << ".config_path";
            DT_LOG_DEBUG(logging, "TEST-2");
            if (config_.has_key(board_slot_config_path_key.str())) {
              DT_LOG_DEBUG(logging, "TEST-3");
              std::string board_slot_config_path = config_.fetch_string(board_slot_config_path_key.str());
              datatools::fetch_path_with_env(board_slot_config_path);
              board_config.read_configuration(board_slot_config_path);
            } else {
              DT_LOG_DEBUG(logging, "TEST-4");
              std::ostringstream board_slot_config_key;
              board_slot_config_key << "board." << islot << ".config.";
              std::string board_config_prefix = board_slot_config_key.str();
              config_.export_and_rename_starting_with(board_config,
                                                      board_config_prefix,
                                                      "");
              DT_LOG_DEBUG(logging, "TEST-5");
            }
            DT_LOG_DEBUG(logging, "TEST-6");
            board_ptr new_board = bb_->make_board(bmodel, board_config);
            new_board->set_slot(islot);
            DT_LOG_DEBUG(logging, "TEST-7");
            add_board(new_board, islot);
          } else {
            DT_LOG_DEBUG(logging, "No board at slot #" << islot);
          }
        }
      }

      if (_boards_.size()) {
        if (config_.has_key("board_layout.lock")) {
          bool bl_lock = config_.fetch_boolean("board_layout.lock");
          if (bl_lock) {
            lock_board_layout();
          }   
        }
      }
    
      _initialized_ = true;
      return;
    }

    void crate::initialize_simple()
    {
      datatools::properties config;
      initialize(config);
      return;
    }

    void crate::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Crate is not initialized!");
      _initialized_ = false;
      _name_.clear();
      _model_ = CRATE_UNDEF;
      _id_ = -1;
      _board_layout_locked_ = false;
      _boards_.clear();
      return;
    }

    // virtual
    void crate::print_tree(std::ostream & out_,
                           const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }
    
      out_ << popts.indent << tag
           << "Name : '" << _name_ << "'" << std::endl;

      out_ << popts.indent << tag
           << "Model : '" << crate_model_label(_model_) << "'" << std::endl;

      out_ << popts.indent << tag
           << "ID : ";
      if (has_id()) {
        out_ << _id_;
      } else {
        out_ << none_label();
      }
      out_ << std::endl;

      out_ << popts.indent << tag
           << "Boards : " << _boards_.size() << std::endl;
 
      std::size_t board_counter = 0;
      for (std::map<int, board_ptr>::const_iterator i = _boards_.begin();
           i != _boards_.end();
           i++) {
        out_ << popts.indent << skip_tag;
        std::ostringstream board_indent_ss;
        board_indent_ss << popts.indent << skip_tag;
        if (++board_counter == _boards_.size()) {
          out_ << last_tag;
          board_indent_ss << last_skip_tag;
        } else {
          out_ << tag;
          board_indent_ss << skip_tag;
        }
        out_ << "Board at slot: " << i->first << std::endl;
        boost::property_tree::ptree options2;
        {
          options2.put("indent", board_indent_ss.str());
        }
        i->second->print_tree(out_, options2);
      }

      if (_boards_.size()) {
        out_ << popts.indent << tag
             << "Board layout lock : " << std::boolalpha << _board_layout_locked_ << std::endl;
      }
 
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Initialized : " << std::boolalpha << _initialized_ << std::endl;
    
      return;
    }

  } // namespace model
} // namespace snfee
