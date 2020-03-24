// Ourselves:
#include <snfee/model/fpga.h>

// This project:
#include <snfee/utils.h>
#include <snfee/model/utils.h>
#include <snfee/model/firmware_builder.h>

namespace snfee {
  namespace model {

    // static
    std::string fpga::build_fpga_name(const fpga_model_type model_,
                                      const int id_)
    {
      DT_THROW_IF(model_ == FPGA_CONTROL && id_ >= 0,
                  std::logic_error,
                  "Invalid control FPGA ID=" << id_ << "!");
      DT_THROW_IF(model_ == FPGA_FRONTEND && id_ < 0,
                  std::logic_error,
                  "Invalid front-end FPGA ID=" << id_ << "!");
      std::ostringstream namess;
      switch (model_) {
      case FPGA_CONTROL:
        namess << fpga_model_label(model_) << "FPGA";
        break;
      case FPGA_FRONTEND:
        namess << fpga_model_label(model_) << "FPGA";
        if (id_ >= 0) namess << '_' << id_;
        break;
      case FPGA_GENERIC:
        namess << fpga_model_label(model_) << "FPGA";
        if (id_ >= 0) namess << '_' << id_;
        break;
      default:
        DT_THROW(std::logic_error, "Invalid FPGA model!");
      }
      return namess.str();
    }

    fpga::fpga()
    {
      return;
    }
 
    // virtual
    fpga::~fpga()
    {
      return;
    }

    bool fpga::has_name() const
    {
      return !_name_.empty();
    }
   
    void fpga::set_name(const std::string & name_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "FPGA is already initialized!");
      _name_ = name_;
      return;
    }

    const std::string & fpga::get_name() const
    {
      return _name_;
    }

    bool fpga::has_model() const
    {
      return _model_ != FPGA_UNDEF;
    }
 
    void fpga::set_model(const fpga_model_type model_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "FPGA is already initialized!");
      _model_ = model_;
      return;
    }
  
    bool fpga::is_control_model() const
    {
      return _model_ == FPGA_CONTROL;
    }
  
    bool fpga::is_frontend_model() const
    {
      return _model_ == FPGA_FRONTEND;
    }
  
    bool fpga::is_generic_model() const
    {
      return _model_ == FPGA_GENERIC;
    }

    bool fpga::has_id() const
    {
      return _id_ >= 0;
    }
 
    void fpga::set_id(const int id_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "FPGA is already initialized!");
      _id_ = id_ < 0 ? -1 : id_;
      return;
    }

    int fpga::get_id() const
    {
      return _id_;
    }

    bool fpga::has_code() const
    {
      return _code_.get() != nullptr;
    }

    void fpga::set_code(const firmware_ptr & code_)
    {
      DT_THROW_IF(is_code_locked(), std::logic_error,
                  "FPGA code is locked!");
      _code_= code_;
      return;
    }

    void fpga::flash(const firmware_builder & fwb_, const std::string & fwid_)
    {
      set_code(fwb_.get_firmware(fwid_));
      return;
    }

    bool fpga::is_code_locked() const
    {
      return _code_lock_;
    }

    void fpga::lock_code()
    {
      _code_lock_ = true;
      return;
    }
  
    void fpga::unlock_code()
    {
      _code_lock_ = false;
      return;
    }

    const base_firmware & fpga::get_code() const
    {
      return *_code_.get();
    }

    bool fpga::is_initialized() const
    {
      return _initialized_;
    }
    
    void fpga::initialize(const datatools::properties & config_,
                          firmware_builder * fb_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "FPGA is already initialized!");

      if (!has_name()) {
        if (config_.has_key("name")) {
          std::string name = config_.fetch_string("name");
          set_name(name);
        }
      }
    
      if (!has_model()) {
        if (config_.has_key("model")) {
          std::string model_label = config_.fetch_string("model");
          if (model_label == fpga_model_label(FPGA_CONTROL)) {
            set_model(FPGA_CONTROL);
          } else if (model_label == fpga_model_label(FPGA_FRONTEND)) {
            set_model(FPGA_FRONTEND);
          } else if (model_label == fpga_model_label(FPGA_GENERIC)) {
            set_model(FPGA_GENERIC);
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

      if (!has_code()) {
        if (fb_ != nullptr) {
          if (config_.has_key("code.id")) {
            std::string code_id = config_.fetch_string("code.id");
            firmware_ptr new_code = fb_->make_firmware(code_id);
            set_code(new_code);
          }
        }
      }
    
      if (!has_name()) {
        set_name(build_fpga_name(_model_, _id_));
      }
      DT_THROW_IF(!has_name(), std::logic_error,
                  "FPGA has no name!");
      DT_THROW_IF(!has_model(), std::logic_error,
                  "FPGA has no model!");
      // DT_THROW_IF(!is_control_model() && !has_id(),
      //             std::logic_error,
      //             "FPGA has no ID!");
    
      if (has_code()) {
        if (config_.has_key("code.lock")) {
          bool fwlock = config_.fetch_boolean("code.lock");
          if (fwlock) {
            lock_code();
          }
        }
      }
    
      _initialized_ = true;
      return;
    }

    void fpga::initialize_simple()
    {
      datatools::properties config;
      initialize(config);
      return;
    }

    void fpga::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "FPGA is not initialized!");
      _initialized_ = false;
      _name_.clear();
      _model_ = FPGA_UNDEF;
      _id_ = -1;
      _code_lock_ = false;
      _code_.reset();
      return;
    }

    // virtual
    void fpga::print_tree(std::ostream & out_,
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
           << "Model : '" << fpga_model_label(_model_) << "'" << std::endl;

      out_ << popts.indent << tag
           << "ID   : ";
      if (has_id()) {
        out_ << _id_;
      } else {
        out_ << none_label();
      }
      out_ << std::endl;

      out_ << popts.indent << tag
           << "Code : ";
      if (has_code()) {
        out_ << "'" << _code_->full_id() << "'";
      } else {
        out_ << none_label();
      }
      out_ << std::endl;
 
      if (has_code()) {
        out_ << popts.indent << tag
             << "Code lock : " << std::boolalpha << _code_lock_ << std::endl;
      }
    
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Initialized : " << std::boolalpha << _initialized_ << std::endl;
    
      return;
    }


  } // namespace model
} // namespace snfee
