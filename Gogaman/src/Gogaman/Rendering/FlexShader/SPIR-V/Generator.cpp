#include "pch.h"
#include "Generator.h"

namespace Gogaman
{
	namespace FlexShader
	{
		namespace AST
		{
			void SPIRV_Visitor::VisitStatementBlock(Node::StatementBlock &node)
			{
			}

			void SPIRV_Visitor::VisitVariableDeclaration(Node::VariableDeclaration &node)
			{
			}

			void SPIRV_Visitor::VisitFunctionPrototype(Node::FunctionPrototype &node)
			{
			}

			void SPIRV_Visitor::VisitFunction(Node::Function &node)
			{
			}

			void SPIRV_Visitor::VisitNumericLiteral(Node::NumericLiteral &node)
			{
			}

			void SPIRV_Visitor::VisitStringLiteral(Node::StringLiteral &node)
			{
			}

			void SPIRV_Visitor::VisitIdentifier(Node::Identifier &node)
			{
			}

			void SPIRV_Visitor::VisitBinaryOperation(Node::BinaryOperation &node)
			{
			}

			void SPIRV_Visitor::VisitFunctionCall(Node::FunctionCall &node)
			{
			}

			void SPIRV_Visitor::VisitAssignment(Node::Assignment &node)
			{
			}
		}

		SPIRV_Generator::SPIRV_Generator(const AST::Node::Abstract *rootNode)
		{
			m_Module.reserve(5);

			//Magic number
			m_Module.emplace_back(0x07230203);

			//Version number
			//1.00 but should probably be 1.3 for Vulkan API v1.1
			m_Module.emplace_back(0x00010000);

			//Generator's magic number
			//Gogaman FlexShader generator is just 0
			m_Module.emplace_back(0);

			//Bound
			//Use symbolTable.size()
			m_Module.emplace_back(1);

			//Reserved word
			m_Module.emplace_back(0);

			//Instruction stream start

			//Declare capabilities
			//Matrix
			EmitInstruction(2, 17, { 0 });
			//Shader
			EmitInstruction(2, 17, { 1 });

			//Import extended instruction sets
			const auto words = PackStringLiteral("GLSL.std.450");
			uint32_t std450ResultID = 1;
			EmitInstruction(3 + words.size(), 11, std450ResultID, words);

			//Set addressing/memory model
			//Logcal addressing model and GLSL450 memory model
			EmitInstruction(3, 14, { 0, 1 });

			//Declare entry point(s)
			//for(auto i : root.programs)
			const uint32_t executionModel = 4;
			/*
			{
				switch(i.type)
				{
				case Shader::Type::Vertex:
					executionModel = 0;
				case Shader::Type::Geometry:
					executionModel = 3;
				case Shader::Type::Fragment:
					executionModel = 4;
				}
			}
			*/

			//Result<id> of OpFunction() of program's main function
			const uint32_t mainFunctionID = 2;

			std::vector<uint32_t> args = { executionModel, mainFunctionID };
			auto mainLiteralPacked = PackStringLiteral("main");
			args.insert(args.end(), mainLiteralPacked.begin(), mainLiteralPacked.end());
			//also insert <id> of global OpVariable for inputs/outputs of the entry point (eg. v_Color input, p_Color output)
			//The wordCount might not be right for this. It says "4 + variable" in the spec
			EmitInstruction(4 + mainLiteralPacked.size(), 15, args);

			//Declare execution mode
			uint32_t executionMode = 7;
			/*
			{
				switch(i.type)
				{
				case Shader::Type::Vertex:
					executionMode = ;
				case Shader::Type::Geometry:
					executionMode = ;
				case Shader::Type::Fragment:
				//OriginUpperLeft | Change to 8 for OriginLowerLeft (do we want OpenGL or DirectX style UVs?)
					executionMode = 7;
				}
			}
			*/

			//optionalLiterals are things like vertex count or groupSize etc... insert them to the end of the args
			//std::vector<uint32_t> optionalLiterals = {  };
			EmitInstruction(3, 16, { mainFunctionID, executionMode });

			//Decorations
			//for(const auto i : program.descriptors){
			//const auto decorationNamePacked = PackStringLiteral(i.name);
			//OpDecorate
			//EmitInstruction(3 + decorationNamePacked.size(), 71, 
			//}

			//Declare types
			uint32_t voidTypeID = 0, floatTypeID = 0, float4TypeID = 0;
			//Void
			EmitInstruction(2, 19, voidTypeID);
			//Float (32-bit)
			EmitInstruction(2, 22, floatTypeID, { 32 });
			//Float4 (32-bit)
			EmitInstruction(4, 23, float4TypeID, { floatTypeID, 4 });

			//Constants
			//Eg. float with value 0.35
			uint32_t testFloatConstID = 2;
			EmitInstruction(3 + 1, 43, { floatTypeID, testFloatConstID, (uint32_t)0.35f });
		}

		std::vector<uint32_t> SPIRV_Generator::PackStringLiteral(const std::string &string) const
		{
			//Packs every four UTF-8 octets into a 32-bit word

			//Needs to be uint32_t if there can be > 65.5k chars
			const uint16_t characterCount = string.length() + 1;

			std::vector<uint32_t> words;
			words.resize(ceil(characterCount / 4), 0);

			memcpy(words.data(), string.c_str(), characterCount);
			return words;
		}

		void SPIRV_Generator::EmitInstruction(const uint16_t wordCount, const uint16_t opCode, const std::vector<uint32_t> &operands)
		{
			m_Module.emplace_back(((uint32_t)wordCount << 16) | (uint32_t)opCode);
			for(const uint32_t i : operands)
				m_Module.emplace_back(i);
		}

		void SPIRV_Generator::EmitInstruction(const uint16_t wordCount, const uint16_t opCode, const uint32_t result, const std::vector<uint32_t> &operands)
		{
			m_Module.emplace_back(((uint32_t)wordCount << 16) | (uint32_t)opCode);
			m_Module.emplace_back(result);
			for(const uint32_t i : operands)
				m_Module.emplace_back(i);
		}
	}
}