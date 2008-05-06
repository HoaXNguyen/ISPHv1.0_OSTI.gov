// @HEADER
// ***********************************************************************
// 
// RTOp: Interfaces and Support Software for Vector Reduction Transformation
//       Operations
//                Copyright (2006) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//  
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Roscoe A. Bartlett (rabartl@sandia.gov) 
// 
// ***********************************************************************
// @HEADER


#ifndef RTOPPACK_TYPES_HPP
#define RTOPPACK_TYPES_HPP


#include "RTOp_ConfigDefs.hpp"
#include "Teuchos_Array.hpp"
#include "Teuchos_ArrayRCP.hpp"
#include "Teuchos_TestForException.hpp"


namespace RTOpPack {


//
// Basic types
//

/** \brief . */
typedef Teuchos_Index Index;
/** \brief . */
using Teuchos::Ptr;
/** \brief . */
using Teuchos::RCP;
/** \brief . */
using Teuchos::ArrayRCP;
/** \brief . */
using Teuchos::ArrayView;
/** \brief . */
using Teuchos::Array;

/** \brief Depreciated. */
typedef Teuchos_Index index_type;
/** \brief Depreciated. */
typedef char  char_type;


//
// Exceptions
//


/** \brief . */
class UnknownError : public std::logic_error
{public: UnknownError(const std::string& what_arg) : std::logic_error(what_arg) {}};
/** \brief . */
class InvalidUsage : public std::logic_error
{public: InvalidUsage(const std::string& what_arg) : std::logic_error(what_arg) {}};
/** \brief . */
class InvalidNumVecs : public std::logic_error
{public: InvalidNumVecs(const std::string& what_arg) : std::logic_error(what_arg) {}};
/** \brief . */
class InvalidNumTargVecs : public std::logic_error
{public: InvalidNumTargVecs(const std::string& what_arg) : std::logic_error(what_arg) {}};
/** \brief . */
class IncompatibleVecs : public std::logic_error
{public: IncompatibleVecs(const std::string& what_arg) : std::logic_error(what_arg) {}};


//
// VectorBase subviews
//


/** \brief Class for a non-changeable sub-vector.
 *
 * For a sub-vector <tt>vec</tt>, the corresponding entries in the global
 * vector <tt>x(j)</tt> (one based) are as follows:

 \verbatim

   x( vec.globalOffset() + k ) = v(k), for k = 0...vec.subDim()-1

 \endverbatim

 * The stride <tt>vec.stride()</tt> may be positive (>0) or negative (<0) but
 * not zero (0).  A negative stride <tt>vec.stride() < 0</tt> allows a reverse
 * traversal of the elements.
 *
 * <b>WARNING!</b> the default copy constructor and assignment operators are
 * allowed which results in only pointer copy, not deep copy!  You have been
 * warned!
 */
template<class Scalar>
class ConstSubVectorView {
public:
  /** \brief . */
  ConstSubVectorView() : globalOffset_(0), subDim_(0), stride_(0) {}
  /** \brief . */
  ConstSubVectorView(Teuchos_Index globalOffset, Teuchos_Index subDim,
    const ArrayRCP<const Scalar> &values, ptrdiff_t stride)
    :globalOffset_(0), subDim_(0), stride_(0)
    { initialize(globalOffset, subDim,values, stride); }
  /** \brief . */
  ConstSubVectorView( const ConstSubVectorView<Scalar>& sv )
    :globalOffset_(sv.globalOffset()), subDim_(sv.subDim()),
     values_(sv.values()), stride_(sv.stride()) 
    {}
  /** \brief . */
  void initialize(Teuchos_Index globalOffset, Teuchos_Index subDim,
    const ArrayRCP<const Scalar> &values, ptrdiff_t stride)
    {
#ifdef TEUCHOS_DEBUG
      TEUCHOS_ASSERT(globalOffset >= 0);
      TEUCHOS_ASSERT(subDim >= 0);
      TEUCHOS_ASSERT(stride != 0);
      TEUCHOS_ASSERT(subDim*std::abs(stride) - 1 <= values.upperOffset());
      TEUCHOS_ASSERT(values.lowerOffset() <= 0);
#endif
      globalOffset_=globalOffset;
      subDim_=subDim;
      values_=values;
      stride_=stride;
    }
  /** \brief . */
  void uninitialize()
    { globalOffset_ = 0; subDim_=0; values_ = Teuchos::null; stride_ = 0; }
  /** \brief . */
  void setGlobalOffset(Teuchos_Index globalOffset)
    {
#ifdef TEUCHOS_DEBUG
      TEUCHOS_ASSERT(globalOffset >= 0);
#endif
      globalOffset_ = globalOffset;
    } 
  /** \brief . */
  Teuchos_Index globalOffset() const { return globalOffset_; }
  /** \brief . */
  Teuchos_Index subDim() const { return subDim_; }
  /** \brief . */
  const ArrayRCP<const Scalar>  values() const { return values_; }
  /** \brief . */
  ptrdiff_t stride() const { return stride_; }
  /** \brief Zero-based indexing (Preconditions: <tt>values()!=NULL && (0 <= i
   * < subDim())</tt>). */
  const Scalar& operator[](Teuchos_Index i) const
    {
#ifdef TEUCHOS_DEBUG
      TEUCHOS_ASSERT_IN_RANGE_UPPER_EXCLUSIVE(i, 0, subDim_);
#endif
      return valuesBegin()[stride_*i];
    }
  /** \brief Zero-based indexing (Preconditions: <tt>values()!=NULL && (0 <= i
   * < subDim())</tt>). */
  const Scalar& operator()(Teuchos_Index i) const { return (*this)[i]; }
private:
  Teuchos_Index globalOffset_;
  Teuchos_Index subDim_;
  ArrayRCP<const Scalar> values_;
  ptrdiff_t stride_;
  const typename ArrayRCP<const Scalar>::iterator valuesBegin() const
    {
      if (stride_ > 0)
        return values_.begin();
      return values_.begin() + (subDim_*std::abs(stride_) - 1);
    } 
public:
  /** \brief Deprecated. */
  ConstSubVectorView(Teuchos_Index globalOffset, Teuchos_Index subDim,
    const Scalar values[], ptrdiff_t stride)
    :globalOffset_(globalOffset), subDim_(subDim),
     values_(values,0,subDim*stride,false), stride_(stride) 
    {}
  /** \brief Deprecated. */
  void initialize(Teuchos_Index globalOffset, Teuchos_Index subDim,
    const Scalar values[], ptrdiff_t stride)
    {
      globalOffset_=globalOffset; subDim_=subDim;
      values_=Teuchos::arcp(values,0,subDim*std::abs(stride),false);
      stride_=stride;
    }
  /** \brief Deprecated. */
  void set_uninitialized()
    { uninitialize(); }
};


/** \brief Class for a changeable sub-vector.
 *
 * This class derives from <tt>ConstSubVectorView</tt> and adds methods to
 * change the data.  Note, a <tt>const SubVectorView</tt> object allows
 * clients to change the values in the underlying subvector.  The meaning of
 * <tt>const</tt> in this context is that the view of the data can not change.
 *
 * <b>WARNING!</b> the default copy constructor and assignment operators are
 * allowed which results in only pointer copy, not deep copy!  You have been
 * warned!
 *
 * NOTE: It is perfectly safe to derive this class from ConstSubVectorView
 * even through it does not have a virtual destructor.  That is because this
 * derived class has no data members that would cause problems in slicing or
 * memory leaks when deleting.
 */
template<class Scalar>
class SubVectorView : public ConstSubVectorView<Scalar> {
public:
  /** \brief . */
  SubVectorView() {}
  /** \brief . */
  SubVectorView(Teuchos_Index globalOffset, Teuchos_Index subDim,
    const ArrayRCP<Scalar> &values, ptrdiff_t stride)
    :ConstSubVectorView<Scalar>(globalOffset, subDim, values, stride)
    {}
  /** \brief . */
  SubVectorView(Teuchos_Index subDim)
    :ConstSubVectorView<Scalar>(0, subDim, Teuchos::arcp<Scalar>(subDim), 1)
    {}
  /** \brief . */
  SubVectorView(const SubVectorView<Scalar> & sv)
    :ConstSubVectorView<Scalar>(sv)
    {}
  /** \brief . */
  void initialize(Teuchos_Index globalOffset, Teuchos_Index subDim,
    const ArrayRCP<Scalar> &values, ptrdiff_t stride)
    { ConstSubVectorView<Scalar>::initialize(globalOffset, subDim, values, stride); }
  /** \brief . */
  const ArrayRCP<Scalar> values() const
    { return Teuchos::arcp_const_cast<Scalar>(ConstSubVectorView<Scalar>::values());  }
  /** \brief Zero-based indexing (Preconditions: <tt>values()!=NULL && (0 <= i
   * < subDim())</tt>). */
  Scalar& operator[](Teuchos_Index i) const
    { return const_cast<Scalar&>(ConstSubVectorView<Scalar>::operator[](i)); }
  /** \brief Zero-based indexing (Preconditions: <tt>values()!=NULL && (0 <= i
   * < subDim())</tt>). */
  Scalar& operator()(Teuchos_Index i) const { return (*this)[i]; }
public:
  /** \brief Deprecated. */
  SubVectorView(Teuchos_Index globalOffset, Teuchos_Index subDim,
    Scalar values[], ptrdiff_t stride)
    :ConstSubVectorView<Scalar>(globalOffset, subDim, values, stride)
    {}
  /** \brief Deprecated. */
  void initialize(Teuchos_Index globalOffset, Teuchos_Index subDim,
    Scalar values[], ptrdiff_t stride)
    { ConstSubVectorView<Scalar>::initialize(globalOffset, subDim, values, stride); }
};


/** \brief . */
template<class Scalar>
void assign_entries( const Ptr<const SubVectorView<Scalar> > &msv,
  const ConstSubVectorView<Scalar> &sv )
{
#ifdef TEUCHOS_DEBUG
  TEUCHOS_ASSERT_EQUALITY(msv->subDim(), sv.subDim());
#endif
  for( int i = 0; i < sv.subDim(); ++i ) {
    (*msv)(i) = sv(i);
  }
}


//
// MultiVectorBase subviews
//


/** \brief Class for a non-changeable sub-multi-vector (submatrix).
 *
 * For a sub-multi-vector <tt>mv</tt>, the corresponding entries in the global
 * multi-vector <tt>X(j)</tt> (one based) are as follows:

 \verbatim

   X(mv.globalOffset()+k1,mv.colOffset()+k2) = mv(k1,k2),
       for k1 = 0...mv.subDim()-1, k2 = 0...mv.numSubCols()-1

 \endverbatim

 * Unlike vectors, there can only be a unit stride between vector elements in
 * a particular column and there is a Fortran-like leading dimension
 * <tt>mv.leadingDim()</tt> that separates corresponding elements in each
 * column sub-vector.
 *
 * <b>WARNING!</b> the default copy constructor and assignment operators are
 * allowed which results in only pointer copy, not deep copy!  You have been
 * warned!
 */
template<class Scalar>
class ConstSubMultiVectorView {
public:
  /** \brief . */
  ConstSubMultiVectorView()
    :globalOffset_(0), subDim_(0), colOffset_(0), numSubCols_(0),
     leadingDim_(0)
    {}
  /** \brief . */
  ConstSubMultiVectorView(
    Teuchos_Index globalOffset, Teuchos_Index subDim,
    Teuchos_Index colOffset, Teuchos_Index numSubCols,
    const ArrayRCP<const Scalar> &values, Teuchos_Index leadingDim
    )
    :globalOffset_(0), subDim_(0), colOffset_(0), numSubCols_(0),
     leadingDim_(0)
    {
      initialize(globalOffset, subDim, colOffset, numSubCols, values,
        leadingDim);
    }
  /** \brief . */
  ConstSubMultiVectorView( const ConstSubMultiVectorView<Scalar>& smv )
    :globalOffset_(smv.globalOffset()), subDim_(smv.subDim()),
     colOffset_(smv.colOffset()), numSubCols_(smv.numSubCols()),
     values_(smv.values()), leadingDim_(smv.leadingDim())
    {}
  /** \brief . */
  void initialize(
    Teuchos_Index globalOffset, Teuchos_Index subDim,
    Teuchos_Index colOffset, Teuchos_Index numSubCols,
    const ArrayRCP<const Scalar> &values, Teuchos_Index leadingDim
    )
    {
#ifdef TEUCHOS_DEBUG
      TEUCHOS_ASSERT(globalOffset >= 0);
      TEUCHOS_ASSERT(subDim >= 0);
      TEUCHOS_ASSERT(colOffset >= 0);
      TEUCHOS_ASSERT(leadingDim >= subDim);
      TEUCHOS_ASSERT(numSubCols*leadingDim - 1 <= values.upperOffset());
      TEUCHOS_ASSERT(values.lowerOffset() <= 0);
#endif
      globalOffset_=globalOffset;
      subDim_=subDim;
      colOffset_=colOffset;
      numSubCols_=numSubCols;
      values_=values;
      leadingDim_=leadingDim;
    }
  /** \brief . */
  void uninitialize()
    {
      globalOffset_ = 0; subDim_=0; colOffset_=0, numSubCols_=0;
      values_=Teuchos::null; leadingDim_=0;
    }
  /** \brief . */
  void setGlobalOffset(Teuchos_Index globalOffset)
    {
#ifdef TEUCHOS_DEBUG
      TEUCHOS_ASSERT(globalOffset >= 0);
#endif
      globalOffset_ = globalOffset;
    } 
  /** \brief . */
  Teuchos_Index globalOffset() const { return globalOffset_; }
  /** \brief . */
  Teuchos_Index subDim() const { return subDim_; }
  /** \brief . */
  Teuchos_Index colOffset() const { return colOffset_; }
  /** \brief . */
  Teuchos_Index numSubCols() const { return numSubCols_; }
  /** \brief . */
  const ArrayRCP<const Scalar> values() const { return values_; }
  /** \brief . */
  Teuchos_Index leadingDim() const { return leadingDim_; }
  /** \brief Zero-based indexing (Preconditions: <tt>values()!=NULL &&
   * (0<=i<subDim()) && (0<=j< numSubCols()</tt>).
   */
  const Scalar& operator()(Teuchos_Index i, Teuchos_Index j) const
    {
#ifdef TEUCHOS_DEBUG
      TEUCHOS_ASSERT_IN_RANGE_UPPER_EXCLUSIVE(i, 0, subDim_);
      TEUCHOS_ASSERT_IN_RANGE_UPPER_EXCLUSIVE(j, 0, numSubCols_ );
#endif
      return values_[ i + leadingDim_*j ];
    }
  /** \brief Return a <tt>ConstSubVectorView</tt> view of the jth sub-column
   * (Preconditions: <tt>values()!=NULL && (0<=j<numSubCols()</tt>).
   */
  ConstSubVectorView<Scalar> col( const Teuchos_Index j ) const
    {
#ifdef TEUCHOS_DEBUG
      TEUCHOS_ASSERT_IN_RANGE_UPPER_EXCLUSIVE(j, 0, numSubCols_ );
#endif
      return ConstSubVectorView<Scalar>(
        globalOffset(), subDim(), values().persistingView(j*leadingDim(),subDim()), 1 );
    }
private:
  Teuchos_Index globalOffset_;
  Teuchos_Index subDim_;
  Teuchos_Index colOffset_;
  Teuchos_Index numSubCols_;
  ArrayRCP<const Scalar> values_;
  Teuchos_Index leadingDim_;
public:
  /** \brief Deprecated. */
  ConstSubMultiVectorView(
    Teuchos_Index globalOffset, Teuchos_Index subDim,
    Teuchos_Index colOffset, Teuchos_Index numSubCols,
    const Scalar values[], Teuchos_Index leadingDim
    )
    :globalOffset_(globalOffset), subDim_(subDim),
     colOffset_(colOffset), numSubCols_(numSubCols),
     values_(values,0,numSubCols*leadingDim,false),
     leadingDim_(leadingDim)
    {}
  /** \brief Deprecated. */
  void initialize(
    Teuchos_Index globalOffset, Teuchos_Index subDim,
    Teuchos_Index colOffset, Teuchos_Index numSubCols,
    const Scalar values[], Teuchos_Index leadingDim
    )
    {
      globalOffset_=globalOffset; subDim_=subDim; colOffset_=colOffset;
      numSubCols_=numSubCols;
      values_=Teuchos::arcp(values,0,numSubCols*leadingDim,false);
      leadingDim_=leadingDim;
    }
  /** \brief Deprecated. */
  void set_uninitialized()
    { uninitialize(); }
};


/** \brief Class for a changeable sub-vector.
 *
 * This class derives from <tt>ConstSubVectorView</tt> and adds methods to
 * change the data.  Note, a <tt>const SubVectorView</tt> object allows
 * clients to change the values in the underlying subvector.  The meaning of
 * <tt>const</tt> in this context is that the view of the data can not change.
 *
 * <b>WARNING!</b> the default copy constructor and assignment operators are
 * allowed which results in only pointer copy, not deep copy!  You have been
 * warned!
 *
 * NOTE: It is perfectly safe to derive this class from
 * ConstSubMultiVectorView even through it does not have a virtual destructor.
 * That is because this derived class has no data members that would cause
 * problems in slicing or memory leaks when deleting.
 */
template<class Scalar>
class SubMultiVectorView : public ConstSubMultiVectorView<Scalar> {
public:
  /** \brief . */
  SubMultiVectorView() {}
  /** \brief . */
  SubMultiVectorView(
    Teuchos_Index numRows, Teuchos_Index numCols
    )
    :ConstSubMultiVectorView<Scalar>(0, numRows, 0, numCols,
      Teuchos::arcp<Scalar>(numRows*numCols), numRows)
    {}
  /** \brief . */
  SubMultiVectorView(
    Teuchos_Index globalOffset, Teuchos_Index subDim,
    Teuchos_Index colOffset, Teuchos_Index numSubCols,
    const ArrayRCP<Scalar> &values, Teuchos_Index leadingDim
    )
    :ConstSubMultiVectorView<Scalar>(globalOffset,subDim,colOffset,numSubCols,
      values,leadingDim)
    {}
  /** \brief . */
  SubMultiVectorView( const SubMultiVectorView<Scalar> & smv)
    :ConstSubMultiVectorView<Scalar>(smv)
    {}
  /** \brief . */
 void initialize(
   Teuchos_Index globalOffset, Teuchos_Index subDim,
   Teuchos_Index colOffset, Teuchos_Index numSubCols,
   const ArrayRCP<Scalar> &values, Teuchos_Index leadingDim
   )
   {
     ConstSubMultiVectorView<Scalar>::initialize(globalOffset,subDim,
       colOffset,numSubCols,values,leadingDim);
   }
  /** \brief . */
  const ArrayRCP<Scalar> values() const
    {
      return Teuchos::arcp_const_cast<Scalar>(
        ConstSubMultiVectorView<Scalar>::values());
    }
  /** \brief Zero-based indexing (Preconditions: <tt>values()!=NULL && (0<=i<
   * subDim()) && (0<=j<numSubCols()</tt>).
   */
  Scalar& operator()(Teuchos_Index i, Teuchos_Index j) const
    { return const_cast<Scalar&>(ConstSubMultiVectorView<Scalar>::operator()(i,j)); }
  /** \brief Return a <tt>SubVectorView</tt> view of the jth sub-column
   * (Preconditions: <tt>values()!=NULL && && (0<=j<numSubCols()</tt>).
   */
  SubVectorView<Scalar> col( const Teuchos_Index j ) const
    {
#ifdef TEUCHOS_DEBUG
      TEUCHOS_ASSERT_IN_RANGE_UPPER_EXCLUSIVE(j, 0, this->numSubCols());
#endif
      return SubVectorView<Scalar>(this->globalOffset(), this->subDim(),
        values().persistingView(j*this->leadingDim(),this->subDim()), 1);
    }
public:
  /** \brief Deprecated. */
  SubMultiVectorView(
    Teuchos_Index globalOffset, Teuchos_Index subDim,
    Teuchos_Index colOffset, Teuchos_Index numSubCols,
    Scalar values[], Teuchos_Index leadingDim
    )
    :ConstSubMultiVectorView<Scalar>(globalOffset,subDim,colOffset,numSubCols,
      values,leadingDim)
    {}
  /** \brief Deprecated. */
 void initialize(
   Teuchos_Index globalOffset, Teuchos_Index subDim,
   Teuchos_Index colOffset, Teuchos_Index numSubCols,
   Scalar values[], Teuchos_Index leadingDim
   )
   {
     ConstSubMultiVectorView<Scalar>::initialize(globalOffset,subDim,
       colOffset,numSubCols,values,leadingDim);
   }
};


/** \brief . */
template<class Scalar>
void assign_entries( const Ptr<const SubMultiVectorView<Scalar> > &msmv,
  const ConstSubMultiVectorView<Scalar> &smv )
{
#ifdef TEUCHOS_DEBUG
  TEUCHOS_ASSERT_EQUALITY(msmv->subDim(), smv.subDim());
  TEUCHOS_ASSERT_EQUALITY(msmv->numSubCols(), smv.numSubCols());
#endif
  for( Teuchos_Index j = 0; j < smv.numSubCols(); ++j ) {
    for( Teuchos_Index i = 0; i < smv.subDim(); ++i ) {
      (*msmv)(i,j) = smv(i,j);
    }
  }
}


//
// Templated types
//


/** \brief . */
template<class Scalar>  class RTOpT;


} // namespace RTOpPack


#endif // RTOPPACK_TYPES_HPP
