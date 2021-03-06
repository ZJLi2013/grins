//-----------------------------------------------------------------------bl-
//--------------------------------------------------------------------------
// 
// GRINS - General Reacting Incompressible Navier-Stokes 
//
// Copyright (C) 2010-2013 The PECOS Development Team
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the Version 2.1 GNU Lesser General
// Public License as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc. 51 Franklin Street, Fifth Floor,
// Boston, MA  02110-1301  USA
//
//-----------------------------------------------------------------------el-


// This class
#include "grins/heat_conduction.h"

// GRINS
#include "grins/assembly_context.h"
#include "grins/generic_ic_handler.h"
#include "grins/heat_transfer_bc_handling.h"
#include "grins/grins_physics_names.h"

// libMesh
#include "libmesh/quadrature.h"
#include "libmesh/fem_system.h"

namespace GRINS
{

  HeatConduction::HeatConduction( const GRINS::PhysicsName& physics_name, const GetPot& input )
    : Physics(physics_name,input),
      _temp_vars(input,heat_conduction),
      _rho(  input("Physics/"+heat_conduction+"/rho", 1.0) ), /*! \todo same as Incompressible NS */
      _Cp( input("Physics/"+heat_conduction+"/Cp", 1.0) ),
      _k( input("Physics/"+heat_conduction+"/k", 1.0) )
  {
    // This is deleted in the base class
    this->_bc_handler = new HeatTransferBCHandling( physics_name, input );
    this->_ic_handler = new GenericICHandler( physics_name, input );

    return;
  }

  HeatConduction::~HeatConduction()
  {
    return;
  }

  void HeatConduction::init_variables( libMesh::FEMSystem* system )
  {
    // Get libMesh to assign an index for each variable
    this->_dim = system->get_mesh().mesh_dimension();

    _temp_vars.init(system);
    
    return;
  }

  void HeatConduction::set_time_evolving_vars( libMesh::FEMSystem* system )
  {
    // Tell the system to march temperature forward in time
    system->time_evolving(_temp_vars.T_var());

    return;
  }

  void HeatConduction::init_context( AssemblyContext& context )
  {
    // We should prerequest all the data
    // we will need to build the linear system
    // or evaluate a quantity of interest.
    context.get_element_fe(_temp_vars.T_var())->get_JxW();
    context.get_element_fe(_temp_vars.T_var())->get_phi();
    context.get_element_fe(_temp_vars.T_var())->get_dphi();
    context.get_element_fe(_temp_vars.T_var())->get_xyz();

    context.get_side_fe(_temp_vars.T_var())->get_JxW();
    context.get_side_fe(_temp_vars.T_var())->get_phi();
    context.get_side_fe(_temp_vars.T_var())->get_dphi();
    context.get_side_fe(_temp_vars.T_var())->get_xyz();

    return;
  }

