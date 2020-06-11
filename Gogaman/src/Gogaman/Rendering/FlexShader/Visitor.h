#pragma once

#include "ComponentInstance.h"

namespace Gogaman
{
	namespace FlexShader
	{
		namespace AST
		{
			enum class Type : uint8_t;

			namespace Node
			{
				struct Abstract;
				struct Expression;
				struct Statement;
				struct StatementBlock;
				struct VariableDeclaration;
				struct FunctionPrototype;
				struct Function;
				struct FunctionCall;
				struct Component;
				struct TypeDeclaration;
				struct NumericLiteral;
				struct StringLiteral;
				struct Identifier;
				struct BinaryOperation;
				struct Assignment;
			}

			//Use CRTP class ???
			template<typename ImplementationType>
			class AbstractVisitor
			{
			public:
				inline void VisitAbstract(Node::Abstract &node)                       {}
				inline void VisitExpression(Node::Expression &node)                   {}
				inline void VisitStatement(Node::Statement &node)                     {}
				inline void VisitStatementBlock(Node::StatementBlock &node)           {}
				inline void VisitFunctionPrototype(Node::FunctionPrototype &node)     {}
				inline void VisitFunction(Node::Function &node)                       {}
				inline void VisitFunctionCall(Node::FunctionCall &node)               {}
				inline void VisitComponent(Node::Component &node)                     {}
				inline void VisitVariableDeclaration(Node::VariableDeclaration &node) {}
				inline void VisitTypeDeclaration(Node::TypeDeclaration &node)         {}
				inline void VisitNumericLiteral(Node::NumericLiteral &node)           {}
				inline void VisitStringLiteral(Node::StringLiteral &node)             {}
				inline void VisitIdentifier(Node::Identifier &node)                   {}
				inline void VisitBinaryOperation(Node::BinaryOperation &node)         {}
				inline void VisitAssignment(Node::Assignment &node)                   {}
			};

			class PrintVisitor : public AbstractVisitor<PrintVisitor>
			{
			public:
				PrintVisitor()
					: m_Depth(0)
				{}

				~PrintVisitor() = default;
				
				void VisitStatementBlock(Node::StatementBlock &node);
				void VisitFunctionPrototype(Node::FunctionPrototype &node);
				void VisitFunction(Node::Function &node);
				void VisitFunctionCall(Node::FunctionCall &node);
				void VisitComponent(Node::Component &node);
				void VisitVariableDeclaration(Node::VariableDeclaration &node);
				void VisitNumericLiteral(Node::NumericLiteral &node);
				void VisitStringLiteral(Node::StringLiteral &node);
				void VisitIdentifier(Node::Identifier &node);
				void VisitBinaryOperation(Node::BinaryOperation &node);
				void VisitAssignment(Node::Assignment &node);
			private:
				uint32_t m_Depth;
			};

			//Visitor with symbol table(s) that verifies that functions/variables/(maybe)components are declared before use
			class ValidationVisitor
			{
			public:
				ValidationVisitor()
					: m_CurrentNamespaceDepth(0)
				{}

				~ValidationVisitor() = default;

				//Non-terminals
				//void VisitStatementBlock(Node::StatementBlock &node);
				//void VisitFunction(Node::Function &node);
				//void VisitComponent(Node::Component &node);
				//void VisitBinaryOperation(Node::BinaryOperation &node);
				//void VisitAssignment(Node::Assignment &node);

				//Terminal
				//void VisitVariableDeclaration(Node::VariableDeclaration &node);

				Type VisitAbstract(Node::Abstract &node);
				Type VisitExpression(Node::Expression &node);
				Type VisitStatement(Node::Statement &node);
				Type VisitStatementBlock(Node::StatementBlock &node);
				Type VisitFunctionPrototype(Node::FunctionPrototype &node);
				Type VisitFunction(Node::Function &node);
				Type VisitFunctionCall(Node::FunctionCall &node);
				Type VisitComponent(Node::Component &node);
				Type VisitVariableDeclaration(Node::VariableDeclaration &node);
				Type VisitTypeDeclaration(Node::TypeDeclaration &node);
				Type VisitNumericLiteral(Node::NumericLiteral &node);
				Type VisitStringLiteral(Node::StringLiteral &node);
				Type VisitIdentifier(Node::Identifier &node);
				Type VisitBinaryOperation(Node::BinaryOperation &node);
				Type VisitAssignment(Node::Assignment &node);

				Type GetType(const std::string &symbol);

				//Can remove. Only needed for debugging
				inline constexpr const std::unordered_map<std::string, Type> &GetSymbolTable(const uint8_t depth) const { return m_SymbolTables[depth]; }
			private:
				//Global, component, function namespaces
				std::unordered_map<std::string, Type> m_SymbolTables[3];
				uint8_t                               m_CurrentNamespaceDepth;
			};

			class ModuleVisitor : public AbstractVisitor<ModuleVisitor>
			{
			public:
				ModuleVisitor()
					: m_IsVisitingComponent(false)
				//	: m_Depth(0)
				{}

				~ModuleVisitor() = default;

				void VisitStatementBlock(Node::StatementBlock &node);
				void VisitFunctionPrototype(Node::FunctionPrototype &node);
				void VisitFunction(Node::Function &node);
				void VisitFunctionCall(Node::FunctionCall &node);
				void VisitComponent(Node::Component &node);
				void VisitVariableDeclaration(Node::VariableDeclaration &node);
				void VisitNumericLiteral(Node::NumericLiteral &node);
				void VisitStringLiteral(Node::StringLiteral &node);
				void VisitIdentifier(Node::Identifier &node);
				void VisitBinaryOperation(Node::BinaryOperation &node);
				void VisitAssignment(Node::Assignment &node);
			private:
				//uint32_t m_Depth;
				std::unordered_map<std::string, ComponentTypeID> m_ComponentTypeIDs;
				//Maybe a stack instead of vector?
				std::vector<Component> m_Components;
				bool m_IsVisitingComponent;
			};
		}
	}
}