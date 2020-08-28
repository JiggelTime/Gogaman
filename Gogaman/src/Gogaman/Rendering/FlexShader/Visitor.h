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
				struct NumericLiteral;
				struct StringLiteral;
				struct Identifier;
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
				inline void VisitNumericLiteral(Node::NumericLiteral &node)                 {}
				inline void VisitStringLiteral(Node::StringLiteral &node)                   {}
				inline void VisitIdentifier(Node::Identifier &node)                         {}
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
				void VisitNumericLiteral(Node::NumericLiteral &node);
				void VisitStringLiteral(Node::StringLiteral &node);
				void VisitIdentifier(Node::Identifier &node);
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
				void VisitVariableDeclaration(Node::VariableDeclaration &node);
				void VisitComponentInstantiation(Node::ComponentInstantiation &node);
				void VisitNumericLiteral(Node::NumericLiteral &node);
				void VisitStringLiteral(Node::StringLiteral &node);
				void VisitIdentifier(Node::Identifier &node);
				void VisitBinaryOperation(Node::BinaryOperation &node);
				void VisitAssignment(Node::Assignment &node);
				void VisitBranch(Node::Branch &node);
				void VisitReturn(Node::Return &node);
			private:
				std::vector<std::string>                     m_ComponentNames;
				std::unordered_map<std::string, std::string> m_InterfaceComponents;
			};
			
			class ModuleVisitor
			{
			public:
				struct ComponentData
				{
					std::vector<RHI::DescriptorGroupLayout::Binding> bindings;
					//uint8 is enough for 16 mat4. This is probably enough for any component
					uint8_t constantBufferSize = 0;
				};
			public:
				ModuleVisitor()  = default;
				~ModuleVisitor() = default;

				Node::Abstract *VisitAbstract(Node::Abstract &node, const std::string &componentName);
				Node::Abstract *VisitExpression(Node::Expression &node, const std::string &componentName);
				Node::Abstract *VisitStatement(Node::Statement &node, const std::string &componentName);
				Node::Abstract *VisitStatementBlock(Node::StatementBlock &node, const std::string &componentName);
				Node::Abstract *VisitFunctionPrototype(Node::FunctionPrototype &node, const std::string &componentName);
				Node::Abstract *VisitFunction(Node::Function &node, const std::string &componentName);
				Node::Abstract *VisitFunctionCall(Node::FunctionCall &node, const std::string &componentName);
				Node::Abstract *VisitComponent(Node::Component &node, const std::string &componentName);
				Node::Abstract *VisitVariableDeclaration(Node::VariableDeclaration &node, const std::string &componentName);
				Node::Abstract *VisitComponentInstantiation(Node::ComponentInstantiation &node, const std::string &componentName);
				Node::Abstract *VisitNumericLiteral(Node::NumericLiteral &node, const std::string &componentName);
				Node::Abstract *VisitStringLiteral(Node::StringLiteral &node, const std::string &componentName);
				Node::Abstract *VisitIdentifier(Node::Identifier &node, const std::string &componentName);
				Node::Abstract *VisitBinaryOperation(Node::BinaryOperation &node, const std::string &componentName);
				Node::Abstract *VisitAssignment(Node::Assignment &node, const std::string &componentName);
				Node::Abstract *VisitBranch(Node::Branch &node, const std::string &componentName);
				Node::Abstract *VisitReturn(Node::Return &node, const std::string &componentName);

				inline const std::unordered_map<std::string, ComponentData> &GetComponentData() const { return m_ComponentData; }
			private:
				std::unordered_map<std::string, ComponentData> m_ComponentData;

				std::unordered_map<std::string, std::vector<Node::Function *>>            m_ComponentFunctions;
				std::unordered_map<std::string, std::vector<Node::VariableDeclaration *>> m_ComponentVariableDeclarations;

				std::unordered_map<std::string, std::string> m_ResolvedComponentVariableNames;
			};
		}
	}
}