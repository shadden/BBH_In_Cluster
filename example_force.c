#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rebound.h"
#include "reboundx.h"
void example_force(struct reb_simulation* r){
	struct reb_particle* particles = r->particles;
	const int N = r->N;
	const struct reb_particle star = particles[0]; 
	const double EPS = -0.02;
#pragma omp parallel for
    for (int i=1;i<N;i++){
        const struct reb_particle p = particles[i];             
        const double prx  = p.x-star.x;
        const double pry  = p.y-star.y;
        const double prz  = p.z-star.z;
        const double pr_sq   = prx*prx + pry*pry + prz*prz;         // distance relative to star squared
        const double pr4   = pr_sq*pr_sq;
	particles[i].ax += -2 * EPS * prx / pr4;
	particles[i].ay += -2 * EPS * pry / pr4;
	particles[i].az += -2 * EPS * prz / pr4;
    }
}
