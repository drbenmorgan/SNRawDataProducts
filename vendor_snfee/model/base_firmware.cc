// This project:
#include <snfee/model/base_firmware.h>

// Standard library:
#include <sstream>
#include <fstream>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
// - Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/logger.h>

namespace snfee {
namespace model {

  // Factory interface :
  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(base_firmware, "snfesetup::base_firmware/__system__")

  // Registration interface :
  SNFEE_MODEL_FIRMWARE_REGISTRATION_IMPLEMENT(base_firmware, "snfesetup::base_firmware")

  base_firmware::base_firmware()
  {
    return;
  }

  base_firmware::~base_firmware()
  {
    return;
  }

  bool base_firmware::has_name() const
  {
    return !_name_.empty();
  }
   
  void base_firmware::set_name(const std::string & name_)
  {
    _name_ = name_;
    return;
  }

  const std::string & base_firmware::get_name() const
  {
    return _name_;
  }

  bool base_firmware::has_version() const
  {
    return !_version_.empty();
  }

  void base_firmware::set_version(const std::string & version_)
  {
    _version_ = version_;
    return;
  }

  const std::string & base_firmware::get_version() const
  {
    return _version_;
  }

  std::string base_firmware::full_id() const
  {
    std::ostringstream f;
    f << _name_;
    if (has_version()) {
      f << "-" << _version_;
    }
    return f.str();
  }

  bool base_firmware::has_category() const
  {
    return _category_ != FWCAT_UNDEF;
  }
  
  void base_firmware::set_category(const firmware_category_type category_)
  {
    _category_ = category_;
    return;
  }

  firmware_category_type base_firmware::get_category() const
  {
    return _category_;
  }

  const std::map<std::string, register_description> & base_firmware::get_registers() const
  {
    return _registers_;
  }

  std::set<std::string> base_firmware::get_register_names() const
  {
    std::set<std::string> names;
    for (const auto & p : _registers_) {
      const std::string & rname = p.first;
      names.insert(rname);
    }
    return names;
  }

  bool base_firmware::has_register(const std::string & name_) const
  {
    return _registers_.count(name_);
  }

  bool base_firmware::has_register_at(const std::string & address_) const
  {
    for (const auto & p : _registers_) {
      const register_description & reg = p.second;
      if (reg.get_address() == address_) return true;
    }
    return false;
  }
  
  void base_firmware::add_register(const register_description & reg_, const std::string & name_)
  {
    DT_THROW_IF(!reg_.is_valid(), std::logic_error,
                "Attempt to add an invalid register in firmware " << full_id() << "!");
    std::string name = name_;
    if (name.empty()) {
      name = reg_.get_name();
    }
    DT_THROW_IF(has_register(name), std::logic_error,
                "Firmware " << full_id() << " already has register named '" << name_ << "'!");
    DT_THROW_IF(has_register_at(reg_.get_address()), std::logic_error,
                "Firmware " << full_id() << " already has register named '" << name_ << "' with address '" << reg_.get_address() << "'!");
    _registers_[name] = reg_;
    return;
  }
  
  void base_firmware::remove_register(const std::string & name_)
  {
    std::map<std::string, register_description>::const_iterator found = _registers_.find(name_);
    DT_THROW_IF(found == _registers_.end(), std::logic_error,
                "Firmware " << full_id() << " has no register named '" << name_ << "'!");
    _registers_.erase(name_);
    return;
  }
  
  const register_description & base_firmware::get_register(const std::string & name_) const
  {
    std::map<std::string, register_description>::const_iterator found = _registers_.find(name_);
    DT_THROW_IF(found == _registers_.end(), std::logic_error,
                "Firmware " << full_id() << " has no register named '" << name_ << "'!");
    return found->second;
  }
  
  void base_firmware::build_register_names(std::set<std::string> & names_) const
  {
    names_.clear();
    for (const auto & p : _registers_) {
      names_.insert(p.first);
    }
    return;
  }
  
  bool base_firmware::is_register_layout_locked() const
  {
    return _register_layout_locked_;
  }

  void base_firmware::lock_register_layout()
  {
    _register_layout_locked_ = true;
    return;
  }

