#pragma once

namespace Gogaman
{
	template<typename ResourceIdentifierType>
	class Resource
	{
	public:
		Resource(const Resource &) = delete;
		Resource(Resource &&other) = default;

		Resource &operator=(const Resource &) = delete;
		Resource &operator=(Resource &&other) = default;
	protected:
		Resource()
			: identifier(0)
		{}

		~Resource() = default;
	public:
		ResourceIdentifierType identifier;
	};
}