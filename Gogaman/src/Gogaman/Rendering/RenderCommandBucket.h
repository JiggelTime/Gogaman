#pragma once

#include "RenderingContext.h"

#include "RenderCommand.h"
#include "RenderCommandPacket.h"

#include "Gogaman/Sorting/RadixSort.h"

namespace Gogaman
{
	template<uint32_t maxRenderCommandPacketCount, typename RenderCommandKeyType>
	class RenderCommandBucket
	{
	public:
		RenderCommandBucket()
			: m_KeyCount(0)
		{
			m_Keys    = new RenderCommandKeyType[maxRenderCommandPacketCount];
			m_Packets = new RenderCommandPacket::RenderCommandPacket[maxRenderCommandPacketCount];
		}

		template<typename RenderCommandType>
		RenderCommandType *AddRenderCommand(const RenderCommandKeyType key, const uint32_t additionalDataSize)
		{
			RenderCommandPacket::RenderCommandPacket packet = RenderCommandPacket::Create<RenderCommandType>(additionalDataSize);
			RenderCommandPacket::SetNextRenderCommandPacket(packet, nullptr);
			RenderCommandPacket::SetDispatchRenderCommandFunction(packet, RenderCommandType::Dispatch);
			
			m_Keys[m_KeyCount]    = key;
			m_Packets[m_KeyCount] = packet;
			m_KeyCount++;

			return RenderCommandPacket::GetRenderCommand<RenderCommandType>(packet);
		}

		void SortPackets()
		{
			//RadixSort::Sort();
		}

		void DispatchPackets(const RenderingContext &context)
		{
			for(const auto i = 0; i < m_KeyCount; i++)
			{
				RenderCommandPacket::RenderCommandPacket packet = m_Packets[i];
				while(packet != nullptr)
				{
					RenderCommand::DispatchFunction Dispatch = *RenderCommandPacket::GetDispatchRenderCommandFunction(packet);
					Dispatch(context, RenderCommandPacket::GetRenderCommand(packet));
					packet = RenderCommandPacket::GetNextRenderCommandPacket(packet);
				}
			}
		}
	private:
		uint32_t                                  m_KeyCount;
		RenderCommandKeyType                     *m_Keys;
		RenderCommandPacket::RenderCommandPacket *m_Packets;
	};
}