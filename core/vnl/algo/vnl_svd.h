#ifndef vnl_svd_h_
#define vnl_svd_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
//  \file
//  \brief Holds the singular value decomposition of a vnl_matrix.
//  \author Andrew W. Fitzgibbon, Oxford IERG
//  \date   15 Jul 96
//
// \verbatim
//  Modifications
// F. Schaffalitzky, Oxford IESRG, 26 Mar 1999
//     1. The singular values are now stored as reals (not complexes) when T is complex.
//     2. Fixed bug : for complex T, matrices have to be conjugated as well as transposed.
// \endverbatim

#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_diag_matrix.h>
#include <vcl_iosfwd.h>

//: Holds the singular value decomposition of a vnl_matrix.
//
//  The class holds three matrices U, W, V such that the original matrix
//  \f$ M = U W V^\top\f$ .  The DiagMatrix W stores the singular values in decreasing
//  order.  The columns of U which correspond to the nonzero singular values
//  form a basis for range of M, while the columns of V corresponding to the
//  zero singular values are the nullspace.
//
//  The SVD is computed at construction time, and enquiries may then be made
//  of the SVD.  In particular, this allows easy access to multiple
//  right-hand-side solves without the bother of putting all the RHS's into a
//  Matrix.
//
//  This class is supplied even though there is an existing vnl_matrix method
//  for several reasons:
//
//  It is more convenient to use as it manages all the storage for
//  the U,S,V matrices, allowing repeated queries of the same SVD
//  results.
//
//  It avoids namespace clutter in the Matrix class.   While svd()
//  is a perfectly reasonable method for a Matrix, there are many other
//  decompositions that might be of interest, and adding them all would
//  make for a very large Matrix class.
//
//  It demonstrates the holder model of compute class, implementing an
//  algorithm on an object without adding a member that may not be of
//  general interest.  A similar pattern can be used for other
//  decompositions which are not defined as members of the library Matrix
//  class.
//
//  It extends readily to n-ary operations, such as generalized
//  eigensystems, which cannot be members of just one matrix.

export template <class T>
class vnl_svd {
public:
  // The singular values of a matrix of complex<T> are of type T,not complex<T>
  typedef typename vnl_numeric_traits<T>::abs_t singval_t;

//:
// Construct an vnl_svd<T> object from \f$m \times n\f$ matrix \f$M\f$.  The
// vnl_svd<T> object contains matrices $U$, $W$, $V$ such that
// \f$U W V^\top = M\f$.
//
// Uses linpack routine DSVDC to calculate an ``economy-size'' SVD
// where the returned \f$U\f$ is the same size as \f$M\f$, while \f$W\f$
// and \f$V\f$ are both \f$n \times n\f$.  This is efficient for
// large rectangular solves where \f$m > n\f$, typical in least squares.
//
// The optional argument zero_out_tol is used to mark the zero singular
// values: If nonnegative, any s.v. smaller than zero_out_tol in
// absolute value is set to zero.  If zero_out_tol is negative, the
// zeroing is relative to |zero_out_tol| * sigma_max();

  vnl_svd(vnl_matrix<T> const &M, double zero_out_tol = 0.0);
 ~vnl_svd() {}

  // Data Access---------------------------------------------------------------
  void            zero_out_absolute(double tol = 1e-8); //sqrt(machine epsilon)
  void            zero_out_relative(double tol = 1e-8); //sqrt(machine epsilon)
  int             singularities () const { return W_.n() - rank(); }
  int             rank () const { return rank_; }
  singval_t       well_condition () const { return sigma_min()/sigma_max(); }
  singval_t       determinant_magnitude () const;
  singval_t       norm() const;

//: Return the matrix U and the (i,j)th entry (to avoid svd.U()(i,j); ).
  vnl_matrix<T>      & U()       { return U_; }
  vnl_matrix<T> const& U() const { return U_; }
  T U(int i, int j) { return U_(i,j); }

//: Get at DiagMatrix (q.v.) of singular values,sorted from largest to smallest
  vnl_diag_matrix<singval_t>       & W()             { return W_; }
//: Get at DiagMatrix (q.v.) of singular values,sorted from largest to smallest
  vnl_diag_matrix<singval_t> const & W() const       { return W_; }
  vnl_diag_matrix<singval_t>       & Winverse()      { return Winverse_; }
  vnl_diag_matrix<singval_t> const & Winverse() const { return Winverse_; }
  singval_t                   & W(int i, int j) { return W_(i,j); }
  singval_t                   & W(int i)        { return W_(i,i); }
  singval_t     sigma_max() const { return W_(0,0); }       // largest
  singval_t     sigma_min() const { return W_(n_-1,n_-1); } // smallest

//: Return the matrix V.
  vnl_matrix<T>      & V()       { return V_; }
  vnl_matrix<T> const& V() const { return V_; }
  T V(int i, int j) { return V_(i,j); }

  //
  vnl_matrix<T> inverse () const;
  vnl_matrix<T> pinverse () const; // pseudo-inverse (for non-square matrix).
  vnl_matrix<T> tinverse () const;
  vnl_matrix<T> recompose () const;

  //
  vnl_matrix<T> solve (vnl_matrix<T> const& rhs) const;
  vnl_vector<T> solve (vnl_vector<T> const& rhs) const;
  void          solve (T const *rhs, T *lhs) const; // min ||A*lhs - rhs||
  void solve_preinverted(vnl_vector<T> const& rhs, vnl_vector<T>* out) const;

  //
  vnl_matrix<T> nullspace() const;
  vnl_matrix<T> left_nullspace() const;

  vnl_matrix<T> nullspace(int required_nullspace_dimension) const;
  vnl_matrix<T> left_nullspace(int required_nullspace_dimension) const;

  vnl_vector<T> nullvector() const;
  vnl_vector<T> left_nullvector() const;

private:

  int m_, n_;              // Size of M, local cache.
  vnl_matrix<T> U_;        // Columns Ui are basis for range of M for Wi != 0
  vnl_diag_matrix<singval_t> W_;// Singular values, sorted in decreasing order
  vnl_diag_matrix<singval_t> Winverse_;
  vnl_matrix<T> V_;       // Columns Vi are basis for nullspace of M for Wi = 0
  unsigned rank_;
  bool have_max_;
  singval_t max_;
  bool have_min_;
  singval_t min_;
  double last_tol_;

  // Disallow assignment.
  vnl_svd(vnl_svd<T> const &) { }
  vnl_svd<T>& operator=(vnl_svd<T> const &) { return *this; }
};

template <class T>
inline
vnl_matrix<T> vnl_svd_inverse(vnl_matrix<T> const& m)
{
  return vnl_svd<T>(m).inverse();
}

// this aint no friend.
export template <class T>
vcl_ostream& operator<<(vcl_ostream&, vnl_svd<T> const& svd);

#endif // vnl_svd_h_
