// This project:
#include <snfee/model/base_board.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/model/utils.h>
#include <snfee/model/firmware_builder.h>

namespace snfee {
  namespace model {
  
    // static
    std::string base_board::build_board_name(const board_model_type model_,
                                             const int id_)
    {
      DT_THROW_IF(model_ == BOARD_UNDEF,
                  std::logic_error,
                  "Invalid board type!");
      DT_THROW_IF(model_ == BOARD_TB && id_ >= 0,
                  std::logic_error,
                  "Invalid TB board ID=" << id_ << "!");
      DT_THROW_IF(model_ == BOARD_CB && id_ >= 0,
                  std::logic_error,
                  "Invalid CB board ID=" << id_ << "!");
      std::ostringstream namess;
      namess << board_model_label(model_);
      if (id_ >= 0) {
        namess << '_' << id_;
      }
      return namess.str();
    }

    base_board::base_board()
    {
      return;
    }

    // virtual
    base_board::~base_board()
    {
      return;
    }

    bool base_board::has_name() const
    {
      return !_name_.empty();
    }
   
    void base_board::set_name(const std::string & name_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Board is initialized and locked!");
      _name_ = name_;
      return;
    }

    const std::string & base_board::get_name() const
    {
      return _name_;
    }

    bool base_board::has_model() const
    {
      return _model_ != BOARD_UNDEF;
    }
  
    void base_board::set_model(const board_model_type model_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Board is initialized and locked!");
      _model_ = model_;
      return;
    }

    board_model_type base_board::get_model() const
    {
      return _model_;
    }

    void base_board::remove_fpga(const std::string & name_)
    {
      DT_THROW_IF(is_fpga_layout_locked(),
                  std::logic_error,
                  "FPGA layout is locked!");
      DT_THROW_IF(!has_fpga(name_),
                  std::logic_error,
                  "No FPGA named '" << name_ << "' in the board!");
      _fpgas_.erase(name_);
      return;
    }

    void base_board::remove_fpga(const fpga_model_type model_, const int id_)
    {
      std::string fpgaName = fpga::build_fpga_name(model_, id_);
      remove_fpga(fpgaName);
      return;
    }

    void base_board::add_fpga(const fpga & pga_)
    {
      DT_THROW_IF(is_fpga_layout_locked(),
                  std::logic_error,
                  "FPGA layout is locked!");
      DT_THROW_IF(! pga_.is_initialized(),
                  std::logic_error,
                  "Base board '" << get_name() << "' reject not initialized FPGA!");
      _fpgas_[pga_.get_name()] = pga_;
      return;
    }

    void base_board::add_fpga(const fpga_model_type model_, const int id_)
    {
      DT_THROW_IF(is_fpga_layout_locked(),
                  std::logic_error,
                  "FPGA layout is locked!");
      std::string fpgaName = fpga::build_fpga_name(model_, id_);
      DT_THROW_IF(_fpgas_.count(fpgaName),
                  std::logic_error,
                  "Front-end setup '" << get_name()
                  << "' already has a FPGA named '" << fpgaName << "'!");
      {
        fpga dummyFpga;
        _fpgas_[fpgaName] = dummyFpga;
      }
      fpga & newFpga = _fpgas_.find(fpgaName)->second;
      newFpga.set_name(fpgaName);
      newFpga.set_model(model_);
      if (id_ >= 0) {
        newFpga.set_id(id_);
      }
      newFpga.initialize_simple();
      return;
    }

    void base_board::flash_fpga(const firmware_builder & fwb_,
                                const std::string & fwid_,
                                const std::string & name_)
    {
      std::map<std::string, fpga>::iterator found = _fpgas_.find(name_);
      DT_THROW_IF(found == _fpgas_.end(),
                  std::logic_error,
                  "No FPGA with name '" << name_ << "'!"); 
      fpga & pga = found->second;
      pga.flash(fwb_,fwid_);
      return;
    }

    void base_board::flash_fpga(const firmware_builder & fwb_,
                                const std::string & fwid_,
                                const fpga_model_type model_,
                                const int id_)
    {
      std::string fpgaName = fpga::build_fpga_name(model_, id_);
      flash_fpga(fwb_, fwid_, fpgaName);
      return;
    }
 
    bool base_board::has_fpga(const std::string & name_) const
    {
      return _fpgas_.count(name_);
    }

    bool base_board::has_fpga(const fpga_model_type model_,
                              const int id_) const
    {
      std::string fpgaName = fpga::build_fpga_name(model_, id_);
      return _fpgas_.count(fpgaName);
    }

    const fpga & base_board::get_fpga(const std::string & name_) const
    {
      std::map<std::string, fpga>::const_iterator found = _fpgas_.find(name_);
      DT_THROW_IF(found == _fpgas_.end(),
                  std::logic_error,
                  "No FPGA with name '" << name_ << "'!");
      return found->second;
    }
 
    const fpga & base_board::get_fpga(const fpga_model_type model_,
                                      const int id_) const
    {
      std::string fpgaName = fpga::build_fpga_name(model_, id_);
      return get_fpga(fpgaName);
    }

    std::set<std::string> base_board::get_fpga_names() const
    {
      std::set<std::string> nms;
      for (const auto & p : _fpgas_) {
        nms.insert(p.first);
      }
      return nms;
    }

    void base_board::lock_all_fpgas_code()
    {
      for (auto & p : _fpgas_) {
        p.second.lock_code();
      }
      return;
    }

    void base_board::unlock_all_fpgas_code()
    {
      for (auto & p : _fpgas_) {
        p.second.unlock_code();
      }
      return;
    }
  
