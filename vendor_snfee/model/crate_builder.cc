// This project:
#include <snfee/model/crate_builder.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/model/crate.h>

namespace snfee {
  namespace model {

    crate_builder::crate_builder(const board_builder & board_maker_)
      : _board_maker_(board_maker_)
    {
      DT_THROW_IF(!_board_maker_.is_initialized(),
                  std::logic_error,
                  "Board builder is not initialized!");
      return;
    }

    // virtual
    crate_builder::~crate_builder()
    {
      return;
    }

    const board_builder & crate_builder::get_board_maker() const
    {
      return _board_maker_;
    }

    bool crate_builder::is_initialized() const
    {
      return _initialized_;
    }
   
    void crate_builder::initialize_simple()
    {
      datatools::properties config;
      initialize(config);
      return;
    }
    
    void crate_builder::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Crate builder is already initialized!");

      // Processing configuration parameters:
      this->logging = datatools::logger::extract_logging_configuration(config_,
                                                                       this->logging,
                                                                       true);

      // Initialization ops:
      if (_supported_crate_models_.size() == 0) {
        // Default to full list of SuperNEMO crates:
        add_supported_crate_model(CRATE_CALORIMETER);
        add_supported_crate_model(CRATE_TRACKER);
        add_supported_crate_model(CRATE_COMMISSIONING);
      }
    
      _initialized_ = true;
      return;
    }

    void crate_builder::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Crate builder is not initialized!");
      _initialized_ = false;
      _supported_crate_models_.clear();
      return;
    }
 
    bool crate_builder::is_crate_model_supported(const crate_model_type model_) const
    {
      return _supported_crate_models_.count(model_) > 0;
    }

    void crate_builder::add_supported_crate_model(const crate_model_type model_)
    {
      _supported_crate_models_.insert(model_);
      return;
    }
 
    const std::set<crate_model_type> & crate_builder::get_supported_crate_models() const
    {
      return _supported_crate_models_;
    }

    crate_ptr crate_builder::make_crate(const crate_model_type crate_model_,
                                        const datatools::properties & crate_config_) const
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Crate builder is not initialized!");
      DT_THROW_IF(!is_crate_model_supported(crate_model_), std::logic_error,
                  "Crate model '" << crate_model_label(crate_model_) << "' is not supported!");
      if (datatools::logger::is_debug(logging)) {
        crate_config_.tree_dump(std::cerr, "Configuration for the new crate: ", "[debug] ");
      }
    
      crate_ptr new_crate;
  
      DT_LOG_DEBUG(logging, "Making a '" << crate_model_label(crate_model_) << "' crate...");
      if (crate_model_ == CRATE_CALORIMETER) {
        new_crate.reset(new crate);
        new_crate->set_model(CRATE_CALORIMETER);
        new_crate->initialize(crate_config_, &_board_maker_);
      }
    
      if (crate_model_ == CRATE_TRACKER) {
        new_crate.reset(new crate);
        new_crate->set_model(CRATE_TRACKER);
        new_crate->initialize(crate_config_, &_board_maker_);
      }
    
      if (crate_model_ == CRATE_COMMISSIONING) {
        new_crate.reset(new crate);
        new_crate->set_model(CRATE_COMMISSIONING);
        new_crate->initialize(crate_config_, &_board_maker_);
      }

      if (datatools::logger::is_debug(logging)) {
        if (new_crate) new_crate->tree_dump(std::cerr, "New crate: ", "[debug] ");
      }
    
      return new_crate;
    }

    // virtual
    void crate_builder::print_tree(std::ostream & out_,
                                   const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }
   
      out_ << popts.indent << tag
           << "Board maker: " << &_board_maker_
           << std::endl;

      out_ << popts.indent << tag
           << "Supported crate models: " << _supported_crate_models_.size() << std::endl;
      std::size_t bt_counter = 0;
      for (std::set<crate_model_type>::const_iterator i = _supported_crate_models_.begin();
           i != _supported_crate_models_.end();
           i++) {
        out_ << popts.indent << skip_tag;
        std::ostringstream bt_indent_ss;
        if (++bt_counter == _supported_crate_models_.size()) {
          out_ << last_tag;
        } else {
          out_ << tag;
        }
        out_ << "Crate model: '" << crate_model_label(*i) << "'" << std::endl;
      }
   
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Initialized : "
           << std::boolalpha << _initialized_ << std::endl;
 
      return;
    }
  
  } // namespace model
} // namespace snfee
