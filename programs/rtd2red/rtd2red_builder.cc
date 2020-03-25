// This project:
#include <snfee/redb/rtd2red_builder.h>

namespace snfee {
  namespace redb {

    rtd2red_builder::rtd2red_builder(
      builder_pimpl_type& pimpl_,
      const int32_t run_id_,
      const builder_config::build_algo_type algo_,
      const datatools::logger::priority logging_)
      : _pimpl_(pimpl_)
    {
      _logging_ = logging_;
      _algo_ = algo_;
      _run_id_ = run_id_;
      return;
    }

    void
    rtd2red_builder::stop()
    {
      _stop_request_ = true;
      return;
    }

    bool
    rtd2red_builder::is_stopped() const
    {
      return _stop_request_;
    }

    std::size_t
    rtd2red_builder::get_red_records_counter() const
    {
      return _red_records_counter_;
    }

    int32_t
    rtd2red_builder::get_next_trigger_id_from_input_buffer() const
    {
      // snfee::data::trigger_id_comparator_type trigger_id_less;
      int32_t trigid = snfee::data::INVALID_TRIGGER_ID;
      bool cannot_determine = false;
      {
        auto& ibuf = _pimpl_.ibuffer;
        {
          std::lock_guard<std::mutex> lck(*_pimpl_.imtx);
          if (!ibuf.is_finished()) {
            // We consider only a running buffer:
            int32_t next_trig_id = ibuf.get_next_poppable_trig_id();
            if (next_trig_id == snfee::data::INVALID_TRIGGER_ID) {
              // No way to determine the next trigger ID to be collected:
              cannot_determine = true;
            }
            if (!cannot_determine) {
              if (ibuf.get_front_trig_id() != snfee::data::INVALID_TRIGGER_ID) {
                if (trigid == snfee::data::INVALID_TRIGGER_ID) {
                  trigid = next_trig_id;
                }
              }
            }
          }
          // DT_LOG_DEBUG(_logging_, "Input buffer #" << i << " is unlocked by
          // merger.");
        }
        if (cannot_determine) {
          trigid = snfee::data::INVALID_TRIGGER_ID;
        }
      }
      return trigid;
    }

    int32_t
    rtd2red_builder::get_minimum_trigger_id_from_input_buffer() const
    {
      snfee::data::trigger_id_comparator_type trigger_id_less;
      int32_t mintrigid = snfee::data::INVALID_TRIGGER_ID;
      bool cannot_determine = false;
      {
        auto& ibuf = _pimpl_.ibuffer;
        {
          std::lock_guard<std::mutex> lck(*_pimpl_.imtx);
          // DT_LOG_DEBUG(_logging_, "Input buffer #" << i << " is locked by
          // merger.");
          if (!ibuf.is_finished()) {
            // We consider only a running buffer:
            int32_t next_trig_id = ibuf.get_next_poppable_trig_id();
            if (next_trig_id == snfee::data::INVALID_TRIGGER_ID) {
              // No way to determine the next trigger ID to be collected:
              cannot_determine = true;
            }
            if (!cannot_determine) {
              if (ibuf.get_front_trig_id() != snfee::data::INVALID_TRIGGER_ID) {
                if (mintrigid == snfee::data::INVALID_TRIGGER_ID or
                    trigger_id_less(next_trig_id, mintrigid)) {
                  mintrigid = next_trig_id;
                }
              }
            }
          }
          // DT_LOG_DEBUG(_logging_, "Input buffer #" << i << " is unlocked by
          // merger.");
        }
        if (cannot_determine) {
          mintrigid = snfee::data::INVALID_TRIGGER_ID;
        }
      }
      return mintrigid;
    }

    void
    rtd2red_builder::run()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      if (_algo_ == builder_config::BUILD_ALGO_STANDARD) {
        DT_LOG_NOTICE(_logging_, "Running standard builder algo...");
        _run_standard_algo_();
      } else if (_algo_ == builder_config::BUILD_ALGO_ONETOONE) {
        DT_LOG_NOTICE(_logging_, "Running standard 1-to-1 algo...");
        _run_one2one_algo_();
      }
      DT_LOG_NOTICE(_logging_, "Builder run is stopped.");
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void
    rtd2red_builder::_run_one2one_algo_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      // snfee::io::red_record red_rec;

      // _event_id_ = 0;
      // _red_records_counter_ = 0;
      // while (!_stop_request_) {

