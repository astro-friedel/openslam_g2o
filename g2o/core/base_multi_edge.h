// g2o - General Graph Optimization
// Copyright (C) 2011 R. Kuemmerle, G. Grisetti, H. Strasdat, W. Burgard
// 
// g2o is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// g2o is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef BASE_MULTI_EDGE_H
#define BASE_MULTI_EDGE_H

#include <iostream>
#include <iomanip>
#include <limits>

#include "optimizable_graph.h"

namespace g2o {

  using namespace Eigen;

  /**
   * \brief base class to represent an edge connecting an arbitrary number of nodes
   *
   * D - Dimension of the measurement
   * E - type to represent the measurement
   */
  template <int D, typename E>
  class BaseMultiEdge : public OptimizableGraph::Edge
  {
    public:
      /**
       * \brief helper for mapping the Hessian memory of the upper triangular block
       */
      struct HessianHelper {
        Map<MatrixXd> matrix;     ///< the mapped memory
        bool transposed;          ///< the block has to be transposed
        HessianHelper() : matrix(0, 0, 0), transposed(false) {}
      };

    public:
      static const int Dimension = D;
      typedef E Measurement;
      typedef MatrixXd JacobianType;
      typedef Matrix<double, D, 1> ErrorVector;
      typedef Matrix<double, D, D> InformationType;
      typedef Map<MatrixXd, MatrixXd::Flags & AlignedBit ? Aligned : Unaligned > HessianBlockType;

      BaseMultiEdge() : OptimizableGraph::Edge()
      {
        _dimension = D;
      }

      virtual double chi2() const 
      {
        return _error.dot(information()*_error);
      }

      virtual void robustifyError()
      {
        double nrm = sqrt(_error.dot(information()*_error));
        double w = sqrtOfHuberByNrm(nrm,_huberWidth);
        _error *= w;
      }

      virtual const double* errorData() const { return _error.data();}
      virtual double* errorData() { return _error.data();}
      const ErrorVector& error() const { return _error;}
      ErrorVector& error() { return _error;}

      /**
       * Linearizes the oplus operator in the vertex, and stores
       * the result in temporary variables _jacobianOplusXi and _jacobianOplusXj
       */
      virtual void linearizeOplus();
      
      virtual void resize(size_t size);

      virtual void constructQuadraticForm() ;

      virtual int rank() const {return _dimension;}

      virtual void initialEstimate(const OptimizableGraph::VertexSet& from, OptimizableGraph::Vertex* to);

      virtual void mapHessianMemory(double* d, int i, int j, bool rowMajor);

      const InformationType& information() const { return _information;}
      InformationType& information() { return _information;}
      void setInformation(const InformationType& information) { _information = information;}

      // accessor functions for the measurement represented by the edge
      const Measurement& measurement() const { return _measurement;}
      Measurement& measurement() { return _measurement;}
      void setMeasurement(const Measurement& m) { _measurement = m;}

      const Measurement& inverseMeasurement() const { return _inverseMeasurement;}
      Measurement& inverseMeasurement() { return _inverseMeasurement;}
      void setInverseMeasurement(const Measurement& m) { _measurement = m;}

    protected:
      std::vector<HessianHelper> _hessian;
      ErrorVector _error;
      Measurement _measurement;
      Measurement _inverseMeasurement;
      InformationType _information;
      std::vector<JacobianType> _jacobianOplus; ///< jacobians of the edge (w.r.t. oplus)

    public:
      EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  };

#include "base_multi_edge.hpp"

} // end namespace g2o

#endif