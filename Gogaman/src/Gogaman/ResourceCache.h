#pragma once

namespace Gogaman
{
	template<typename ResourceType>
	class ResourceCache
	{
	public:
		ResourceCache()
		{}

		~ResourceCache()
		{}

		void Add(std::string &name, ResourceType &&resource) { m_Resources[name] = resource; }

		//Change to: ResourceType &Get(std::string &name) const { return m_Resources[name]; }
		ResourceType &Get(std::string &name) { return m_Resources[name]; }

		void Remove(std::string &name) {}
	private:
		std::unordered_map<std::string, ResourceType> m_Resources;
	};
}