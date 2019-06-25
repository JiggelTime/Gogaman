#pragma once

namespace Gogaman
{
	template<typename ResourceType, typename ResourceIdentifierType>
	class ResourceCache
	{
	public:
		inline void          Set(const ResourceIdentifierType resourceID, std::unique_ptr<ResourceType> resource) { m_CachedResources[resourceID] = std::move(resource); }
		inline const ResourceType &Get(const ResourceIdentifierType resourceID) const { return *m_CachedResources[resourceID].get(); }
	private:
		std::array<std::unique_ptr<ResourceType>, std::numeric_limits<ResourceIdentifierType>::max()> m_CachedResources;
	};
}