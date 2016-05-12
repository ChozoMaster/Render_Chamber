#include <vector>
#include "Particle.h"
#include "Spring.h"

class Integrator
{
public:
	
	
	
	static void Euler(std::vector<Particle*>& particles, std::vector<RigidBody*>& rigidBodies, std::vector<Spring*>& springs);
	static void prepareForNextTimestep(std::vector<Particle*>& particles, std::vector<RigidBody*>& rigidBodies, std::vector<Spring*>& springs);
};