      //   // Extract the next trigger ID from the input buffer:
      //   int32_t fetchable_trig_id = get_next_trigger_id_from_input_buffer();
      //   bool process_input_rtd = false;
      //   if (fetchable_trig_id == snfee::data::INVALID_TRIGGER_ID) {
      //     // The input buffer cannot provide new records yet
      //     // DT_LOG_DEBUG(_logging_, "No fetchable trigger ID is available
      //     yet!");
      //   } else {
      //     // DT_LOG_DEBUG(_logging_, "Fetchable trigger ID is : " <<
      //     fetchable_trig_id); process_input_rtd = true;
      //   }

      //   if (process_input_rtd) {
      //          // Scan the input buffer:
      //     DT_LOG_DEBUG(_logging_, "Inspect input buffer");
      //       {
      //         std::lock_guard<std::mutex> lck(*_pimpl_.imtx);
      //         DT_LOG_DEBUG(_logging_, "Input buffer is locked by builder.");
      //         auto & ibuf = _pimpl_.ibuffer;
      //         if (datatools::logger::is_debug(_logging_)) {
      //           ibuf.print(std::cerr);
      //         }
      //              const snfee::io::rtd_record & rec = ibuf.pop_record();
      //              const snfee::data::raw_trigger_data & rtd = rec.get_rtd();

      //            }
      //          }

      //     // There is some RTD to be processed:
      //     if (red_rec.get_event_id() == snfee::data::INVALID_EVENT_ID) {
      //       // We initiate a new RTD working record to host upcoming RHD
      //       records: DT_LOG_DEBUG(_logging_, "Make a new working RTD record
      //       with trigger ID = "
      //                    << fetchable_trig_id);
      //       int64_t reference_time = 0;

      //       // *** Compute reference time here ! ***

      //       red_rec.make_record(_run_id_, _event_id_, reference_time);
      //     }

      //     DT_LOG_DEBUG(_logging_, "Loop on input buffers...");
      //     // Scan the input buffer:
      //     {
      //       DT_LOG_DEBUG(_logging_, "Inspect input buffer");
      //       {
      //         std::lock_guard<std::mutex> lck(*_pimpl_.imtx);
      //         DT_LOG_DEBUG(_logging_, "Input buffer is locked by builder.");
      //         auto & ibuf = _pimpl_.ibuffer;
      //         if (datatools::logger::is_debug(_logging_)) {
      //           ibuf.print(std::cerr);
      //         }
      //              const snfee::io::rtd_record & rec = ibuf.pop_record();

      //         // Extract all RTD records matching the working trigger ID from
      //         the input buffer: while (!ibuf.is_empty()) {
      //           if (ibuf.get_front_trig_id() != rtd_rec.get_trigger_id()) {
      //             break;
      //           }
      //           DT_LOG_DEBUG(_logging_, "Pop record from input buffer #" <<
      //           i); const snfee::io::rhd_record & rec = ibuf.pop_record(); if
      //           (datatools::logger::is_debug(_logging_)) {
      //             rec.print(std::cerr);
      //           }
      //           DT_LOG_DEBUG(_logging_, "Install the RHD record from input
      //           buffer #"
      //                        << i << " into the RTD record...");
      //           rtd_rec.install_rhd(rec);
      //           DT_LOG_DEBUG(_logging_, "RHD record has been inserted in the
      //           current RTD record!");
      //         }
      //         if (datatools::logger::is_debug(_logging_)) {
      //           ibuf.print(std::cerr);
      //         }
      //         DT_LOG_DEBUG(_logging_, "Input buffer #" << i << " is unlocked
      //         by merger.");
      //       }
      //       if (datatools::logger::is_debug(_logging_)) {
      //         rtd_rec.print(std::cerr);
      //       }
      //       DT_LOG_DEBUG(_logging_, "Done with input buffers #" << i);
      //     } // end of scan input buffer.
      //   } // process_input_rhd

