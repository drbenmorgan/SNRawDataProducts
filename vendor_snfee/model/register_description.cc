// Ourselves:
#include <snfee/model/register_description.h>

// Third party:
// - Boost:
#include <boost/dynamic_bitset.hpp>
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/utils.h>
#include <snfee/model/utils.h>

namespace snfee {
  namespace model {

    register_description::register_description()
    {
      return;
    }

    register_description::register_description(const std::string & name_,
                                               const std::string & desc_,
                                               const std::string & address_,
                                               const std::size_t nbits_,
                                               const access_type access_,
                                               const usage_type usage_,
                                               const action_type action_,
                                               const std::string & defval_)
    {
      set_name(name_);
      set_description(desc_);
      set_address(address_);
      set_nbits(nbits_);
      set_access(access_);
      set_usage(usage_);
      set_action(action_);
      if (!defval_.empty()) {
        set_default_value(defval_);
      }
      return;
    }
 
    // virtual
    register_description::~register_description()
    {
      return;
    }

    void register_description::reset()
    {
      _name_.clear();
      _description_.clear();
      _address_.clear();
      _nbits_  = 0;
      _access_ = ACCESS_UNDEF;
      _usage_  = USAGE_UNDEF;
      _action_ = ACTION_UNDEF;
      _default_value_.clear();
      return;
    }
 
    bool register_description::has_usage() const
    {
      return _usage_ != USAGE_UNDEF;
    }
  
    void register_description::set_usage(const usage_type usage_)
    {
      _usage_ = usage_;
      return;
    }

    usage_type register_description::get_usage() const
    {
      return _usage_;
    }

    bool register_description::has_action() const
    {
      return _action_ != ACTION_UNDEF;
    }
  
    void register_description::set_action(const action_type action_)
    {
      _action_ = action_;
      return;
    }

    action_type register_description::get_action() const
    {
      return _action_;
    }

    bool register_description::has_access() const
    {
      return _access_ != ACCESS_UNDEF;
    }
  
    void register_description::set_access(const access_type access_)
    {
      _access_ = access_;
      return;
    }

    access_type register_description::get_access() const
    {
      return _access_;
    }

    bool register_description::is_read_only() const
    {
      return _access_ == ACCESS_READ;
    }

    bool register_description::is_write_only() const
    {
      return _access_ == ACCESS_WRITE;
    }

    bool register_description::is_read_write() const
    {
      return _access_ == ACCESS_RW;
    }
        
    bool register_description::is_readable() const
    {
      return _access_ == ACCESS_READ || _access_ == ACCESS_RW;
    }
        
    bool register_description::is_writable() const
    {
      return _access_ == ACCESS_WRITE || _access_ == ACCESS_RW;
    }

    bool register_description::is_valid() const
    {
      if (!has_name()) return false;
      if (!has_address()) return false;
      if (!has_nbits()) return false;
      if (!has_access()) return false;
      return true;
    }

    bool register_description::has_name() const
    {
      return !_name_.empty();
    }
   
    void register_description::set_name(const std::string & name_)
    {
      _name_ = name_;
      return;
    }

    const std::string & register_description::get_name() const
    {
      return _name_;
    }

    bool register_description::has_description() const
    {
      return !_description_.empty();
    }
   
    void register_description::set_description(const std::string & description_)
    {
      _description_ = description_;
      return;
    }

    const std::string & register_description::get_description() const
    {
      return _description_;
    }

    bool register_description::has_address() const
    {
      return !_address_.empty();
    }
   
    void register_description::set_address(const std::string & address_)
    {
      _address_ = address_;
      return;
    }

    const std::string & register_description::get_address() const
    {
      return _address_;
    }

    bool register_description::has_nbits() const
    {
      return _nbits_ > 0;
    }
   
