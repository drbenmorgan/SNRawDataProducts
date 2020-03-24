// This project:
#include <snfee/model/fpga_builder.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/model/fpga.h>
#include <snfee/model/firmware_builder.h>

namespace snfee {
  namespace model {

    fpga_builder::fpga_builder(const firmware_builder & fw_builder_)
      : _fwb_(fw_builder_)
    {
      DT_THROW_IF(!_fwb_.is_initialized(), std::logic_error,
                  "Firmware builder is not initialized!");
      return;
    }

    // virtual
    fpga_builder::~fpga_builder()
    {
      return;
    }

    bool fpga_builder::is_initialized() const
    {
      return _initialized_;
    }
    
    void fpga_builder::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "FPGA builder is already initialized!");

      // Processing configuration parameters:
      if (config_.has_key("supported_fpga_models")) {
        std::set<std::string> sfms;
        config_.fetch("supported_fpga_models", sfms);
        for (auto fm_repr : sfms) {
          fpga_model_type fm = fpga_model_from(fm_repr);
          DT_THROW_IF(fm == FPGA_UNDEF, std::logic_error,
                      "Invalid FPGA model '" << fm_repr << "'!");
          add_supported_fpga_model(fm);
        }
      }

      // Initialization ops:
      if (_supported_fpga_models_.size() == 0) {
        // Default to full list of SuperNEMO FPGA models:
        add_supported_fpga_model(FPGA_CONTROL);
        add_supported_fpga_model(FPGA_FRONTEND);
        add_supported_fpga_model(FPGA_GENERIC);
      }
    
      _initialized_ = true;
      return;
    }
   
    void fpga_builder::initialize_simple()
    {
      datatools::properties config;
      initialize(config);
      return;
    }

    void fpga_builder::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "FPGA builder is not initialized!");
      _initialized_ = false;
      _supported_fpga_models_.clear();
      return;
    }
 
    bool fpga_builder::is_fpga_model_supported(const fpga_model_type & model_) const
    {
      return _supported_fpga_models_.count(model_) > 0;
    }

    void fpga_builder::add_supported_fpga_model(const fpga_model_type & model_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "FPGA builder is already initialized!");
      _supported_fpga_models_.insert(model_);
      return;
    }
 
    const std::set<fpga_model_type> & fpga_builder::get_supported_fpga_models() const
    {
      return _supported_fpga_models_;
    }

    fpga fpga_builder::make_fpga_index(const fpga_model_type fpga_model_,
                                       const int id_,
                                       const std::string & fwid_) const
    {
      fpga pga;
      _make_fpga_(pga, fpga_model_, id_, fwid_);
      return pga;
    }

    fpga fpga_builder::make_fpga_unique(const fpga_model_type fpga_model_,
                                        const std::string & fwid_) const
    {
      fpga pga;
      _make_fpga_(pga, fpga_model_, -1, fwid_);
      return pga;
    }

    void fpga_builder::_make_fpga_(fpga & pga_,
                                   const fpga_model_type fpga_model_,
                                   const int id_,
                                   const std::string & fwid_) const
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "FPGA builder is not initialized!");
      DT_THROW_IF(!is_fpga_model_supported(fpga_model_),
                  std::logic_error,
                  "FPGA builder is not initialized!");
      pga_.set_name(fpga::build_fpga_name(fpga_model_, id_));
      pga_.set_model(fpga_model_);
      if (id_ >= 0) {
        pga_.set_id(id_);
      }
      if (!fwid_.empty()) {
        pga_.set_code(_fwb_.get_firmware(fwid_));
      }
      pga_.initialize_simple();
      return;
    }

    // virtual
    void fpga_builder::print_tree(std::ostream & out_,
                                  const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag
           << "Supported FPGA models: " << _supported_fpga_models_.size() << std::endl;
      std::size_t bt_counter = 0;
      for (std::set<fpga_model_type>::const_iterator i = _supported_fpga_models_.begin();
           i != _supported_fpga_models_.end();
           i++) {
        out_ << popts.indent << skip_tag;
        std::ostringstream bt_indent_ss;
        if (++bt_counter == _supported_fpga_models_.size()) {
          out_ << last_tag;
        } else {
          out_ << tag;
        }
        out_ << "FPGA model: '" << fpga_model_label(*i) << "'" << std::endl;
      }
   
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Initialized : "
           << std::boolalpha << _initialized_ << std::endl;
 
      return;
    }
  
  } // namespace model
} // namespace snfee
