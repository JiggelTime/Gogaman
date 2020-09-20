#include "pch.h"
#include "Translator.h"

//#include "Type.h"
#include "AbstractSyntaxTree.h"

namespace Gogaman
{
	namespace FlexShader
	{
		IR::Address Translator::VisitStatementBlock(AST::Node::StatementBlock &node)
		{
			m_SymbolTables.emplace_back();
			for(AST::Node::Statement *i : node.statements)
				i->Accept(*this);

			m_SymbolTables.pop_back();
			return {};
		}

		IR::Address Translator::VisitFunctionPrototype(AST::Node::FunctionPrototype &node) { return {}; }

		IR::Address Translator::VisitFunction(AST::Node::Function &node)
		{
			const IR::Address address(IR::Address::Type::Function, m_FunctionCount++, node.prototype->returnType);
			RegisterSymbolAddress(node.prototype->name, address);
			m_IR.functionSignatures[address.GetValue()].returnType = node.prototype->returnType;

			m_IR.executionOrder.emplace_back(m_IR.instructions.size());
			AddInstruction(IR::Operation::BeginFunction, address);
			m_SymbolTables.emplace_back();

			for(AST::Node::VariableDeclaration *i : node.prototype->parameters)
			{
				m_IR.functionSignatures[address.GetValue()].parameterTypes.emplace_back(i->type);
				m_IR.functionSignatures[address.GetValue()].parameterIDs.emplace_back(i->Accept(*this).GetValue());
			}

			node.body->Accept(*this);

			m_IR.executionOrder.emplace_back(m_IR.instructions.size());
			AddInstruction(IR::Operation::EndFunction);
			m_SymbolTables.pop_back();

			return address;
		}

		IR::Address Translator::VisitFunctionCall(AST::Node::FunctionCall &node)
		{
			std::vector<IR::Address> parameterAddresses;
			parameterAddresses.reserve(node.parameters.size());
			for(AST::Node::Expression *i : node.parameters)
				parameterAddresses.emplace_back(i->Accept(*this));

			for(const IR::Address i : parameterAddresses)
			{
				m_IR.executionOrder.emplace_back(m_IR.instructions.size());
				AddInstruction(IR::Operation::FunctionParameter, i);
			}

			m_IR.executionOrder.emplace_back(m_IR.instructions.size());
			AddInstruction(IR::Operation::FunctionCall, GetSymbolAddress(node.name));
			return { IR::Address::Type::InstructionPointer, (uint32_t)m_IR.instructions.size() - 1, GetSymbolAddress(node.name).GetDataType() };
		}

		IR::Address Translator::VisitVariableDeclaration(AST::Node::VariableDeclaration &node)
		{
			const IR::Address address(IR::Address::Type::Variable, m_VariableCount++, node.type);
			RegisterSymbolAddress(node.name, address);
			m_IR.variableSpecifiers.emplace_back(node.specifier);

			if((uint8_t)node.specifier < 16)
				m_IR.descriptors[(uint8_t)node.specifier].emplace_back(address);

			m_IR.executionOrder.emplace_back(m_IR.instructions.size());
			AddInstruction(IR::Operation::Variable, address);
			return address;
		}

		IR::Address Translator::VisitBooleanLiteral(AST::Node::BooleanLiteral &node) { return { IR::Address::Type::ConstantOrVector, node.value ? 1u : 0u, Type::Boolean }; }

		IR::Address Translator::VisitIntegerLiteral(AST::Node::IntegerLiteral &node)
		{
			for(uint32_t i = 0; i < m_IR.integerConstantValues.size(); i++)
				if(node.value == m_IR.integerConstantValues[i])
					return { IR::Address::Type::ConstantOrVector, i, Type::Integer };

			const uint32_t identifier = m_IR.integerConstantValues.size();
			m_IR.integerConstantValues.emplace_back(node.value);
			return { IR::Address::Type::ConstantOrVector, identifier, Type::Integer };
		}

