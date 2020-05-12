/*
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * Copyright (c) 2020 Andrey Semashev
 */
/*!
 * \file   atomic/atomic_ref.hpp
 *
 * This header contains definition of \c atomic_ref template.
 */

#ifndef BOOST_ATOMIC_ATOMIC_REF_HPP_INCLUDED_
#define BOOST_ATOMIC_ATOMIC_REF_HPP_INCLUDED_

#include <boost/atomic/capabilities.hpp>
#include <boost/atomic/detail/atomic_ref_template.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

using atomics::atomic_ref;

} // namespace boost

#endif // BOOST_ATOMIC_ATOMIC_REF_HPP_INCLUDED_
