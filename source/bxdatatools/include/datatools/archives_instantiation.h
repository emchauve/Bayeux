/* -*- mode: c++; -*- */
/* datatools::archives_instantiation.h */
/*
 * Description :
 *
 * Define some macro to instantiate serialization methods for all archives
 * supported by datatools.
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */
#ifndef DATATOOLS_ARCHIVES_INSTANTIATION_H_
#define DATATOOLS_ARCHIVES_INSTANTIATION_H_

// Standard Library

// Third Party

// Datatools
#include <datatools/archives_list.h>


// Intrusive version :
#define DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,A) \
template void T::serialize(A& ar, const unsigned int version);  \
/**/

#define DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL_IN(T)   \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::polymorphic_iarchive) \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::text_iarchive) \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::xml_iarchive) \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,eos::portable_iarchive) \
  /**/

#define DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL_OUT(T)  \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::polymorphic_oarchive) \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::text_oarchive) \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::xml_oarchive) \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,eos::portable_oarchive) \
  /**/

#define DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(T) \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL_IN(T) \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL_OUT(T) \
  /**/

// Non-intrusive version :
#define DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE(T,A) \
template void boost::serialization::serialize(A& ar, T& object, const unsigned int version); \
/**/

#define DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL_IN(T) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::polymorphic_iarchive) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::text_iarchive) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::xml_iarchive) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE(T,eos::portable_iarchive) \
  /**/

#define DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL_OUT(T) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::polymorphic_oarchive) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::text_oarchive) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::xml_oarchive) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE(T,eos::portable_oarchive) \
  /**/

#define DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL(T) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL_IN(T) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL_OUT(T) \
  /**/

#endif // DATATOOLS_ARCHIVES_INSTANTIATION_H_

