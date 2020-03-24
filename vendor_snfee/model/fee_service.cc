// Ourselves:
#include <snfee/model/fee_service.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/model/crate_builder.h>
#include <snfee/model/board_builder.h>
#include <snfee/model/firmware_builder.h>
#include <snfee/model/fpga_builder.h>

namespace snfee {
  namespace model {

    /** Auto-registration of the service class in a central service Db */
    DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(fee_service, "snfee::model::fee_service")

    struct fee_service::work_type
    {
      std::unique_ptr<firmware_builder> firmware_factory;
      std::unique_ptr<fpga_builder>     fpga_factory;
      std::unique_ptr<board_builder>    board_factory;
      std::unique_ptr<crate_builder>    crate_factory;
      std::unique_ptr<setup>            fe_setup;
    };
  
    fee_service::fee_service()
      : datatools::base_service("fe2s",
                                "Front-end electronics service",
                                "0.1")
    {
      _initialized_ = false;
      return;
    }
  
    fee_service::~fee_service()
    {
      if (this->is_initialized()) this->fee_service::reset();
      return;
    }
  
    // virtual
    bool fee_service::is_initialized() const
    {
      return _initialized_;
    }
    
    // virtual
    int fee_service::initialize(const datatools::properties & config_,
                                datatools::service_dict_type & /* service_map_ */)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Service is already initialized!");
      _common_initialize(config_);
      _at_init_(config_);
    
      _initialized_ = true;
    
      DT_LOG_DEBUG(get_logging_priority(), "Initialization of service '" << get_name() << "' is done.");
      return datatools::SUCCESS;
    }  
    
    // virtual
    int fee_service::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Service is not initialized!");
      _initialized_ = false;
      _at_reset_();
    
      return datatools::SUCCESS;
    }

    void fee_service::_at_init_(const datatools::properties & config_)
    {
      _work_.reset(new work_type);
      
      _work_->firmware_factory.reset(new firmware_builder);
      {
        std::string fwbConfigPath;    
        if (config_.has_key("firmware_builder.config_path")) {
          fwbConfigPath = config_.fetch_string("firmware_builder.config_path");  
        }
        datatools::fetch_path_with_env(fwbConfigPath);
        datatools::properties fwfConfig;
        fwfConfig.read_configuration(fwbConfigPath);
        _work_->firmware_factory->initialize(fwfConfig);
      }

      {
        datatools::properties fpgaConfig;
        _work_->fpga_factory.reset(new fpga_builder(*_work_->firmware_factory));
        _work_->fpga_factory->initialize(fpgaConfig);
      }
    
      {
        datatools::properties bfConfig;
        _work_->board_factory.reset(new board_builder(*_work_->fpga_factory));
        _work_->board_factory->initialize(bfConfig);
      }
    
      {
        std::string cfConfigPath;    
        if (config_.has_key("crate_builder.config_path")) {
          cfConfigPath = config_.fetch_string("crate_builder.config_path");  
        }
        datatools::fetch_path_with_env(cfConfigPath);
        datatools::properties cfConfig;
        cfConfig.read_configuration(cfConfigPath);
        _work_->crate_factory.reset(new crate_builder(*_work_->board_factory));
        _work_->crate_factory->initialize(cfConfig);
      }

      {
        std::string setupConfigPath;    
        if (config_.has_key("setup.config_path")) {
          setupConfigPath = config_.fetch_string("setup.config_path");  
        }
        DT_THROW_IF(setupConfigPath.empty(), std::logic_error,
                    "No setup config path is set!");
        datatools::fetch_path_with_env(setupConfigPath);
        datatools::properties setupConfig;
        setupConfig.read_configuration(setupConfigPath);
        _work_->fe_setup.reset(new setup);
        _work_->fe_setup->initialize(setupConfig, _work_->crate_factory.get());
      }
   
      return;
    }

    void fee_service::_at_reset_()
    {
      _work_->fe_setup.reset();
      _work_->crate_factory.reset();
      _work_->board_factory.reset();
      _work_->fpga_factory.reset();
      _work_->firmware_factory.reset();
      _work_.reset();
      return;
    }

    // virtual
    void fee_service::print_tree(std::ostream & out_,
                                 const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      this->base_service::tree_dump(out_, popts.title, popts.indent, true);
    
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Setup : " << std::boolalpha << has_setup() << std::endl;
    
      return;
    }

    bool fee_service::has_setup() const
    {
      return _work_->fe_setup.get() != nullptr;
    }

    const setup & fee_service::get_setup() const
    {
      DT_THROW_IF(!has_setup(), std::logic_error, "No setup is instantiated!");
      return *_work_->fe_setup.get();
    }

  } // namespace model
} // namespace snfee