  void HeatConduction::element_time_derivative( bool compute_jacobian,
						AssemblyContext& context,
						CachedValues& /*cache*/ )
  {
    // The number of local degrees of freedom in each variable.
    const unsigned int n_T_dofs = context.get_dof_indices(_temp_vars.T_var()).size();

    // We get some references to cell-specific data that
    // will be used to assemble the linear system.

    // Element Jacobian * quadrature weights for interior integration.
    const std::vector<libMesh::Real> &JxW =
      context.get_element_fe(_temp_vars.T_var())->get_JxW();

    // The temperature shape function gradients (in global coords.)
    // at interior quadrature points.
    const std::vector<std::vector<libMesh::RealGradient> >& T_gradphi =
      context.get_element_fe(_temp_vars.T_var())->get_dphi();

    const std::vector<libMesh::Point>& q_points = 
      context.get_element_fe(_temp_vars.T_var())->get_xyz();

    // The subvectors and submatrices we need to fill:
    //
    // K_{\alpha \beta} = R_{\alpha},{\beta} = \partial{ R_{\alpha} } / \partial{ {\beta} } (where R denotes residual)
    // e.g., for \alpha = T and \beta = v we get: K_{Tu} = R_{T},{u}
    //

    libMesh::DenseSubMatrix<Number> &KTT = context.get_elem_jacobian(_temp_vars.T_var(), _temp_vars.T_var()); // R_{T},{T}

    libMesh::DenseSubVector<Number> &FT = context.get_elem_residual(_temp_vars.T_var()); // R_{T}

    // Now we will build the element Jacobian and residual.
    // Constructing the residual requires the solution and its
    // gradient from the previous timestep.  This must be
    // calculated at each quadrature point by summing the
    // solution degree-of-freedom values by the appropriate
    // weight functions.
    unsigned int n_qpoints = context.get_element_qrule().n_points();

    for (unsigned int qp=0; qp != n_qpoints; qp++)
      {
	// Compute the solution & its gradient at the old Newton iterate.
	libMesh::Gradient grad_T;
	grad_T = context.interior_gradient(_temp_vars.T_var(), qp);

	const Real f = this->forcing( q_points[qp] );
	
	// First, an i-loop over the  degrees of freedom.
	for (unsigned int i=0; i != n_T_dofs; i++)
	  {
	    FT(i) += JxW[qp] *
	      (-_k*(T_gradphi[i][qp]*grad_T) + f);  // diffusion term

	    if (compute_jacobian)
	      {
		for (unsigned int j=0; j != n_T_dofs; j++)
		  {
		    // TODO: precompute some terms like:
		    //   _rho*_Cp*T_phi[i][qp]*(vel_phi[j][qp]*T_grad_phi[j][qp])

		    KTT(i,j) += JxW[qp] *
		      ( -_k*(T_gradphi[i][qp]*T_gradphi[j][qp]) ); // diffusion term
		  } // end of the inner dof (j) loop

	      } // end - if (compute_jacobian && context.get_elem_solution_derivative())

	  } // end of the outer dof (i) loop
      } // end of the quadrature point (qp) loop

    return;
  }

  void HeatConduction::mass_residual( bool compute_jacobian,
				      AssemblyContext& context,
				      CachedValues& /*cache*/ )
  {
    // First we get some references to cell-specific data that
    // will be used to assemble the linear system.

    // Element Jacobian * quadrature weights for interior integration
    const std::vector<Real> &JxW = 
      context.get_element_fe(_temp_vars.T_var())->get_JxW();

    // The shape functions at interior quadrature points.
    const std::vector<std::vector<Real> >& phi = 
      context.get_element_fe(_temp_vars.T_var())->get_phi();

    // The number of local degrees of freedom in each variable
    const unsigned int n_T_dofs = context.get_dof_indices(_temp_vars.T_var()).size();

    // The subvectors and submatrices we need to fill:
    DenseSubVector<Real> &F = context.get_elem_residual(_temp_vars.T_var());

    DenseSubMatrix<Real> &M = context.get_elem_jacobian(_temp_vars.T_var(), _temp_vars.T_var());

    unsigned int n_qpoints = context.get_element_qrule().n_points();

    for (unsigned int qp = 0; qp != n_qpoints; ++qp)
      {
	// For the mass residual, we need to be a little careful.
	// The time integrator is handling the time-discretization
	// for us so we need to supply M(u_fixed)*u for the residual.
	// u_fixed will be given by the fixed_interior_* functions
	// while u will be given by the interior_* functions.
        Real T_dot = context.interior_value(_temp_vars.T_var(), qp);

	for (unsigned int i = 0; i != n_T_dofs; ++i)
	  {
	    F(i) += JxW[qp]*(_rho*_Cp*T_dot*phi[i][qp] );

	    if( compute_jacobian )
              {
                for (unsigned int j=0; j != n_T_dofs; j++)
                  {
		    // We're assuming rho, cp are constant w.r.t. T here.
                    M(i,j) += JxW[qp]*_rho*_Cp*phi[j][qp]*phi[i][qp] ;
                  }
              }// End of check on Jacobian
          
	  } // End of element dof loop
      
      } // End of the quadrature point loop

    return;
  }

  inline
  Real HeatConduction::forcing( const libMesh::Point& p )
  {
    const Real x = p(0);
    const Real y = p(1);
    const Real z = p(2);

    return std::cos(.5*pi*x)*std::sin(.5*pi*y)*std::cos(.5*pi*z);
  }

} // namespace GRINS