      //   // // Manage the completeness of the current RTD for pushing in the
      //   output buffer:
      //   // bool push_current_red = false;
      //   // if (push_current_rtd) {
      //   //   DT_LOG_DEBUG(_logging_,
      //   //                "Current RED record is completed with event ID = "
      //   //                << red_rec.get_trigger_id());
      //   //   {
      //   //     std::lock_guard<std::mutex> olck(*_pimpl_.omtx);
      //   //     DT_LOG_DEBUG(_logging_, "Output buffer is locked by merger.");
      //   //     DT_THROW_IF(_force_complete_rtd_ and
      //   !rtd_rec.get_rtd().is_complete(),
      //   //                 std::logic_error,
      //   //                 "Incomplete RTD data!");
      //   //     _pimpl_.obuffer.push_record(rtd_rec);
      //   //     // We reset the working RTD record and trigger ID:
      //   //     DT_LOG_DEBUG(_logging_, "Reset the working RTD record...");
      //   //     red_rec.reset();
      //   //     _red_records_counter_++;
      //   //   }
      //   //   if (datatools::logger::is_debug(_logging_)) {
      //   //     _pimpl_.obuffer.print(std::cerr);
      //   //   }
      //   //   DT_LOG_DEBUG(_logging_, "Output buffer is unlocked by merger.");
      //   // }

      //   std::this_thread::yield();
      // } // main while loop

      // red_rec.reset();
      // {
      //   std::lock_guard<std::mutex> olck(*_pimpl_.omtx);
      //   DT_LOG_DEBUG(_logging_, "Output buffer is terminated.");
      //   _pimpl_.obuffer.terminate();
      // }

      DT_LOG_NOTICE(_logging_, "Builder one2one algo is done.");
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void
    rtd2red_builder::_run_standard_algo_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      // Working RED record:
      snfee::io::red_record red_rec;

