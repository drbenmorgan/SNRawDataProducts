// This project:
#include <snfee/model/setup_builder.h>

// Standard library:
#include <sstream>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/model/crate_builder.h>
#include <snfee/model/utils.h>
#include <snfee/model/board_builder.h>

namespace snfee {
  namespace model {

    setup_builder::setup_builder()
    {
      return;
    }

    setup_builder::~setup_builder()
    {
      return;
    }

    int setup_builder::get_setup_id() const
    {
      return _setup_id_;
    }

    void setup_builder::print_tree(std::ostream & out_,
                                   const boost::property_tree::ptree & options_) const
    {
      return;
    }

    std::shared_ptr<setup> setup_builder::build() const
    {
      std::shared_ptr<setup> p(new setup);
      p->set_name("SuperNEMO/FEE");
      p->set_tag(std::tostring(_setup_id_);

      if (_setup_id_ == 0) {
      }
      return p;
    }

    void setup_builder::_build_prod_supernemo(setup & setup_) const
    {
      board_builder boardBuilder;
      crate_builder crateBuilder(boardBuilder);
      crate_ptr calo_crate_0 = crateBuilder.make_crate(CRATE_CALORIMETER);
      crate_ptr calo_crate_1 = crateBuilder.make_crate(CRATE_CALORIMETER);
      crate_ptr calo_crate_2 = crateBuilder.make_crate(CRATE_CALORIMETER);
      setup_.add_crate(calo_crate_0, "Calo_0");
      setup_.add_crate(calo_crate_1, "Calo_1");
      setup_.add_crate(calo_crate_2, "Calo_2");
      return;
    }
  
  } // namespace model
} // namespace snfee