    void register_description::set_nbits(const std::size_t n_)
    {
      DT_THROW_IF(n_ > MAX_SIZE,
                  std::range_error,
                  "Unsupported register size " << n_ << " > " << MAX_SIZE << "!");
      if (has_default_value()) {
        unsigned long defnumval;
        parse(_default_value_, defnumval, 0);
        if (check_overflow(defnumval, n_)) {
          DT_THROW(std::logic_error,
                   "Number of bits " << n_ << " is too low to match the register's default value '" << _default_value_ << "'!");
        }
      }
      _nbits_ = n_;
      return;
    }
    
    std::size_t register_description::get_nbits() const
    {
      return _nbits_;
    }

    bool register_description::has_default_value() const
    {
      return _default_value_.length() > 0;
    }

    const std::string & register_description::get_default_value() const
    {
      DT_THROW_IF(!has_default_value(), std::logic_error,
                  "Register has no default value!");
      return _default_value_;
    }

    // static
    bool register_description::check_overflow(const unsigned long value_,
                                              const std::size_t nbits_)
    {
      if (nbits_ == 0) return true;
      if (nbits_ >= MAX_SIZE) return false;
      // std::cerr << "[debug] sizeof(unsigned long) = " << sizeof(unsigned long) << std::endl;   
      unsigned long max_value = (0x1 << nbits_) - 1;
      // std::cerr << "[debug] nbits     = " << nbits_ << std::endl;
      // std::cerr << "[debug] max value = " << max_value << std::endl;
      return value_ > max_value;
    }

    void register_description::set_default_value(const std::string & v_)
    {
      if (v_.empty()) {
        _default_value_.clear();
        return;
      }
      {
        unsigned long defnumval = 0;
        if (!parse(v_, defnumval, 0)) {
          DT_THROW(std::logic_error,
                   "Invalid format for register's default value '" << v_ << "'!");
        }
        if (has_nbits()) {
          if (check_overflow(defnumval, _nbits_)) {
            DT_THROW(std::logic_error,
                     "Overflow for register's default value '" << v_ << "'!");
          }
        }
      }
      _default_value_ = v_;
      return;
    }

    bool register_description::validate(const unsigned long value_) const
    {
      return !check_overflow(value_, _nbits_);
    }
 
    unsigned long register_description::get_default_value_as_ulong() const
    {
      DT_THROW_IF(!has_default_value(), std::logic_error,
                  "Register has no default value!");
      unsigned long defnumval;
      parse(_default_value_, defnumval, 0);
      return defnumval;
    }
                
    boost::dynamic_bitset<> register_description::get_default_value_as_bitset() const
    {
      unsigned long defnumval = get_default_value_as_ulong();
      return  boost::dynamic_bitset<>(_nbits_, defnumval);
    }
   
    // virtual
    void register_description::print_tree(std::ostream & out_,
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
           << "Description : ";
      if (has_description()) {
        out_ << "'" << _description_ << "'";
      } else {
        out_ << none_label();
      }
      out_<< std::endl;
    
      out_ << popts.indent << tag
           << "Number of bits : " << _nbits_ << std::endl;
    
      out_ << popts.indent << tag
           << "Address : " << _address_ << std::endl;
   
      out_ << popts.indent << tag
           << "Access : ";
      if (has_access()) {
        out_ << "'" << access_label(_access_) << "'";
      } else {
        out_ << none_label();
      }
      out_<< std::endl;
   
      out_ << popts.indent << tag
           << "Usage : ";
      if (has_usage()) {
        out_ << "'" << usage_label(_usage_) << "'";
      } else {
        out_ << none_label();
      }
      out_<< std::endl;
   
      out_ << popts.indent << tag
           << "Action : ";
      if (has_action()) {
        out_ << "'" << action_label(_action_) << "'";
      } else {
        out_ << none_label();
      }
      out_<< std::endl;
    
      out_ << popts.indent << tag
           << "Default value : ";
      if (has_default_value()) {
        out_ << "'" << _default_value_ << "'";
      } else {
        out_ << none_label();
      }
      out_ << std::endl;
   
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Valid : " << std::boolalpha << is_valid() << std::endl;
      return;
    }
  
  } // namespace model
} // namespace snfee