		IR::Address Translator::VisitFloatingPointLiteral(AST::Node::FloatingPointLiteral &node)
		{
			for(uint32_t i = 0; i < m_IR.floatConstantValues.size(); i++)
				if(node.value == m_IR.floatConstantValues[i])
					return { IR::Address::Type::ConstantOrVector, i, Type::Float };

			const uint32_t identifier = m_IR.floatConstantValues.size();
			m_IR.floatConstantValues.emplace_back(node.value);
			return { IR::Address::Type::ConstantOrVector, identifier, Type::Float };
		}

		IR::Address Translator::VisitIdentifier(AST::Node::Identifier &node) { return GetSymbolAddress(node.name); }

		IR::Address Translator::VisitVector(AST::Node::Vector &node)
		{
			IR::Address *addresses = new IR::Address[node.expressions.size()];
			for(auto i = 0; i < node.expressions.size(); i++)
				addresses[i] = node.expressions[i]->Accept(*this);

			switch(node.expressions.size())
			{
			case 2:
				if(addresses[0].GetDataType() == Type::Integer)
				{
					const IR::Address address(IR::Address::Type::ConstantOrVector, m_IR.integer2Values.size(), Type::Integer2);
					m_IR.float2Values.emplace_back(addresses);
					return address;
				}
				else if(addresses[0].GetDataType() == Type::Float)
				{
					const IR::Address address(IR::Address::Type::ConstantOrVector, m_IR.float2Values.size(), Type::Float2);
					m_IR.float2Values.emplace_back(addresses);
					return address;
				}
			case 3:
				if(addresses[0].GetDataType() == Type::Integer)
				{
					const IR::Address address(IR::Address::Type::ConstantOrVector, m_IR.integer3Values.size(), Type::Integer3);
					m_IR.integer3Values.emplace_back(addresses);
					return address;
				}
				else if(addresses[0].GetDataType() == Type::Float)
				{
					const IR::Address address(IR::Address::Type::ConstantOrVector, m_IR.float3Values.size(), Type::Float3);
					m_IR.float3Values.emplace_back(addresses);
					return address;
				}
			case 4:
				if(addresses[0].GetDataType() == Type::Integer)
				{
					const IR::Address address(IR::Address::Type::ConstantOrVector, m_IR.integer4Values.size(), Type::Integer4);
					m_IR.integer4Values.emplace_back(addresses);
					return address;
				}
				else if(addresses[0].GetDataType() == Type::Float)
				{
					const IR::Address address(IR::Address::Type::ConstantOrVector, m_IR.float4Values.size(), Type::Float4);
					m_IR.float4Values.emplace_back(addresses);
					return address;
				}
			default:
				return {};
			}
		}

		IR::Address Translator::VisitMemberSelection(AST::Node::MemberSelection &node)
		{
			Type resultType;
			if((uint8_t)GetSymbolAddress(node.object->name).GetDataType() < (uint8_t)Type::Float)
				resultType = Type((uint8_t)Type::Integer + (node.memberName.size() - 1));
			else
				resultType = Type((uint8_t)Type::Float + (node.memberName.size() - 1));

			uint32_t components = (uint32_t)resultType;

			GM_DEBUG_ASSERT(node.memberName.size() <= 4, "Failed to translate FlexShader member selection | Member name \"%s\" is invalid", node.memberName.c_str());
			for(auto i = 0; i < node.memberName.size(); i++)
			{
				switch(node.memberName[i])
				{
				case 'x':
				case 'r':
					break;
				case 'y':
				case 'g':
					components |= 1 << (i * 2 + 8);
					break;
				case 'z':
				case 'b':
					components |= 2 << (i * 2 + 8);
					break;
				case 'w':
				case 'a':
					components |= 3 << (i * 2 + 8);
					break;
				default:
					break;
				}
			}

			m_IR.executionOrder.emplace_back(m_IR.instructions.size());
			AddInstruction(IR::Operation::VectorSwizzle, GetSymbolAddress(node.object->name), components);
			return { IR::Address::Type::InstructionPointer, (uint32_t)m_IR.instructions.size() - 1, resultType };
		}

