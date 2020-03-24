// This project:
#include <snfee/model/setup.h>

// Standard library:
#include <sstream>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

namespace snfee {
  namespace model {

    setup::setup()
    {
      return;
    }

    setup::~setup()
    {
      return;
    }

    bool setup::has_name() const
    {
      return !_name_.empty();
    }
   
    void setup::set_name(const std::string & name_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Setup is initialized!");
      _name_ = name_;
      return;
    }

    const std::string & setup::get_name() const
    {
      return _name_;
    }

    bool setup::has_tag() const
    {
      return !_tag_.empty();
    }

    void setup::set_tag(const std::string & tag_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Setup is initialized!");
      _tag_ = tag_;
      return;
    }

    const std::string & setup::get_tag() const
    {
      return _tag_;
    }

    void setup::add_crate(const crate_ptr & crate_, const std::string & name_)
    {
      DT_THROW_IF(is_crate_layout_locked(), std::logic_error,
                  "Crate '" << name_ << "' layout is locked!");
      DT_THROW_IF(!crate_->has_model(),
                  std::logic_error,
                  "Crate '" << name_ << "' has no valid model!");
      DT_THROW_IF(!crate_->has_id(),
                  std::logic_error,
                  "Crate '" << name_ << "' has no ID!");
      std::string crateName = name_;
      if (crateName.empty()) {
        crateName = crate::build_crate_name(crate_->get_model(), crate_->get_id());
      }
      _crates_[crateName] = crate_;
      return;  
    }

    // crate & setup::add_crate(const crate_model_type model_, const int id_)
    // {
    //   std::string crateName = crate::build_crate_name(model_, id_);
    //   crate_ptr crateP(new crate);
    //   crateP->set_name(crateName);
    //   crateP->set_model(model_);
    //   crateP->set_id(id_);
    //   add_crate(crateP, crateName);
    //   return *_crates_.find(crateName)->second.get();
    // }
 
    bool setup::has_crate(const std::string & name_) const
    {
      return _crates_.count(name_);
    }

    bool setup::has_crate(const crate_model_type model_,
                          const int id_) const
    {
      std::string crateName = crate::build_crate_name(model_, id_);
      return _crates_.count(crateName);
    }

    std::set<std::string> setup::get_crate_names() const
    {
      std::set<std::string> names;
      for (const auto & p : _crates_) {
        const std::string & cname = p.first;
        names.insert(cname);
      }
      return names;
    }

    void setup::construct_paths(std::set<path_type> & paths_,
                                const std::string & /* filter_ */) const
    {
      // bool add_crates = false;
      // bool add_boards = false;
      // bool add_fpgas = false;
      // bool add_register = false;
    
      std::set<std::string> cnames = get_crate_names();
      for (const std::string & cname : cnames) {
        const crate & cr = get_crate(cname);
        // std::cerr << "[devel] crate = '" << cname << "'" << std::endl;
        // paths_.insert({{cname}}); 
        std::set<std::string> bnames = cr.get_board_names();
        for (const std::string & bname : bnames) {
          const base_board & brd = cr.get_board(bname);
          // std::cerr << "[devel] board = '" << bname << "'" << std::endl;
          // paths_.insert({{cname, bname}}); 
          std::set<std::string> fnames = brd.get_fpga_names();
          for (const std::string & fname : fnames) {
            const fpga & pga = brd.get_fpga(fname);
            // paths_.insert({{cname, bname, fname}}); 
            if (pga.has_code()) {
              std::set<std::string> rnames = pga.get_code().get_register_names();
              for (const std::string & rname : rnames) {
                paths_.insert({{cname, bname, fname, rname}}); 
              }
            }
          }
        }
      }
      return;
    }
  
    const crate & setup::get_crate(const path_type & path_) const
    {
      DT_THROW_IF(path_.size() != 1, std::range_error,
                  "Invalid depth for crate path '" << path_to_string(path_) << "'!");
      return get_crate(path_[0]);
    }

    const base_board & setup::get_board(const path_type & path_) const
    {
      DT_THROW_IF(path_.size() != 2, std::range_error,
                  "Invalid depth for board path '" << path_to_string(path_) << "'!");
      path_type cratePath = truncate_path(path_, 1);
      const crate & cr = get_crate(cratePath);
      return cr.get_board(path_[1]);
    }
  
    const fpga & setup::get_fpga(const path_type & path_) const
    {
      DT_THROW_IF(path_.size() != 3, std::range_error,
                  "Invalid depth for FPGA path '" << path_to_string(path_) << "'!");
      path_type boardPath = truncate_path(path_, 1);
      const base_board & brd = get_board(boardPath);
      return brd.get_fpga(path_[2]);
    }
  
    const register_description & setup::get_register(const path_type & path_) const
    {
      DT_THROW_IF(path_.size() != 4, std::range_error,
                  "Invalid depth for register path '" << path_to_string(path_) << "'!");
      path_type fpgaPath = truncate_path(path_, 1);
      const fpga & fpga = get_fpga(fpgaPath);
      return fpga.get_code().get_register(path_[3]);
    }

    crate & setup::grab_crate(const std::string & name_)
    {
      std::map<std::string, crate_ptr>::iterator found = _crates_.find(name_);
      DT_THROW_IF(found == _crates_.end(),
                  std::logic_error,
                  "No crate with name '" << name_ << "'!");
      return *found->second.get();
    }

    const crate & setup::get_crate(const std::string & name_) const
    {
      std::map<std::string, crate_ptr>::const_iterator found = _crates_.find(name_);
      DT_THROW_IF(found == _crates_.end(),
                  std::logic_error,
                  "No crate with name '" << name_ << "'!");
      return *found->second.get();
    }

