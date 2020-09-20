#pragma once

#include "ComponentInstance.h"

namespace Gogaman
{
	namespace FlexShader
	{
		namespace AST
		{
			namespace Node
			{
				struct Abstract;
				struct Expression;
				struct Statement;
				struct StatementBlock;
				struct VariableDeclaration;
				struct ComponentInstantiation;
				struct FunctionPrototype;
				struct Function;
				struct FunctionCall;
				struct Component;
				struct BooleanLiteral;
				struct IntegerLiteral;
				struct FloatingPointLiteral;
				struct StringLiteral;
				struct Identifier;
				struct Vector;
				struct MemberSelection;
				struct BinaryOperation;
				struct Assignment;
				struct Branch;
				struct Return;
			}

			//Use CRTP class ???
			template<typename ImplementationType>
			class AbstractVisitor
			{
			public:
				inline void VisitAbstract(Node::Abstract &node)                             {}
				inline void VisitExpression(Node::Expression &node)                         {}
				inline void VisitStatement(Node::Statement &node)                           {}
				inline void VisitStatementBlock(Node::StatementBlock &node)                 {}
				inline void VisitFunctionPrototype(Node::FunctionPrototype &node)           {}
				inline void VisitFunction(Node::Function &node)                             {}
				inline void VisitFunctionCall(Node::FunctionCall &node)                     {}
				inline void VisitComponent(Node::Component &node)                           {}
				inline void VisitVariableDeclaration(Node::VariableDeclaration &node)       {}
				inline void VisitComponentInstantiation(Node::ComponentInstantiation &node) {}
				inline void VisitBooleanLiteral(Node::BooleanLiteral &node)                 {}
				inline void VisitIntegerLiteral(Node::IntegerLiteral &node)                 {}
				inline void VisitFloatingPointLiteral(Node::FloatingPointLiteral &node)     {}
				inline void VisitStringLiteral(Node::StringLiteral &node)                   {}
				inline void VisitIdentifier(Node::Identifier &node)                         {}
				inline void VisitVector(Node::Vector &node)                                 {}
				inline void VisitMemberSelection(Node::MemberSelection &node)               {}
				inline void VisitBinaryOperation(Node::BinaryOperation &node)               {}
				inline void VisitAssignment(Node::Assignment &node)                         {}
				inline void VisitBranch(Node::Branch &node)                                 {}
				inline void VisitReturn(Node::Return &node)                                 {}
			};

			class LogVisitor : public AbstractVisitor<LogVisitor>
			{
			public:
				LogVisitor()
					: m_Depth(0)
				{}

				~LogVisitor() = default;

				void VisitStatementBlock(Node::StatementBlock &node);
				void VisitFunctionPrototype(Node::FunctionPrototype &node);
				void VisitFunction(Node::Function &node);
				void VisitFunctionCall(Node::FunctionCall &node);
				void VisitComponent(Node::Component &node);
				void VisitVariableDeclaration(Node::VariableDeclaration &node);
				void VisitComponentInstantiation(Node::ComponentInstantiation &node);
				void VisitBooleanLiteral(Node::BooleanLiteral &node);
				void VisitIntegerLiteral(Node::IntegerLiteral &node);
				void VisitFloatingPointLiteral(Node::FloatingPointLiteral &node);
				void VisitStringLiteral(Node::StringLiteral &node);
				void VisitIdentifier(Node::Identifier &node);
				void VisitVector(Node::Vector &node);
				void VisitMemberSelection(Node::MemberSelection &node);
				void VisitBinaryOperation(Node::BinaryOperation &node);
				void VisitAssignment(Node::Assignment &node);
				void VisitBranch(Node::Branch &node);
				void VisitReturn(Node::Return &node);
			private:
				uint32_t m_Depth;
			};

			class SpecializationVisitor : public AbstractVisitor<SpecializationVisitor>
			{
			public:
				SpecializationVisitor(const std::vector<std::string> &componentNames = {})
					: m_ComponentNames(componentNames)
				{}

				~SpecializationVisitor() = default;

				void VisitStatementBlock(Node::StatementBlock &node);
				void VisitFunctionPrototype(Node::FunctionPrototype &node);
				void VisitFunction(Node::Function &node);
				void VisitFunctionCall(Node::FunctionCall &node);
				void VisitComponent(Node::Component &node);
				void VisitComponentInstantiation(Node::ComponentInstantiation &node);
				void VisitVector(Node::Vector &node);
				void VisitMemberSelection(Node::MemberSelection &node);
				void VisitBinaryOperation(Node::BinaryOperation &node);
				void VisitAssignment(Node::Assignment &node);
				void VisitBranch(Node::Branch &node);
				void VisitReturn(Node::Return &node);
			private:
				std::vector<std::string>                     m_ComponentNames;
				std::unordered_map<std::string, std::string> m_InterfaceComponents;
			};
		}
	}
}