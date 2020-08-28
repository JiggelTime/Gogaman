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
			const IR::Address address = { m_FunctionCount++, IR::Address::Type::Function };
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
			return { (uint32_t)m_IR.instructions.size() - 1, IR::Address::Type::InstructionPointer };
		}

		IR::Address Translator::VisitVariableDeclaration(AST::Node::VariableDeclaration &node)
		{
			const IR::Address address = { m_VariableCount++, IR::Address::Type::Variable };
			RegisterSymbolAddress(node.name, address);
			m_IR.variableSymbolTable.emplace_back(node.type, node.specifierFlags);
			return address;
		}

		IR::Address Translator::VisitNumericLiteral(AST::Node::NumericLiteral &node)
		{
			union ConstantValue
			{
				float    floatValue;
				uint32_t uint32Value;
			};

			const ConstantValue value = { stof(node.value) };
			for(uint32_t i = 0; i < m_IR.constantValues.size(); i++)
				if(value.uint32Value == m_IR.constantValues[i])
					return { i, IR::Address::Type::Constant };

			m_IR.constantValues.emplace_back(value.uint32Value);
			return { m_ConstantCount++, IR::Address::Type::Constant };
		}

		IR::Address Translator::VisitStringLiteral(AST::Node::StringLiteral &node)
		{
			return {};
		}

		IR::Address Translator::VisitIdentifier(AST::Node::Identifier &node)
		{
			return GetSymbolAddress(node.name);
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
			return { (uint32_t)m_IR.instructions.size() - 1, IR::Address::Type::InstructionPointer };
		}

		IR::Address Translator::VisitAssignment(AST::Node::Assignment &node)
		{
			if(node.operatorType == Token::Type::Equal)
				AddInstruction(IR::Operation::Assignment, node.lValue->Accept(*this), node.rValue->Accept(*this));

			m_IR.executionOrder.emplace_back(m_IR.instructions.size() - 1);
			return { (uint32_t)m_IR.instructions.size() - 1, IR::Address::Type::InstructionPointer };
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
				AddInstruction(IR::Operation::BranchOnNotEqual, condition, { branchTarget, IR::Address::Type::InstructionPointer });
				
				m_IR.executionOrder[jumpExecutionIndex] = m_IR.instructions.size();
				AddInstruction(IR::Operation::Branch, { (uint32_t)m_IR.executionOrder.size(), IR::Address::Type::InstructionPointer });
			}
			else
			{
				const uint32_t branchOnNotEqualExecutionIndex = m_IR.executionOrder.emplace_back(m_IR.executionOrder.size());

				node.ifBody->Accept(*this);

				m_IR.executionOrder[branchOnNotEqualExecutionIndex] = m_IR.instructions.size();
				AddInstruction(IR::Operation::BranchOnNotEqual, condition, { (uint32_t)m_IR.executionOrder.size(), IR::Address::Type::InstructionPointer });
			}

			return {};
		}

		IR::Address Translator::VisitReturn(AST::Node::Return &node)
		{
			if(node.value)
				AddInstruction(IR::Operation::Return, node.value->Accept(*this));
			else
				AddInstruction(IR::Operation::Return);

			m_IR.executionOrder.emplace_back(m_IR.instructions.size() - 1);
			return { (uint32_t)m_IR.instructions.size() - 1, IR::Address::Type::InstructionPointer };
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