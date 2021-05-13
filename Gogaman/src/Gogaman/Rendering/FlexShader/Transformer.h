#pragma once

#include "Visitor.h"

namespace Gogaman
{
	namespace FlexShader
	{
		namespace AST
		{
			class Transformer
			{
			public:
				struct ComponentData
				{
					std::vector<RHI::DescriptorGroupLayout::Binding> bindings;
					//uint8 is enough for 16 mat4. This is probably enough for any component
					uint8_t constantBufferSize = 0;
				};

				struct ComponentInlineData
				{
					std::vector<Node::Function *>            functions;
					std::vector<Node::VariableDeclaration *> variables;
				};
			public:
				Transformer()  = default;
				~Transformer() = default;

				inline std::vector<Node::Abstract *> VisitAbstract(Node::Abstract &node, const std::string &componentName)     { return { (Node::Abstract *)&node }; }
				inline std::vector<Node::Abstract *> VisitExpression(Node::Expression &node, const std::string &componentName) { return { (Node::Abstract *)&node }; }
				inline std::vector<Node::Abstract *> VisitStatement(Node::Statement &node, const std::string &componentName)   { return { (Node::Abstract *)&node }; }
				std::vector<Node::Abstract *> VisitStatementBlock(Node::StatementBlock &node, const std::string &componentName);
				inline std::vector<Node::Abstract *> VisitFunctionPrototype(Node::FunctionPrototype &node, const std::string &componentName) { return { (Node::Abstract *)&node }; }
				std::vector<Node::Abstract *> VisitFunction(Node::Function &node, const std::string &componentName);
				inline std::vector<Node::Abstract *> VisitFunctionCall(Node::FunctionCall &node, const std::string &componentName) { return { (Node::Abstract *)&node }; }
				std::vector<Node::Abstract *> VisitComponent(Node::Component &node, const std::string &componentName);
				std::vector<Node::Abstract *> VisitVariableDeclaration(Node::VariableDeclaration &node, const std::string &componentName);
				std::vector<Node::Abstract *> VisitComponentInstantiation(Node::ComponentInstantiation &node, const std::string &componentName);
				inline std::vector<Node::Abstract *> VisitBooleanLiteral(Node::BooleanLiteral &node, const std::string &componentName)             { return { (Node::Abstract *)&node }; }
				inline std::vector<Node::Abstract *> VisitIntegerLiteral(Node::IntegerLiteral &node, const std::string &componentName)             { return { (Node::Abstract *)&node }; }
				inline std::vector<Node::Abstract *> VisitFloatingPointLiteral(Node::FloatingPointLiteral &node, const std::string &componentName) { return { (Node::Abstract *)&node }; }
				inline std::vector<Node::Abstract *> VisitStringLiteral(Node::StringLiteral &node, const std::string &componentName)               { return { (Node::Abstract *)&node }; }
				std::vector<Node::Abstract *> VisitIdentifier(Node::Identifier &node, const std::string &componentName);
				std::vector<Node::Abstract *> VisitVector(Node::Vector &node, const std::string &componentName);
				std::vector<Node::Abstract *> VisitMemberSelection(Node::MemberSelection &node, const std::string &componentName);
				std::vector<Node::Abstract *> VisitBinaryOperation(Node::BinaryOperation &node, const std::string &componentName);
				std::vector<Node::Abstract *> VisitAssignment(Node::Assignment &node, const std::string &componentName);
				std::vector<Node::Abstract *> VisitBranch(Node::Branch &node, const std::string &componentName);
				std::vector<Node::Abstract *> VisitReturn(Node::Return &node, const std::string &componentName);

				inline const std::unordered_map<std::string, ComponentData> &GetComponentData() const { return m_ComponentData; }
			private:
				std::unordered_map<std::string, ComponentData> m_ComponentData;

				std::unordered_map<std::string, ComponentInlineData> m_ComponentInlineData;

				std::unordered_map<std::string, std::string> m_ComponentInstanceTypeNames;

				//std::unordered_map<std::string, std::string> m_ResolvedComponentVariableNames;
			};
		}
	}
}