  void base_firmware::unlock_register_layout()
    {
    _register_layout_locked_ = false;
    return;
  }
  
  // virtual
  void base_firmware::print_tree(std::ostream & out_,
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
         << "Version : '" << _version_ << "'" << std::endl;
  
    out_ << popts.indent << tag
         << "Category : '" << firmware_category_label(_category_) << "'" << std::endl;
 
    out_ << popts.indent << tag
         << "Registers : " << _registers_.size() << std::endl;

    std::size_t reg_counter = 0;
    for (std::map<std::string, register_description>::const_iterator i = _registers_.begin();
         i != _registers_.end();
         i++) {
      out_ << popts.indent << skip_tag;
      std::ostringstream reg_indent_ss;
      reg_indent_ss << popts.indent << skip_tag;
      if (++reg_counter == _registers_.size()) {
        out_ << last_tag;
        reg_indent_ss << last_skip_tag;
      } else {
        out_ << tag;
        reg_indent_ss << skip_tag;
      }
      out_ << "Register: '" << i->first << "'" << std::endl;
      boost::property_tree::ptree options2;
      {
        options2.put("indent", reg_indent_ss.str());
      }
      i->second.print_tree(out_, options2);
    }

    if (_registers_.size()) {
      out_ << popts.indent << tag
           << "Register layout lock : " << std::boolalpha << _register_layout_locked_ << std::endl;
    }

    out_ << popts.indent << inherit_tag(popts.inherit)
         << "Initialized : " << std::boolalpha << _initialized_ << std::endl;
   
    return;
  }

  bool base_firmware::is_initialized() const
  {
    return _initialized_;
  }
    
  void base_firmware::initialize(const datatools::properties & config_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Firmware is already initialized!");
    _at_initialize_(config_);    
    _initialized_ = true;
    return;
  }

  void base_firmware::reset()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Firmware is not initialized!");
    _initialized_ = false;
    _at_reset_();
    return;
  }

  void base_firmware::store_registers_to_csv(std::ostream & out_) const
  {
    for (const auto & p : _registers_) {
      const register_description & reg = p.second;
      
      out_ << reg.get_name() << ';';
      out_ << reg.get_address() << ';';
      out_ << reg.get_nbits() << ';';
      if (reg.has_access()) {
        out_ << access_label(reg.get_access());
      }
      out_ << ';';
      if (reg.has_usage()) {
        out_ << usage_label(reg.get_usage());
      }
      out_ << ';';
      if (reg.has_action()) {
        out_ << action_label(reg.get_action());
      }
      out_ << ';';
      if (reg.has_default_value()) {
        out_ << reg.get_default_value();
      }
      out_ << ';';
      if (reg.has_description()) {
        out_ << reg.get_description();
      }
      out_ << std::endl;
    }
    return;
  }
  
  void base_firmware::store_registers_to_csv_file(const std::string & filename_) const
  {
    std::string registers_list = filename_;
    datatools::fetch_path_with_env(registers_list);
    std::ofstream ofrl(registers_list.c_str());
    DT_THROW_IF(! ofrl,
                std::runtime_error,
                "Cannot open registers definition file '" << filename_ << "'!");
    store_registers_to_csv(ofrl);
    return;
  }

  void base_firmware::load_registers_from_csv_file(const std::string & filename_)
  {
    std::string registers_list = filename_;
    datatools::fetch_path_with_env(registers_list);
    std::ifstream ifrl(registers_list.c_str());
    DT_THROW_IF(! ifrl,
                std::runtime_error,
                "Cannot open registers definition file '" << filename_ << "'!");
    load_registers_from_csv(ifrl);
    return;
  }

