#pragma once

#include "Gogaman/Rendering/FlexShader/AbstractCodeGenerator.h"

#include "Gogaman/Rendering/FlexShader/Type.h"

namespace Gogaman
{
	namespace FlexShader
	{
		class CodeGenerator : public AbstractCodeGenerator<CodeGenerator>
		{
		private:
			class InstructionStream
			{
			public:
				InstructionStream()  = default;
				~InstructionStream() = default;

				InstructionStream &operator+=(const InstructionStream &other)
				{
					m_Stream.insert(m_Stream.end(), other.m_Stream.begin(), other.m_Stream.end());
					return *this;
				}

				InstructionStream &operator+=(const uint32_t value)
				{
					m_Stream.emplace_back(value);
					return *this;
				}

				inline void AddInstruction(const uint16_t opCode) { m_Stream.emplace_back((1 << 16) | (uint32_t)opCode); }

				inline void AddInstruction(const uint16_t opCode, const uint32_t argument)
				{
					m_Stream.emplace_back((2 << 16) | (uint32_t)opCode);
					m_Stream.emplace_back(argument);
				}

				inline void AddInstruction(const uint16_t opCode, const uint32_t argument, const std::vector<uint32_t> &arguments)
				{
					m_Stream.emplace_back(((2 + arguments.size()) << 16) | (uint32_t)opCode);
					m_Stream.emplace_back(argument);
					for(const uint32_t i : arguments)
						m_Stream.emplace_back(i);
				}

				inline void AddInstruction(const uint16_t opCode, const uint32_t argument1, const uint32_t argument2)
				{
					m_Stream.emplace_back((3 << 16) | (uint32_t)opCode);
					m_Stream.emplace_back(argument1);
					m_Stream.emplace_back(argument2);
				}

				inline void AddInstruction(const uint16_t opCode, const uint32_t argument1, const uint32_t argument2, const std::vector<uint32_t> &arguments)
				{
					m_Stream.emplace_back(((3 + arguments.size()) << 16) | (uint32_t)opCode);
					m_Stream.emplace_back(argument1);
					m_Stream.emplace_back(argument2);
					for(const uint32_t i : arguments)
						m_Stream.emplace_back(i);
				}

				inline void AddInstruction(const uint16_t opCode, const uint32_t argument1, const uint32_t argument2, const uint32_t argument3)
				{
					m_Stream.emplace_back((4 << 16) | (uint32_t)opCode);
					m_Stream.emplace_back(argument1);
					m_Stream.emplace_back(argument2);
					m_Stream.emplace_back(argument3);
				}

				inline void AddInstruction(const uint16_t opCode, const uint32_t argument1, const uint32_t argument2, const uint32_t argument3, const std::vector<uint32_t> &arguments)
				{
					m_Stream.emplace_back(((4 + arguments.size()) << 16) | (uint32_t)opCode);
					m_Stream.emplace_back(argument1);
					m_Stream.emplace_back(argument2);
					m_Stream.emplace_back(argument3);
					for(const uint32_t i : arguments)
						m_Stream.emplace_back(i);
				}

				inline void AddInstruction(const uint16_t opCode, const uint32_t argument1, const uint32_t argument2, const uint32_t argument3, const uint32_t argument4)
				{
					m_Stream.emplace_back((5 << 16) | (uint32_t)opCode);
					m_Stream.emplace_back(argument1);
					m_Stream.emplace_back(argument2);
					m_Stream.emplace_back(argument3);
					m_Stream.emplace_back(argument4);
				}

				inline void AddInstruction(const uint16_t opCode, const uint32_t argument1, const uint32_t argument2, const uint32_t argument3, const uint32_t argument4, const std::vector<uint32_t> &arguments)
				{
					m_Stream.emplace_back(((5 + arguments.size()) << 16) | (uint32_t)opCode);
					m_Stream.emplace_back(argument1);
					m_Stream.emplace_back(argument2);
					m_Stream.emplace_back(argument3);
					m_Stream.emplace_back(argument4);
					for(const uint32_t i : arguments)
						m_Stream.emplace_back(i);
				}

				inline const std::vector<uint32_t> &GetStream() const { return m_Stream; }
			private:
				std::vector<uint32_t> m_Stream;
			};
		public:
			CodeGenerator(const IR::IR &intermediateRepresentation, const std::string &entryPointName);
			~CodeGenerator() = default;

			inline const std::pair<uint32_t, const uint8_t *> GetData() const { return { m_Instructions.GetStream().size() * sizeof(uint32_t), (uint8_t *)m_Instructions.GetStream().data() }; }
		private:
			std::vector<uint32_t> PackStringLiteral(const std::string &string) const;
		private:
			InstructionStream m_DecorationInstructions;
			InstructionStream m_TypeDeclarationInstructions;
			InstructionStream m_ConstantInstructions;
			InstructionStream m_GlobalVariableDeclarationInstructions;
			InstructionStream m_FunctionDefinitionInstructions;

			InstructionStream m_Instructions;

			uint32_t m_TypeIDs[(uint8_t)Type::Count];
			uint32_t m_FunctionVariableTypeIDs[(uint8_t)Type::Count - 1];
			uint32_t m_InputVariableTypeIDs[(uint8_t)Type::Count - 1];
			uint32_t m_OutputVariableTypeIDs[(uint8_t)Type::Count - 1];

			uint32_t              m_BooleanConstantIDs[2];
			std::vector<uint32_t> m_IntegerConstantIDs;
			std::vector<uint32_t> m_FloatConstantIDs;
		};
	}
}