    crate & setup::grab_crate(const crate_model_type model_,
                              const int id_)
    {
      std::string crateName = crate::build_crate_name(model_, id_);
      return grab_crate(crateName);
    }
 
    const crate & setup::get_crate(const crate_model_type model_,
                                   const int id_) const
    {
      std::string crateName = crate::build_crate_name(model_, id_);
      return get_crate(crateName);
    }

    bool setup::is_crate_layout_locked() const
    {
      return _crate_layout_locked_;
    }

    void setup::lock_crate_layout()
    {
      _crate_layout_locked_ = true;
      return;
    }

    void setup::unlock_crate_layout()
    {
      _crate_layout_locked_ = false;
      return;
    }
   
    bool setup::is_initialized() const
    {
      return _initialized_;
    }
     
    void setup::initialize(const datatools::properties & config_,
                           const crate_builder * cb_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Setup is already initialized!");

      this->logging = datatools::logger::extract_logging_configuration(config_,
                                                                       this->logging,
                                                                       true);
    
      if (!has_name()) {
        if (config_.has_key("name")) {
          std::string name = config_.fetch_string("name");
          set_name(name);
        }
      }
   
      if (!has_tag()) {
        if (config_.has_key("tag")) {
          std::string atag = config_.fetch_string("tag");
          set_tag(atag);
        }
      }

      std::set<std::string> crateNames;
      if (config_.has_key("crates")) {
        config_.fetch("crates", crateNames);
        for (const std::string & crateName : crateNames) {
          DT_LOG_DEBUG(logging, "Processing configuration for crate '" << crateName << "'...");
          crate_entry crateEntry;
          crateEntry.name = crateName;
          crateEntry.model = CRATE_UNDEF;
          std::string crateConfigPath;
          {
            DT_LOG_DEBUG(logging, "Searching model of crate '" << crateName << "'...");
            std::ostringstream crate_model_ss;
            crate_model_ss << "crates." << crateName << ".model";
            if (config_.has_key(crate_model_ss.str())) {
              std::string crate_model_repr = config_.fetch_string(crate_model_ss.str());
              crateEntry.model = crate_model_from(crate_model_repr);
              DT_THROW_IF(crateEntry.model == CRATE_UNDEF,
                          std::logic_error,
                          "Invalid crate model '" << crate_model_repr << "'!");
              DT_LOG_DEBUG(logging, "Found model '" << crate_model_repr << "' of crate '" << crateName << "'...");
            }
          }
          {
            DT_LOG_DEBUG(logging, "Searching config. path of crate '" << crateName << "'...");
            std::ostringstream crate_config_path_ss;
            crate_config_path_ss << "crates." << crateName << ".config_path";
            if (config_.has_key(crate_config_path_ss.str())) {
              crateConfigPath = config_.fetch_string(crate_config_path_ss.str());
              DT_THROW_IF(crateConfigPath.empty(),
                          std::logic_error,
                          "Missing crate config!");
              DT_LOG_DEBUG(logging, "Found config. path '" << crateConfigPath << "' of crate '" << crateName << "'...");
            }
            if (!crateConfigPath.empty()) {
              datatools::fetch_path_with_env(crateConfigPath);
              crateEntry.config.read_configuration(crateConfigPath);
              if (datatools::logger::is_debug(logging)) {
                crateEntry.config.tree_dump(std::cerr, "Loaded configuration for the '" + crateEntry.name + "' crate: ", "[debug] ");
              }
            }
          }
          _crate_entries_.push_back(crateEntry);
        }
      }
    
      if (cb_ != nullptr) {
        for (const crate_entry & crEnt : _crate_entries_) {
          crate_ptr cp = cb_->make_crate(crEnt.model, crEnt.config);
          add_crate(cp, crEnt.name);                     
        }
      }

      if (_crates_.size()) {
        if (config_.has_key("crate_layout.lock")) {
          bool cl_lock = config_.fetch_boolean("crate_layout.lock");
          if (cl_lock) {
            lock_crate_layout();
          }   
        }
      }
 
      _initialized_ = true;
      return;
    }
 
    void setup::initialize_simple()
    {
      datatools::properties config;
      initialize(config);
      return;
    }

    void setup::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Crate is not initialized!");
      _initialized_ = false;
      _name_.clear();
      _tag_.clear();
      _crate_layout_locked_ = false;
      _crates_.clear();
      return;
    }

    // virtual
    void setup::print_tree(std::ostream & out_,
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
           << "Tag : '" << _tag_ << "'" << std::endl;

      out_ << popts.indent << tag
           << "Crates : " << _crates_.size() << "" << std::endl;
 
      std::size_t crate_counter = 0;
      for (std::map<std::string, crate_ptr>::const_iterator i = _crates_.begin();
           i != _crates_.end();
           i++) {
        out_ << popts.indent << skip_tag;
        std::ostringstream crate_indent_ss;
        crate_indent_ss << popts.indent << skip_tag;
        if (++crate_counter == _crates_.size()) {
          out_ << last_tag;
          crate_indent_ss << last_skip_tag;
        } else {
          out_ << tag;
          crate_indent_ss << skip_tag;
        }
        out_ << "Crate " << i->first << std::endl;
        boost::property_tree::ptree options2;
        {
          options2.put("indent", crate_indent_ss.str());
        }
        i->second->print_tree(out_, options2);
      }
   
      if (_crates_.size()) {
        out_ << popts.indent << tag
             << "Crate layout lock : " << std::boolalpha << _crate_layout_locked_ << std::endl;
      }
 
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Initialized : " << std::boolalpha << _initialized_ << std::endl;

      return;
    }

  } // namespace model
} // namespace snfee
