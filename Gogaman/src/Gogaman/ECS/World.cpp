#include "pch.h"
#include "World.h"

namespace Gogaman
{
	World::World()
	{}

	void World::Initialize()
	{
		std::cout << "[World] Initializing..." << std::endl;

		for(auto &i : m_Systems)
			i->Initialize();
	}

	void World::Update()
	{
		for(auto &i : m_Systems)
			i->Update();
	}

	void World::Render()
	{
		for(auto &i : m_Systems)
			i->Render();
	}

	void World::Shutdown()
	{
		for(auto &i : m_Systems)
			i->Shutdown();
	}

	Entity World::CreateEntity()
	{
		return m_EntityManager.CreateEntity();
	}

	void World::DestroyEntity(Entity entity)
	{
		m_EntityManager.DestroyEntity(entity);
	}

	void World::AddSystem(std::unique_ptr<System> system)
	{
		system->SetWorld(this);
		m_Systems.emplace_back(std::move(system));
	}
}