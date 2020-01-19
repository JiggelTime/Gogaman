#pragma once

#include "Gogaman/RenderHardwareInterface/Identifier.h"
#include "Gogaman/RenderHardwareInterface/Fence.h"
#include "Gogaman/RenderHardwareInterface/CommandBuffer.h"
#include "Gogaman/RenderHardwareInterface/CommandHeap.h"

namespace Gogaman
{
	template<RHI::CommandHeap::Type type>
	class CommandQueue
	{
	private:
		struct SubmissionBatch
		{
			std::vector<RHI::CommandBufferID> commandBufferIDs;

			RHI::Fence *fence = nullptr;
		};
	public:
		CommandQueue()
		{
			m_CommandHeap = std::make_unique<RHI::CommandHeap>(type);
		}

		~CommandQueue()
		{
			for(RHI::CommandBufferID i : m_CommandBufferIDs)
			{
				g_Device->GetResources().commandBuffers.Destroy(i);
			}

			m_CommandHeap.reset();
		}

		inline RHI::CommandBufferID AvailableCommandBuffer()
		{
			if(m_SubmissionBatch.commandBufferIDs.size() == m_CommandBufferIDs.size())
			{
				return m_CommandBufferIDs.emplace_back(m_CommandHeap->CreateCommandBuffer());
			}

			return m_CommandBufferIDs[m_SubmissionBatch.commandBufferIDs.size()];
		}

		inline void Submit(const RHI::CommandBufferID commandBufferID)
		{
			m_SubmissionBatch.commandBufferIDs.emplace_back(commandBufferID);
		}

		inline void Submit(RHI::Fence *fence)
		{
			m_SubmissionBatch.fence = fence;

			Flush();

			m_SubmissionBatch.fence = nullptr;
		}

		inline void Flush()
		{
			//g_Device->SubmitCommands(type, m_UsedCommandBufferCount, m_SubmissionBatch.commandBufferIDs.data(), m_SubmissionBatch.fence);
			g_Device->SubmitRenderCommands(m_SubmissionBatch.commandBufferIDs.size(), m_SubmissionBatch.commandBufferIDs.data(), m_SubmissionBatch.fence);

			m_SubmissionBatch.commandBufferIDs.clear();
		}

		inline void Reset()
		{
			m_CommandHeap->Reset();
		}
	private:
		SubmissionBatch                   m_SubmissionBatch;
		std::vector<RHI::CommandBufferID> m_CommandBufferIDs;
		std::unique_ptr<RHI::CommandHeap> m_CommandHeap;
		//uint8_t                           m_UsedCommandBufferCount;
	};

	class FrameContext
	{
	public:
		FrameContext()  = default;
		~FrameContext() = default;

		inline void Flush()
		{
			m_TransferQueue.Flush();
			m_ComputeQueue.Flush();
			m_RenderQueue.Flush();
		}

		inline void Reset()
		{
			m_TransferQueue.Reset();
			m_ComputeQueue.Reset();
			m_RenderQueue.Reset();
		}

		inline const CommandQueue<RHI::CommandHeap::Type::Transfer> &GetTransferQueue() const { return m_TransferQueue; }
		inline       CommandQueue<RHI::CommandHeap::Type::Transfer> &GetTransferQueue()       { return m_TransferQueue; }

		inline const CommandQueue<RHI::CommandHeap::Type::Compute> &GetComputeQueue() const { return m_ComputeQueue; }
		inline       CommandQueue<RHI::CommandHeap::Type::Compute> &GetComputeQueue()       { return m_ComputeQueue; }

		inline const CommandQueue<RHI::CommandHeap::Type::Render> &GetRenderQueue() const { return m_RenderQueue; }
		inline       CommandQueue<RHI::CommandHeap::Type::Render> &GetRenderQueue()       { return m_RenderQueue; }
	private:
		CommandQueue<RHI::CommandHeap::Type::Transfer> m_TransferQueue;
		CommandQueue<RHI::CommandHeap::Type::Compute>  m_ComputeQueue;
		CommandQueue<RHI::CommandHeap::Type::Render>   m_RenderQueue;

		//TODO: deferred deletions
	};
}