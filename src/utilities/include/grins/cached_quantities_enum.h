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
//
// $Id$
//
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

#ifndef GRINS_CACHED_QUANTITIES_ENUM_H
#define GRINS_CACHED_QUANTITIES_ENUM_H

namespace GRINS
{
  namespace Cache
  {
    enum CachedQuantities{ X_VELOCITY = 0,
			   Y_VELOCITY,
			   Z_VELOCITY,
			   X_VELOCITY_GRAD,
			   Y_VELOCITY_GRAD,
			   Z_VELOCITY_GRAD,
			   PRESSURE,
			   THERMO_PRESSURE,
			   TEMPERATURE,
			   TEMPERATURE_GRAD,
			   PERFECT_GAS_DENSITY,
			   MIXTURE_DENSITY,
			   PERFECT_GAS_VISCOSITY,
			   SPECIES_VISCOSITY,
			   MIXTURE_VISCOSITY,
			   PERFECT_GAS_THERMAL_CONDUCTIVITY,
			   SPECIES_THERMAL_CONDUCTIVITY,
			   MIXTURE_THERMAL_CONDUCTIVITY,
			   PERFECT_GAS_SPECIFIC_HEAT_P,
			   SPECIES_SPECIFIC_HEAT_P,
			   MIXTURE_SPECIFIC_HEAT_P,
			   PERFECT_GAS_SPECIFIC_HEAT_V,
			   SPECIES_SPECIFIC_HEAT_V,
			   MIXTURE_SPECIFIC_HEAT_V,
			   MASS_FRACTIONS,
			   MASS_FRACTIONS_GRAD,
			   MOLE_FRACTIONS,
			   MOLAR_MASS,
			   MOLAR_DENSITIES,
			   SPECIES_GAS_CONSTANTS,
			   MIXTURE_GAS_CONSTANT,
			   DIFFUSION_COEFFS,
			   SPECIES_ENTHALPY,
			   SPECIES_NORMALIZED_ENTHALPY_MINUS_NORMALIZED_ENTROPY,
			   OMEGA_DOT,
			   ELECTRIC_FIELD_X,
			   ELECTRIC_FIELD_Y,
			   ELECTRIC_FIELD_Z,
			   CURRENT_DENSITY_X,
			   CURRENT_DENSITY_Y,
			   CURRENT_DENSITY_Z,
			   MAGNETIC_FIELD_X,
			   MAGNETIC_FIELD_Y,
			   MAGNETIC_FIELD_Z,
			   MAGNETIC_FLUX_X,
			   MAGNETIC_FLUX_Y,
			   MAGNETIC_FLUX_Z,
			   LORENTZ_FORCE_X,
			   LORENTZ_FORCE_Y,
			   LORENTZ_FORCE_Z };
  } // namespace Cache
} // namespace GRINS

#endif // GRINS_CACHED_QUANTITIES_ENUM_H