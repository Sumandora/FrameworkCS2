#pragma once

#include "../VirtualMethod.hpp"

#include "Particle.hpp"
#include "ParticleEffect.hpp"
#include "ParticleSnapshot.hpp"

struct GameParticleManager {
	static void resolve_signatures();
	static GameParticleManager* get();

	VIRTUAL_METHOD(4, show_particle, void, (Particle * particle), (this, particle));

	Particle* create_particle(ParticleEffect* effect);
	void modify_particle(float unk, Particle* particle, int control_point_id, void* data);
	void attach_particle_to_snapshot(Particle* particle, int unk, ParticleSnapshot** snapshot);
};
