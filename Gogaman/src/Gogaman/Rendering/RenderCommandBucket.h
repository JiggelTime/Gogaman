#pragma once

#include "RenderingContext.h"
#include "RenderCommand.h"
#include "RenderCommandPacket.h"
#include "Gogaman/Sorting/RadixSort.h"

namespace Gogaman
{
	template<typename RenderCommandKeyType>
	class RenderCommandBucket
	{
	public:
		RenderCommandBucket(const uint32 renderCommandCount)
		{
			m_Keys = new RenderCommandKeyType[renderCommandCount];
		}

		template<typename RenderCommandType>
		RenderCommandType *AddRenderCommand(const RenderCommandKeyType key, const uint32_t additionalDataSize)
		{
			RenderCommandPacket::RenderCommandPacket packet = RenderCommandPacket::Create<RenderCommandType>(additionalDataSize);
			RenderCommandPacket::SetNextRenderCommandPacket(packet, nullptr);
			RenderCommandPacket::SetDispatchRenderCommandFunction(packet, RenderCommandType::DispatchFunction);
			
			m_Keys[m_CurrentKeyIndex]    = key;
			m_Packets[m_CurrentKeyIndex] = packet;
			m_CurrentKeyIndex++;

			return RenderCommandPacket::GetRenderCommand<RenderCommandType>(packet);
		}

		void SortPackets()
		{
			//RadixSort::Sort();
		}

		void DispatchPackets(const RenderingContext &context)
		{
			for(auto i = 0; i < m_CurrentKeyIndex; i++)
			{
				RenderCommandPacket::RenderCommandPacket packet = m_Packets[i];
				while(packet != nullptr)
				{
					DispatchRenderCommandFunction DispatchFunction = *RenderCommandPacket::GetDispatchRenderCommandFunction(packet);
					DispatchFunction(context, RenderCommandPacket::GetRenderCommand(packet));
					packet = RenderCommandPacket::GetNextRenderCommandPacket(packet);
				}
			}
		}
	private:
		uint32_t               m_CurrentKeyIndex;
		RenderCommandKeyType  *m_Keys;
		RenderCommandPacket   *m_Packets;
	};
}