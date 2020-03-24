//! \file snfee/io/rhd_sorter.h
//! \brief Sorter for RHD records
//
// Copyright (c) 2019 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef SNFEE_IO_RHD_SORTER_H
#define SNFEE_IO_RHD_SORTER_H

// Standard library:
#include <string>
#include <vector>
#include <memory>

// Third party:
// - Boost:
#include <boost/utility.hpp>
// - Bayeux:
#include <bayeux/datatools/logger.h>

// This project:
#include <snfee/io/rhd_record.h>
#include <snfee/data/raw_record_interface.h>

namespace snfee {
  namespace io {

    //! \brief Sorting algorithm for raw hit records (RHD)
    class rhd_sorter
      : private boost::noncopyable
    {
    public:

      static const std::size_t DEFAULT_SORT_BUFFER_SIZE       =  4000;
      static const std::size_t DEFAULT_EVALUATION_BUFFER_SIZE = 10000;
           
      /// \brief Configuration data: 
      struct config_type
      {
        std::string input_rhd_listname;                ///< Name of a file with a sequence of RHD input filenames
        std::vector<std::string> input_rhd_filenames;  ///< Sequence of RHD input filenames
        std::string mode_label;                        ///< Running mode ('sort' or 'evaluation')
        bool no_store = false;                         ///< No store flag
        std::string output_rhd_listname;               ///< Name of a file with a sequence of sorted RHD output filenames
        std::vector<std::string> output_rhd_filenames; ///< Sequence of sorted RHD output filenames
        std::size_t max_records_per_file = 0;          ///< Max number of RHD records per file 
        // std::size_t max_total_records = 0;          ///< Max number of RHD records (very risky)
        std::size_t sort_buffer_size       = DEFAULT_SORT_BUFFER_SIZE;       ///< Size of the sorting buffer
        std::size_t evaluation_buffer_size = DEFAULT_EVALUATION_BUFFER_SIZE; ///< Size of the evaluation buffer
        std::size_t log_modulo = 10000;                ///< Modulo for log print
        std::string report_filename;                   ///< Name of a report file ('sort' or 'evaluation')
      };
 
      //! Constructor
      rhd_sorter();
 
      //! Destructor
      virtual ~rhd_sorter();
 
      //! Return logging priority
      datatools::logger::priority get_logging() const;

      //! Set logging priority
      void set_logging(const datatools::logger::priority);
     
      //! Set the configuration
      void set_config(const config_type &);
      
      //! Check if the sorter is initialized
      bool is_initialized() const;

      //! Initialize the sorter
      void initialize();

      //! Run the sorter
      void run();
      
      //! Reset the sorter
      void terminate();

    private:
      
      //! Add an entry
      void _process_data_(const snfee::data::raw_record_ptr & ph_);

      //! Load a hit
      void _input_unsorted_record_(snfee::data::raw_record_ptr & ph_);

      //! Store a hit
      void _output_sorted_record_(const snfee::data::raw_record_ptr & ph_);
      
      //! Finalize the process
      void _finalize_();

    private:
    
      // Management:
      bool _initialized_ = false; ///< Initialization flag
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL; ///< Logging priority threshold
      
      // Configuration:
      config_type _config_; ///< Configuration

      // Working data:
      struct pimpl_type;
      std::unique_ptr<pimpl_type> _pimpl_; ///< Private working data
      
    };

  } // namespace io
} // namespace snfee

#endif // SNFEE_IO_RHD_SORTER_H

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
