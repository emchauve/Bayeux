/* brio_record.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-07-16
 * Last modified: 2010-07-16
 * 
 * License: 
 * 
 * Description: 
 *
 * rootcint brioDict.C -c include/brio/brio_record.h
 *
 * History: 
 * 
 */
#ifndef BRIO_DETAIL_BRIO_RECORD_H_
#define BRIO_DETAIL_BRIO_RECORD_H_ 1

// Standard Library
#include <vector>

// Third Party
// - ROOT
#include <TObject.h>
#include <TBuffer.h>
#include <TString.h>

// This Project
#include <brio/detail/TArrayCMod.h>

/// The internal ROOT I/O record class
class brio_record : public TObject {
 public:   
  TString    fSerialTag;  /// The serialization tag of the data class
  UInt_t     fVersionTag; /// The serialization version number of the data class
  TArrayCMod fDataBuffer; /// The buffer of bytes that contains the Boost archive associated to the serialized data

 public: 
  //! Constructor
  brio_record();

  //! Destructor
  virtual ~brio_record();
 
  //! Reset the internal data
  void reset();
 
  ClassDef(brio_record,1) //Basic BRIO record
};

#endif // BRIO_DETAIL_BRIO_RECORD_H_

// end of brio_record.h