      // std::size_t nin = 1; //_pimpl_.ibuffers.size();
      // bool input_buffer_finished = false;
      // std::set<int> finished_buffers;
      _red_records_counter_ = 0;
      // Main builder loop:
      while (!_stop_request_) {

        // // Extract the next trigger ID from the input buffer:
        // int32_t fetchable_trig_id =
        // get_minimum_trigger_id_from_input_buffer(); bool process_input_rtd =
        // false; if (fetchable_trig_id == snfee::data::INVALID_TRIGGER_ID) {
        //   // None of the input buffers can provide new records yet
        //   // DT_LOG_DEBUG(_logging_, "No fetchable trigger ID is available
        //   yet!");
        // } else {
        //   // DT_LOG_DEBUG(_logging_, "Fetchable trigger ID is : " <<
        //   fetchable_trig_id); process_input_rtd = true;
        // }

        // // Manage the completeness of the current RTD for pushing in the
        // output buffer: bool push_current_rtd = false; if (process_input_rtd)
        // {
        //   if (rtd_rec.get_trigger_id() != snfee::data::INVALID_TRIGGER_ID
        //       and trigger_id_less(rtd_rec.get_trigger_id(),
        //       fetchable_trig_id)) {
        //     // We already have a working RTD record and it has been completed
        //     // (no more records with same trigger ID are expected from the
        //     input buffers),
        //     // so we push it in the output buffer:
        //     DT_LOG_DEBUG(_logging_, "Make the current RTD pushable because
        //     input buffers cannot provide RTD records with trigger ID = " <<
        //     rtd_rec.get_trigger_id() << " anymore."); push_current_rtd =
        //     true;
        //   }
        // } else if (input_buffer_finished) {
        //   DT_LOG_DEBUG(_logging_, "Make the current RTD pushable because this
        //   is the end of the input buffer."); push_current_rtd = true;
        // }

        // if (push_current_rtd) {
        //   DT_LOG_DEBUG(_logging_,
        //                "Current RTD record is completed with trigger ID = "
        //                << rtd_rec.get_trigger_id());
        //   {
        //     std::lock_guard<std::mutex> olck(*_pimpl_.omtx);
        //     DT_LOG_DEBUG(_logging_, "Output buffer is locked by merger.");
        //     DT_THROW_IF(_force_complete_rtd_ and
        //     !rtd_rec.get_rtd().is_complete(),
        //                 std::logic_error,
        //                 "Incomplete RTD data!");
        //     _pimpl_.obuffer.push_record(rtd_rec);
        //     // We reset the working RTD record and trigger ID:
        //     DT_LOG_DEBUG(_logging_, "Reset the working RTD record...");
        //     rtd_rec.reset();
        //     _rtd_records_counter_++;
        //   }
        //   if (datatools::logger::is_debug(_logging_)) {
        //     _pimpl_.obuffer.print(std::cerr);
        //   }
        //   DT_LOG_DEBUG(_logging_, "Output buffer is unlocked by merger.");
        // }

        // if (process_input_rtd) {
        //   // There is some RTD to be processed:
        //   if (rtd_rec.get_trigger_id() == snfee::data::INVALID_TRIGGER_ID) {
        //     // We initiate a new RTD working record to host upcoming RHD
        //     records: DT_LOG_DEBUG(_logging_, "Make a new working RTD record
        //     with trigger ID = "
        //                  << fetchable_trig_id);
        //     rtd_rec.make_record(_run_id_, fetchable_trig_id);
        //   }

        //   DT_LOG_DEBUG(_logging_, "Loop on input buffers...");
        //   // Scan the input buffer:
        //   {
        //     DT_LOG_DEBUG(_logging_, "Inspect input buffer");
        //     {
        //       std::lock_guard<std::mutex> lck(*_pimpl_.imtx);
        //       DT_LOG_DEBUG(_logging_, "Input buffer is locked by builder.");
        //       auto & ibuf = _pimpl_.ibuffer;
        //       if (datatools::logger::is_debug(_logging_)) {
        //         ibuf.print(std::cerr);
        //       }
        //       // Extract all RTD records matching the working trigger ID from
        //       the input buffer: while (!ibuf.is_empty()) {
        //         if (ibuf.get_front_trig_id() != rtd_rec.get_trigger_id()) {
        //           break;
        //         }
        //         DT_LOG_DEBUG(_logging_, "Pop record from input buffer #" <<
        //         i); const snfee::io::rhd_record & rec = ibuf.pop_record(); if
        //         (datatools::logger::is_debug(_logging_)) {
        //           rec.print(std::cerr);
        //         }
        //         DT_LOG_DEBUG(_logging_, "Install the RHD record from input
        //         buffer #"
        //                      << i << " into the RTD record...");
        //         rtd_rec.install_rhd(rec);
        //         DT_LOG_DEBUG(_logging_, "RHD record has been inserted in the
        //         current RTD record!");
        //       }
        //       if (datatools::logger::is_debug(_logging_)) {
        //         ibuf.print(std::cerr);
        //       }
        //       DT_LOG_DEBUG(_logging_, "Input buffer #" << i << " is unlocked
        //       by merger.");
        //     }
        //     if (datatools::logger::is_debug(_logging_)) {
        //       rtd_rec.print(std::cerr);
        //     }
        //     DT_LOG_DEBUG(_logging_, "Done with input buffers #" << i);
        //   } // end of scan input buffer.
        // } // process_input_rhd

        // if (!input_buffer_finished) {
        //   // Count the number of finished input buffer:
        //   std::size_t nfinished = 0;
        //   // bool print_bufs = false;
        //   {
        //     std::lock_guard<std::mutex> lck(*_pimpl_.imtx);
        //     DT_LOG_DEBUG(_logging_, "Input buffer is locked by builder.");
        //     const auto & ibuf = _pimpl_.ibuffer;
        //     if (ibuf.is_finished()) {
        //       nfinished++;
        //       // if (!finished_buffers.count(0)) {
        //       //   finished_buffers.insert(0);
        //       DT_LOG_NOTICE(_logging_, "Input buffer is finished.");
        //       // }
        //     }
        //     DT_LOG_DEBUG(_logging_, "Input buffer is unlocked by builder.");
        //   }
        //   if (nfinished == nin) {
        //     input_buffer_finished = true;
        //     DT_LOG_NOTICE(_logging_, "Input buffer is finished.");
        //   }
        // }
        // if (input_buffer_finished
        //     and rtd_rec.get_trigger_id() == snfee::data::INVALID_TRIGGER_ID)
        //     {
        //   DT_LOG_NOTICE(_logging_, "Input buffer is finished and there is no
        //   more current RTD record. Stop!"); stop();
        // }
        // if ((_rtd_records_counter_ % 100 == 0) or is_stopped()) {
        //   DT_LOG_NOTICE(_logging_, "Builder run : " << _rtd_records_counter_
        //   << " built RED records");
        // }
        std::this_thread::yield();
      } // main while loop

      red_rec.reset();
      {
        std::lock_guard<std::mutex> olck(*_pimpl_.omtx);
        DT_LOG_DEBUG(_logging_, "Output buffer is terminated.");
        _pimpl_.obuffer.terminate();
      }

      DT_LOG_NOTICE(_logging_, "Builder standard algo is done.");
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

  } // namespace redb
} // namespace snfee
