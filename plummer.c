#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "reboundx.h"
#define S_HALFMASS 2.25992
static void rebx_calculate_plummer_potential_forces(struct rebx_extras* const rebx, struct reb_simulation* const sim, const double GMtot, const double HalfMassRadius){
    struct reb_particle* const particles = sim->particles;
    const double r_to_s = S_HALFMASS / HalfMassRadius;
    const int _N_real = sim->N - sim->N_var;
#pragma omp parallel for
    for (int i=0;i<_N_real;i++){
        	const struct reb_particle p = particles[i];             
        	const double x  = p.x;
        	const double y  = p.y;
        	const double z  = p.z;
        	const double r_sq = x*x + y*y+z*z;
        	const double r_cubed = sqrt(r_sq) * r_sq;
        	const double s_sq = r_sq * r_to_s * r_to_s;
		const double s_cubed = r_cubed * r_to_s * r_to_s * r_to_s;
		const double _x = sqrt(3 + s_sq);
		const double _y = _x *_x *_x;
		const double Menc_divided_by_Mtot = s_cubed / _y;
		particles[i].ax += -1*GMtot * Menc_divided_by_Mtot * x / r_cubed;
		particles[i].ay += -1*GMtot * Menc_divided_by_Mtot * y / r_cubed;
		particles[i].az += -1*GMtot * Menc_divided_by_Mtot * z / r_cubed;
	}
}
void rebx_plummer_potential(struct reb_simulation* const sim, struct rebx_force* const plummer_potential, struct reb_particle* const particles, const int N){
    struct rebx_extras* const rebx = sim->extras;
    double* total_mass = rebx_get_param(rebx, plummer_potential->ap, "total_mass");
    double* half_mass_radius = rebx_get_param(rebx, plummer_potential->ap, "half_mass_radius");
    if (total_mass == NULL){
        reb_error(sim, "Need to set total mass in plummer_forces effect.  See examples in documentation.\n");
    }
    if (half_mass_radius == NULL){
        reb_error(sim, "Need to set half-mass radius in plummer_forces effect.  See examples in documentation.\n");
    }
    const double GMtot = (sim->G) * (*total_mass);
    rebx_calculate_plummer_potential_forces(rebx,sim,GMtot,*half_mass_radius);
}