  void base_firmware::load_registers_from_csv(std::istream & in_)
  {
    std::size_t line_counter = 0;
    in_ >> std::ws;
    if (in_.eof()) return;
    while (in_) {
      in_ >> std::ws;
      std::string line;
      std::getline(in_, line);
      line_counter++;
      DT_THROW_IF(!in_,
                  std::logic_error,
                  "I/O error or invalid format at line #" << line_counter << "!");
      if (line.empty()) {
        continue;
      }
      if (line[0] == '#') {
        continue;
      }
      std::vector<std::string> tokens;
      boost::split(tokens, line, boost::is_any_of(";"));
      DT_THROW_IF(tokens.size() != 8,
                  std::logic_error,
                  "Invalid register definition line format at line #" << line_counter << "!");
      register_description reg;

      int tokindex = 0;
      std::string name = tokens[tokindex];
      boost::trim(name);
      reg.set_name(name);
       
      std::string address = tokens[++tokindex];
      boost::trim(address);
      reg.set_address(address);
      
      std::size_t nbits = 0;
      try {
        std::string nbits_repr = tokens[++tokindex];
        boost::trim(nbits_repr);
        nbits = std::stoul(nbits_repr);
      } catch (std::exception &) {
        DT_THROW(std::logic_error,
                 "Invalid format for number of bits at line #" << line_counter << "!");
      }
      reg.set_nbits(nbits);
      
      std::string access_repr = tokens[++tokindex];
      boost::trim(access_repr);
      access_type access = access_from(access_repr);
      DT_THROW_IF(access == ACCESS_UNDEF,
                  std::logic_error,
                  "Invalid register access '" << access_repr << "' at line #" << line_counter << "!");
      reg.set_access(access);
      
      std::string usage_repr = tokens[++tokindex];
      boost::trim(usage_repr);
      if (!usage_repr.empty()) {
        usage_type usage = usage_from(usage_repr);
        DT_THROW_IF(usage == USAGE_UNDEF,
                    std::logic_error,
                    "Invalid register usage '" << usage_repr << "' at line #" << line_counter << "!");
        reg.set_usage(usage);
      }
      
      std::string action_repr = tokens[++tokindex];
      boost::trim(action_repr);
      if (!action_repr.empty()) {
        action_type action = action_from(action_repr);
        DT_THROW_IF(action == ACTION_UNDEF,
                    std::logic_error,
                    "Invalid register action '" << action_repr << "' at line #" << line_counter << "!");
        reg.set_action(action);
      }
      
      std::string defval = tokens[++tokindex];
      boost::trim(defval);
      if (!defval.empty()) {
        // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "Parsed default value = '" << defval << "'");
        reg.set_default_value(defval);
      }
     
      std::string description = tokens[++tokindex];
      boost::trim(description);
      reg.set_description(description);
      
      add_register(reg);
      // Detect end of file:
      in_ >> std::ws;
      if (in_.eof()) {
        break;
      }
    }
    return;
  }

  void base_firmware::_common_initialize(const datatools::properties & config_)
  {
    if (!has_name()) {
      if (config_.has_key("name")) {
        std::string name = config_.fetch_string("name");
        set_name(name);
      }
    }
    
    if (!has_version()) {
      if (config_.has_key("version")) {
        std::string version = config_.fetch_string("version");
        set_version(version);
      }
    }
    
    if (!has_category()) {
      if (config_.has_key("category")) {
        std::string category_repr = config_.fetch_string("category");
        firmware_category_type fw_category = firmware_category_from(category_repr);
        DT_THROW_IF(fw_category == FWCAT_UNDEF, std::logic_error,
                    "Invalid firmware category '" << category_repr << "'!");
        set_category(fw_category);
      }
    }

    std::set<std::string> registers_csv_lists;
    if (config_.has_key("registers.csv_lists")) {
      config_.fetch("registers.csv_lists", registers_csv_lists);
      for (const std::string & reglist : registers_csv_lists) {
        load_registers_from_csv_file(reglist);
      }
    }
    
    return;
  }
  
  void base_firmware::_common_reset()
  {

    _name_.clear();
    _version_.clear();
    _category_ = FWCAT_UNDEF;
    _registers_.clear();
    return;
  }

  // virtual
  void base_firmware::_at_initialize_(const datatools::properties & config_)
  {
    _common_initialize(config_);
    return;
  }

  // virtual
  void base_firmware::_at_reset_()
  {
    _common_reset();
    return;
  }

} // namespace model
} // namespace snfee