		IR::Address Translator::VisitBinaryOperation(AST::Node::BinaryOperation &node)
		{
			IR::Operation operation;
			switch(node.operatorType)
			{
			case Token::Type::Plus:
				operation = IR::Operation::Add;
				break;
			case Token::Type::Minus:
				operation = IR::Operation::Subtract;
				break;
			case Token::Type::Asterisk:
				operation = IR::Operation::Multiply;
				break;
			case Token::Type::Slash:
				operation = IR::Operation::Divide;
				break;
			case Token::Type::Percent:
				operation = IR::Operation::Modulo;
				break;
			}

			AddInstruction(operation, node.leftOperand->Accept(*this), node.rightOperand->Accept(*this));
			m_IR.executionOrder.emplace_back(m_IR.instructions.size() - 1);
			return { IR::Address::Type::InstructionPointer, (uint32_t)m_IR.instructions.size() - 1 };
		}

		IR::Address Translator::VisitAssignment(AST::Node::Assignment &node)
		{
			const IR::Address lValue = node.lValue->Accept(*this);
			const IR::Address rValue = node.rValue->Accept(*this);

			if(node.operatorType == Token::Type::Equal)
				AddInstruction(IR::Operation::Assignment, lValue, rValue);

			m_IR.executionOrder.emplace_back(m_IR.instructions.size() - 1);
			return { IR::Address::Type::InstructionPointer, (uint32_t)m_IR.instructions.size() - 1, lValue.GetDataType() };
		}

		IR::Address Translator::VisitBranch(AST::Node::Branch &node)
		{
			const IR::Address condition = node.condition->Accept(*this);
			if(node.elseBody)
			{
				const uint32_t branchOnNotEqualExecutionIndex = m_IR.executionOrder.emplace_back(m_IR.executionOrder.size());

				node.ifBody->Accept(*this);

				const uint32_t jumpExecutionIndex = m_IR.executionOrder.emplace_back(m_IR.executionOrder.size());

				const uint32_t branchTarget = m_IR.instructions.size();
				node.elseBody->Accept(*this);

				m_IR.executionOrder[branchOnNotEqualExecutionIndex] = m_IR.instructions.size();
				AddInstruction(IR::Operation::BranchOnNotEqual, condition, IR::Address { IR::Address::Type::InstructionPointer, branchTarget });
				
				m_IR.executionOrder[jumpExecutionIndex] = m_IR.instructions.size();
				AddInstruction(IR::Operation::Branch, IR::Address { IR::Address::Type::InstructionPointer, (uint32_t)m_IR.executionOrder.size() });
			}
			else
			{
				const uint32_t branchOnNotEqualExecutionIndex = m_IR.executionOrder.emplace_back(m_IR.executionOrder.size());

				node.ifBody->Accept(*this);

				m_IR.executionOrder[branchOnNotEqualExecutionIndex] = m_IR.instructions.size();
				AddInstruction(IR::Operation::BranchOnNotEqual, condition, IR::Address { IR::Address::Type::InstructionPointer, (uint32_t)m_IR.executionOrder.size() });
			}

			return {};
		}

		IR::Address Translator::VisitReturn(AST::Node::Return &node)
		{
			Type returnType = Type::Void;
			if(node.value)
			{
				const IR::Address returnValue = node.value->Accept(*this);
				AddInstruction(IR::Operation::Return, returnValue);
				returnType = returnValue.GetDataType();
			}
			else
				AddInstruction(IR::Operation::Return);

			m_IR.executionOrder.emplace_back(m_IR.instructions.size() - 1);
			return { IR::Address::Type::InstructionPointer, (uint32_t)m_IR.instructions.size() - 1, returnType };
		}

		IR::Address Translator::GetSymbolAddress(const std::string name) const
		{
			for(auto i = m_SymbolTables.rbegin(); i != m_SymbolTables.rend(); i++)
				if(i->contains(name))
					return i->at(name);

			GM_DEBUG_ASSERT(false, "Failed to get FlexShader symbol intermediate representation address | Name \"%s\" is not registered", name.c_str());
		}

		const IR::IR &Translator::IR()
		{
			GM_DEBUG_ASSERT(m_IR.executionOrder.size() == m_IR.instructions.size(), "Failed to construct FlexShader intermediate representation");
			return m_IR;
		}
	}
}