    bool base_board::all_fpgas_have_code() const
    {
      for (const auto & p : _fpgas_) {
        if (!p.second.has_code()) return false;
      }
      return true;
    }
  
    bool base_board::all_fpgas_are_locked() const
    {
      for (const auto & p : _fpgas_) {
        if (!p.second.is_code_locked()) return false;
      }
      return true;
    }

    bool base_board::has_id() const
    {
      return _id_ >= 0;
    }
    
    void base_board::set_id(const int id_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Board is initialized and locked!");
      _id_ = id_ < 0 ? -1 : id_;
      return;
    }

    int base_board::get_id() const
    {
      return _id_;
    }

    bool base_board::has_slot() const
    {
      return _slot_ >= 0;
    }
    
    void base_board::set_slot(const int slot_)
    {
      // DT_THROW_IF(is_initialized(), std::logic_error,
      //             "Board is initialized and locked!");
      _slot_ = (slot_ < 0) ? -1 : slot_;
      return;
    }

    int base_board::get_slot() const
    {
      return _slot_;
    }

    bool base_board::has_address() const
    {
      return !_address_.empty();
    }
   
    void base_board::set_address(const std::string & address_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Board is initialized and locked!");
      _address_ = address_;
      return;
    }

    const std::string & base_board::get_address() const
    {
      return _address_;
    }

    bool base_board::is_initialized() const
    {
      return _initialized_;
    }

    void base_board::initialize_simple()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }
    
    void base_board::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Board is already initialized!");
    
      _at_initialize_(config_);    

      // Final check:
      DT_THROW_IF(!has_model(), std::logic_error,
                  "No board model is defined!");

      if (!has_name()) {
        set_name(build_board_name(_model_, _id_));
      }
   
      if (config_.has_key("fpga_layout.lock")) {
        bool fl_lock = config_.fetch_boolean("fpga_layout.lock");
        if (fl_lock) {
          lock_fpga_layout();
        }
      }
    
      _initialized_ = true;
      return;
    }

    void base_board::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Board is not initialized!");
      _initialized_ = false;
      _at_reset_();
      return;
    }

    bool base_board::is_fpga_layout_locked() const
    {
      return _fpga_layout_locked_;
    }

    void base_board::lock_fpga_layout()
    {
      _fpga_layout_locked_ = true;
      return;
    }

    void base_board::unlock_fpga_layout()
    {
      _fpga_layout_locked_ = false;
      return;
    }

    void base_board::_common_initialize(const datatools::properties & config_)
    {
      if (!has_name()) {
        if (config_.has_key("name")) {
          set_name(config_.fetch_string("name"));
        }
      }
      if (!has_model()) {
        if (config_.has_key("model")) {
          set_model(board_model_from(config_.fetch_string("model")));
        }
      }
      if (!has_id()) {
        if (config_.has_key("id")) {
          int id = config_.fetch_positive_integer("id");
          set_id(id);
        }
      }
      if (!has_address()) {
        if (config_.has_key("address")) {
          set_address(config_.fetch_string("address"));
        }
      }
      if (!has_slot()) {
        if (config_.has_key("slot")) {
          set_slot(config_.fetch_positive_integer("slot"));
        }
      }
      return;
    }

    // virtual
    void base_board::_at_initialize_(const datatools::properties & config_)
    {
      _common_initialize(config_);
      return;
    }

    void base_board::_common_reset()
    {
      _name_.clear();
      _model_ = BOARD_UNDEF;
      _id_ = -1;
      _address_.clear();
      _fpgas_.clear();
      _slot_ = -1;
      return;
    }
                
    // virtual
    void base_board::_at_reset_()
    {
      unlock_fpga_layout();
      _common_reset();
      return;
    }
 
    // virtual
    void base_board::print_tree(std::ostream & out_,
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
           << "Model : '" << board_model_label(_model_) << "'" << std::endl;
   
      out_ << popts.indent << tag
           << "ID : ";
      if (has_id()) {
        out_ << _id_;
      } else {
        out_ << none_label();
      }
      out_ << std::endl;
    
      out_ << popts.indent << tag
           << "FPGAs : " << _fpgas_.size() << "" << std::endl;

      std::size_t fpga_counter = 0;
      for (std::map<std::string, fpga>::const_iterator i = _fpgas_.begin();
           i != _fpgas_.end();
           i++) {
        out_ << popts.indent << skip_tag;
        std::ostringstream fpga_indent_ss;
        fpga_indent_ss << popts.indent << skip_tag;
        if (++fpga_counter == _fpgas_.size()) {
          out_ << last_tag;
          fpga_indent_ss << last_skip_tag;
        } else {
          out_ << tag;
          fpga_indent_ss << skip_tag;
        }
        out_ << "FPGA: " << i->first << std::endl;
        boost::property_tree::ptree options2;
        {
          options2.put("indent", fpga_indent_ss.str());
        }
        i->second.print_tree(out_, options2);
      }

      if (_fpgas_.size()) {
        out_ << popts.indent << tag
             << "FPGA layout lock : "
             << std::boolalpha << is_fpga_layout_locked() << std::endl;
      }
    
      out_ << popts.indent << tag
           << "Slot : ";
      if (has_slot()) {
        out_ << _slot_;
      } else {
        out_ << none_label();
      }
      out_ << std::endl;
   
      out_ << popts.indent << tag
           << "Address : ";
      if (has_address()) {
        out_ << _address_;
      } else {
        out_ << none_label();
      }
      out_ << std::endl;
    
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Initialized : " << std::boolalpha << is_initialized() << std::endl;
   
      return;
    }

  } // namespace model
} // namespace snfee
