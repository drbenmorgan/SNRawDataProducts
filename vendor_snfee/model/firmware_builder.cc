// This project:
#include <snfee/model/firmware_builder.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/multi_properties.h>
#include <bayeux/datatools/logger.h>

// This project:
// #include <snfee/model/fpga.h>

namespace snfee {
  namespace model {

    firmware_builder::firmware_builder()
    {
      return;
    }

    // virtual
    firmware_builder::~firmware_builder()
    {
      return;
    }

    bool firmware_builder::is_initialized() const
    {
      return _initialized_;
    }
    
    void firmware_builder::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Firmware builder is already initialized!");

      this->logging = datatools::logger::extract_logging_configuration(config_,
                                                                       this->logging,
                                                                       true);

      if (_supported_firmware_categories_.size() == 0) {
        // Default to full list of SuperNEMO firmwares:
        DT_LOG_DEBUG(logging, "Default to full list of SuperNEMO firmware categories...");
        add_supported_firmware_category(FWCAT_CFEB_FE);
        add_supported_firmware_category(FWCAT_CFEB_CTRL);
        add_supported_firmware_category(FWCAT_TFEB_FE);
        add_supported_firmware_category(FWCAT_TFEB_CTRL);
        add_supported_firmware_category(FWCAT_CB_FE);
        add_supported_firmware_category(FWCAT_CB_CTRL);
        add_supported_firmware_category(FWCAT_TB);
      }

      // Processing configuration parameters:
      if (config_.has_key("firmware_db_paths")) {
        std::set<std::string> firmware_db_paths;
        config_.fetch("firmware_db_paths", firmware_db_paths);
        for (auto p : firmware_db_paths) {
          DT_LOG_DEBUG(logging, "Add firmware DB path '" << p << "'...");
          add_firmware_db_path(p);
        }
      }

      // Initialization ops:
      _at_init_();
    
      _initialized_ = true;
      return;
    }
   
    void firmware_builder::initialize_simple()
    {
      datatools::properties config;
      initialize(config);
      return;
    }

