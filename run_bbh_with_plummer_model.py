import numpy as np
import pandas as pd
import rebound as rb
import reboundx as rbx
import ctypes

force_lib = ctypes.cdll.LoadLibrary("./libforces.so")
myforce.update_accelerations = force_lib.rebx_plummer_potential

# Read IC file
df = pd.read_csv("10000_R3_Plummer_with_BBH.txt",delimiter="\s+")

# Add columns for radius and velocity magnitude
df['r_[pc]']=np.linalg.norm(df[['x_[pc]','y_[pc]','z_[pc]']].values,axis=1)
df['v_[km/s]']=np.linalg.norm(df[["v{}_[km/s]".format(x) for x in 'xyz']].values,axis=1)

# Sort by radial distance
df.sort_values("r_[pc]",ascending=True,inplace=True)

# Columns for use in simulation ICs
PARSEC_TO_AU = 206265
KMperS_TO_AUperYR = .210805
for coord in 'xyz':
    df['{}'.format(coord)] = df['{}_[pc]'.format(coord)] * PARSEC_TO_AU
    df['v{}'.format(coord)] = df['v{}_[km/s]'.format(coord)] * KMperS_TO_AUperYR

def get_sim(Nstar):
    sim = rb.Simulation()
    sim.units = ['AU', 'yr', 'Msun'] # Set units
    sim.integrator='leapfrog'
    ###################
    # Add stars
    ###################
    for row_i in range(Nstar+2):
        row = df.iloc[row_i]
        sim.add(m=row['#Mass_[Msun]'],
                x=row.x,y=row.y,z=row.z,
                vx=row.vx,vy=row.vy,vz=row.vz
        )
    sim.move_to_com()
    sim.dt = sim.particles[1].P / 200.
    return sim

sim = get_sim(100)
# Set up reboundx for external force
rebx = rbx.Extras(sim)
rebx.register_param('total_mass','REBX_TYPE_DOUBLE')
rebx.register_param('half_mass_radius','REBX_TYPE_DOUBLE')
myforce = rebx.create_force("plummer")
myforce.force_type = 'pos'
rebx.add_force(myforce)

# Total mass is all stars minus BBH
myforce.params['total_mass']= df["#Mass_[Msun]"].sum() - 100.0
myforce.params['half_mass_radius']= 3 * PARSEC_TO_AU
rebx.save("rebxpars.bin")

# SA saves snapshot every 100s of wallitme
sim.automateSimulationArchive("plummer_sim_archive.bin",walltime=100.) 
sim.integrate(1e8)
