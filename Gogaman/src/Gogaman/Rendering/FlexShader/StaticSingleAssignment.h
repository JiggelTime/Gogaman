#pragma once

#include "Gogaman/Utilities/Algorithm/DirectedGraph.h"

#include "IntermediateRepresentation.h"

namespace Gogaman
{
	namespace FlexShader
	{
		namespace IR
		{
			class SSA
			{
			public:
				SSA(const IR &intermediateRepresentation);
				~SSA() = default;
			private:
				void BuildControlFlowGraph(const IR &ir);
			private:
				DirectedGraph<uint16_t>                    m_ControlFlowGraph;
				std::vector<std::pair<uint32_t, uint32_t>> m_BasicBlocks;
			};
		}
	}
}