    void firmware_builder::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Firmware builder is not initialized!");
      _initialized_ = false;
      _at_reset_();
      return;
    }

    void firmware_builder::_at_reset_()
    {
      _instances_.clear();
      _db_.clear();
      _firmware_db_paths_.clear();
      _supported_firmware_categories_.clear();
      return;
    }

    void firmware_builder::_at_init_()
    {
      DT_THROW_IF(_firmware_db_paths_.empty(), std::logic_error,
                  "Missing firmware DB path!");

      for (const std::string & p : _firmware_db_paths_) {
        // std::cerr << "[DEVEL] path = " << p << std::endl;
        std::string path = p;
        datatools::fetch_path_with_env(path);
        _load_db_(path);
      }
    
      return;
    }

    void firmware_builder::add_firmware_db_path(const std::string & path_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Firmware builder is already initialized!");
      _firmware_db_paths_.insert(path_);
      return;
    }

    const std::set<std::string> & firmware_builder::get_firmware_db_paths() const
    {
      return _firmware_db_paths_;
    }
 
    bool firmware_builder::is_firmware_category_supported(const firmware_category_type cat_) const
    {
      return _supported_firmware_categories_.count(cat_) > 0;
    }

    void firmware_builder::add_supported_firmware_category(const firmware_category_type cat_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Firmware builder is already initialized!");
      _supported_firmware_categories_.insert(cat_);
      return;
    }
 
    const std::set<firmware_category_type> &
    firmware_builder::get_supported_firmware_categories() const
    {
      return _supported_firmware_categories_;
    }

    void firmware_builder::_load_db_(const std::string & db_path_)
    {
      DT_LOG_DEBUG(logging, "Loading firmware registration entry from DB file '" << db_path_ << "'...");
      std::string db_path = db_path_;
      datatools::fetch_path_with_env(db_path);
      datatools::multi_properties mprops("id", "type");
      mprops.read(db_path);

      const datatools::multi_properties::entries_ordered_col_type & oe = mprops.ordered_entries();
      for (const auto pe : oe) {
        const datatools::multi_properties::entry & e = *pe;
        const std::string & fwid = e.get_key();
        const std::string & fwtype = e.get_meta();
        const datatools::properties & fwcfg = e.get_properties();
        firmware_category_type fwcat = FWCAT_UNDEF;
        if (fwcfg.has_key("category")) {
          fwcat = firmware_category_from(fwcfg.fetch_string("category"));
          if(!_supported_firmware_categories_.count(fwcat)) {
            DT_LOG_NOTICE(logging, "Skipping firmware registration entry '" << fwid << "' of type '" << fwtype << "'");
            continue;
          }
        }
        DT_THROW_IF(fwcat == FWCAT_UNDEF, std::logic_error, "Undocumented firmware category in '" << fwid << "'!");    
        firmware_db_entry fwentry;
        fwentry.id     = fwid;
        fwentry.type   = fwtype;
        fwentry.cat    = fwcat;
        fwentry.config = fwcfg;
        _db_[fwentry.id] = fwentry;
      }
    
      DT_LOG_DEBUG(logging, "Firmware registration entry from DB file '" << db_path_ << "' was loaded.");
      return;
    }

    bool firmware_builder::has_registered_firmware(const std::string & fwid_) const
    {
      return _db_.count(fwid_);
    }

    bool firmware_builder::has_firmware(const std::string & fwid_) const
    {
      return _instances_.count(fwid_);
    }

    firmware_ptr firmware_builder::get_firmware(const std::string & fwid_) const
    {
      firmware_pool_type::const_iterator found = _instances_.find(fwid_);
      if (found != _instances_.end()) {
        return found->second;
      }
      return const_cast<firmware_builder*>(this)->make_firmware(fwid_);
    }
  
    firmware_ptr firmware_builder::make_firmware(const std::string & fwid_, const firmware_category_type fwcat_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Firmware builder is not initialized!");
      DT_THROW_IF(!has_registered_firmware(fwid_), std::logic_error,
                  "Firmware '" << fwid_ << "' is not registered!");
      firmware_db_type::const_iterator found = _db_.find(fwid_);
      const firmware_db_entry & fwe = found->second;
      if (fwcat_ != FWCAT_UNDEF) {
        DT_THROW_IF(fwe.cat != fwcat_, std::logic_error,
                    "Firmware '" << fwid_<< "' with category '" << fwe.cat
                    << "' does not match the requirement '" << firmware_category_label(fwcat_) << "'!");      
      }
      const base_firmware::factory_register_type the_factory_register 
        = DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(base_firmware);
      DT_THROW_IF(!the_factory_register.has(fwe.type), std::logic_error,
                  "Firmware type '" << fwe.type << " is not registered!");
      const base_firmware::factory_register_type::factory_type & the_factory
        = the_factory_register.get(fwe.type);
      firmware_ptr new_firmware(the_factory());
      new_firmware->initialize(fwe.config);
      _instances_[fwid_] = new_firmware;
      return new_firmware;
    }

    // virtual
    void firmware_builder::print_tree(std::ostream & out_,
                                      const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag
           << "Supported firmware types: " << _supported_firmware_categories_.size() << std::endl;
      std::size_t bt_counter = 0;
      for (std::set<firmware_category_type>::const_iterator i = _supported_firmware_categories_.begin();
           i != _supported_firmware_categories_.end();
           i++) {
        out_ << popts.indent << skip_tag;
        std::ostringstream bt_indent_ss;
        if (++bt_counter == _supported_firmware_categories_.size()) {
          out_ << last_tag;
        } else {
          out_ << tag;
        }
        out_ << "Firmware category: '" << firmware_category_label(*i) << "'" << std::endl;
      }

      {
        out_ << popts.indent << tag
             << "Registered firmwares : " << _db_.size() << std::endl;
        std::size_t db_counter = 0;
        for (firmware_db_type::const_iterator i = _db_.begin();
             i != _db_.end();
             i++) {
          out_ << popts.indent << skip_tag;
          std::ostringstream db_indent_ss;
          db_indent_ss << popts.indent << skip_tag;
          if (++ db_counter == _db_.size()) {
            out_ << last_tag;
            db_indent_ss << last_skip_tag;
          } else {
            out_ << tag;
            db_indent_ss << skip_tag;
          }
          out_ << "Firmware: '" << i->first << "' (category = '"
               << firmware_category_label(i->second.cat) << "', type='" << i->second.type << "')" << std::endl;
        }
      }
    
      {
        out_ << popts.indent << tag
             << "Instantiated firmwares : " << _instances_.size() << std::endl;
        std::size_t fw_counter = 0;
        for (firmware_pool_type::const_iterator i = _instances_.begin();
             i != _instances_.end();
             i++) {
          out_ << popts.indent << skip_tag;
          std::ostringstream fw_indent_ss;
          fw_indent_ss << popts.indent << skip_tag;
          if (++ fw_counter == _instances_.size()) {
            out_ << last_tag;
            fw_indent_ss << last_skip_tag;
          } else {
            out_ << tag;
            fw_indent_ss << skip_tag;
          }
          out_ << "Firmware '" << i->first << "' :" << std::endl;
          boost::property_tree::ptree options2;
          {
            options2.put("indent", fw_indent_ss.str());
          }
          i->second->print_tree(out_, options2);
        }
      }
    
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Initialized : "
           << std::boolalpha << _initialized_ << std::endl;
 
      return;
    }
  
  } // namespace model
} // namespace snfee
