#include "GameParticleManager.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include "../../Memory.hpp"

#include "Particle.hpp"
#include "ParticleEffect.hpp"
#include "ParticleSnapshot.hpp"

static GameParticleManager** game_particle_manager = nullptr;

static Particle* (*create_particle)(GameParticleManager* thisptr, ParticleEffect* effect) = nullptr;
static void (*modify_particle)(float unk, GameParticleManager* thisptr, Particle* particle, int control_point_id, void* data) = nullptr;
static void (*attach_particle_to_snapshot)(GameParticleManager* thisptr, Particle* particle, int unk, ParticleSnapshot** snapshot) = nullptr;

void GameParticleManager::resolve_signatures()
{
	// credits to Exlodium for sharing information about the GameParticleManagerSystems layout (https://www.unknowncheats.me/forum/4447037-post7979.html)

	// In case this signature ever dies, you can also just go into any code using it (weapon_tracers_taser or spectator_utility_trail),
	// 		because the game barely stores the pointer, it repeatedly calls a function returning it.
	game_particle_manager
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"26CGameParticleManagerSystem">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
			  .find_xrefs(SignatureScanner::XRefTypes::absolute(),
				  BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
			  .sub(sizeof(char*))
			  .find_xrefs(SignatureScanner::XRefTypes::absolute(),
				  BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
			  .add(sizeof(void*))
			  .find_xrefs(SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .repeater([](auto& ptr) {
				  ptr.next_instruction();
				  return !ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"48 89 05">());
			  })
			  .add(3)
			  .relative_to_absolute() // here lies GameParticleManagerSystems
			  .add(0x18) // This thing has a bunch of XREFs, it's quite easy to find even without looking at other code.
			  .BCRL_EXPECT(GameParticleManager**, game_particle_manager);

	// Once again, if these signatures go irrecoverably outdated, look at the two particles I mentioned above and look at what they do.
	::create_particle
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"particles/entity/spectator_utility_trail.vpcf">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
			  .find_xrefs(SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .add(4)
			  // Dumb initializer function that needs to mention a bunch of random stuff...
			  .filter([](const auto& ptr) { return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"c7">()); })
			  .repeater([](auto& ptr) {
				  ptr.next_instruction();
				  return !ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"e8">());
			  })
			  .add(1)
			  .relative_to_absolute()
			  .BCRL_EXPECT(decltype(::create_particle), ::create_particle);
	::modify_particle
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"ANNOTATIONS: hiding node \"%s\"\n">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
			  .find_xrefs(SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"e8 ? ? ? ? 49">())
			  .add(1)
			  .relative_to_absolute()
			  .BCRL_EXPECT(decltype(::modify_particle), ::modify_particle);
	::attach_particle_to_snapshot
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"prop_group">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
			  .find_xrefs(SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .sub(7)
			  .relative_to_absolute()
			  .BCRL_EXPECT(decltype(::attach_particle_to_snapshot), ::attach_particle_to_snapshot);
}

GameParticleManager* GameParticleManager::get()
{
	return *game_particle_manager;
}

Particle* GameParticleManager::create_particle(ParticleEffect* effect)
{
	return ::create_particle(this, effect);
}

void GameParticleManager::modify_particle(float unk, Particle* particle, int control_point_id, void* data)
{
	::modify_particle(unk, this, particle, control_point_id, data);
}

void GameParticleManager::attach_particle_to_snapshot(Particle* particle, int unk, ParticleSnapshot** snapshot)
{
	::attach_particle_to_snapshot(this, particle, unk, snapshot);
}
