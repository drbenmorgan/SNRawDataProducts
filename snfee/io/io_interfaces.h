//! \file snfee/io/io_interfaces.h
//! \brief I/O interfaces
//
// Copyright (c) 2019 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef SNFEE_IO_IO_INTERFACES_H
#define SNFEE_IO_IO_INTERFACES_H

// Standard library:
#include <iostream>
#include <string>

namespace snfee {
  namespace io {

    //! \brief Data source
    template <class T>
    struct data_source_interface {
      typedef T data_type;
      virtual bool can_pop() const = 0;
      virtual void pop(data_type&) = 0;
      bool
      is_terminated() const
      {
        return !can_pop();
      }
    };

    //! \brief Data sink
    template <class T>
    struct data_sink_interface {
      typedef T data_type;
      virtual bool can_push() const = 0;
      virtual void push(const data_type&) = 0;
      bool
      is_terminated() const
      {
        return !can_push();
      }
    };

  } // namespace io
} // namespace snfee

#endif // SNFEE_IO_IO_INTERFACES_H

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
