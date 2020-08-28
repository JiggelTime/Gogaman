#include "pch.h"
#include "FlexShader.h"

#include "Gogaman/Utilities/Algorithm/DirectedGraph.h"

//include "Token.h"
#include "Lexer.h"

#include "Visitor.h"
#include "AbstractSyntaxTree.h"
#include "SyntacticAnalysis.h"

#include "IntermediateRepresentation.h"

#include "CodeGenerator.h"

#define GM_FLEX_SHADER_ENABLE_AST_LOGGING TRUE
#define GM_FLEX_SHADER_ENABLE_IR_LOGGING  TRUE

namespace Gogaman
{
	namespace FlexShader
	{
		Module::Module(const std::string &source)
		{
			m_RootNode = Parse(Lex(source));

			#if GM_FLEX_SHADER_ENABLE_AST_LOGGING
				AST::LogVisitor logger;
				m_RootNode->Accept(logger);
			#endif
		}

		const std::pair<uint32_t, const uint8_t *> Module::CompileKernel(const std::string &entryPointName, const std::vector<std::string> &componentNames)
		{
			AST::SpecializationVisitor specializer(componentNames);
			m_RootNode->Accept(specializer);

			AST::ModuleVisitor moduleVisitor;
			AST::Node::Abstract *transformedRootNode = m_RootNode->Accept(moduleVisitor);

			AST::SemanticAnalyzer semanticAnalyzer;
			transformedRootNode->Accept(semanticAnalyzer);

			//(Temporary) log transformed AST
			//AST::LogVisitor logger;
			//transformedRootNode->Accept(logger);

			//Generate descriptor layouts from component data
			for(const auto i : moduleVisitor.GetComponentData())
			{
				RHI::DescriptorGroupLayout::Binding *bindings = new RHI::DescriptorGroupLayout::Binding;
				memcpy(bindings, i.second.bindings.data(), sizeof(RHI::DescriptorGroupLayout::Binding) * i.second.bindings.size());
				//TODO: Proper bind flag
				//m_ComponentLayouts[i.first] = RHI::DescriptorGroupLayout(i.second.bindings.size(), bindings, RHI::Shader::StageFlag::Pixel);
			}

			Translator translator;
			transformedRootNode->Accept(translator);
			const IR::IR &intermediateRepresentation = translator.IR();

			#if GM_FLEX_SHADER_ENABLE_IR_LOGGING
				LogIR(intermediateRepresentation);
			#endif
			/*
			//Generate CFG
			const auto IsBranchInstruction = [](const IR::Instruction &instruction) { return (instruction.operation == IR::Operation::UnconditionalBranch) || (instruction.operation == IR::Operation::ConditionalBranch); };

			std::vector<uint32_t> branchInstructionIndices;
			for(auto i = 0; i < intermediateRepresentation.instructions.size(); i++)
				if(IsBranchInstruction(intermediateRepresentation.instructions[i]))
					branchInstructionIndices.emplace_back(i);

			DirectedGraph<uint16_t> controlFlowGraph;
			std::vector<std::pair<uint32_t, uint32_t>> basicBlocks;
			uint32_t currentLeader = 0;
			for(const uint32_t i : branchInstructionIndices)
			{
				const IR::Instruction &instruction = intermediateRepresentation.instructions[i];

				const uint32_t jumpTarget = instruction.operation == IR::Operation::UnconditionalBranch ? instruction.address1.GetValue() : instruction.address2.GetValue();

				const uint16_t sourceVertexIndex = controlFlowGraph.CreateVertex();
				basicBlocks.emplace_back(currentLeader, i);

				basicBlocks.emplace_back(jumpTarget, jumpTarget);
				for(uint32_t j = jumpTarget + 1; j < intermediateRepresentation.instructions.size(); j++)
				{
					if(IsBranchInstruction(intermediateRepresentation.instructions[j]))
					{
						basicBlocks.back().second = j;
						break;
					}
				}

				controlFlowGraph.CreateEdge(sourceVertexIndex, controlFlowGraph.CreateVertex());

				currentLeader = i + 1;
				if(currentLeader == intermediateRepresentation.instructions.size())
					break;
			}
			*/

			CodeGenerator *generator = new CodeGenerator(intermediateRepresentation, entryPointName);
			return generator->GetData();
		}
	}
}