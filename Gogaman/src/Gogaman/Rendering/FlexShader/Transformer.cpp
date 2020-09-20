#include "pch.h"
#include "Transformer.h"

#include "Gogaman/Core/Logging/Log.h"

#include "Type.h"
#include "AbstractSyntaxTree.h"

namespace Gogaman
{
	namespace FlexShader
	{
		namespace AST
		{
			std::vector<Node::Abstract *> Transformer::VisitStatementBlock(Node::StatementBlock &node, const std::string &componentName)
			{
				Node::StatementBlock *block = new Node::StatementBlock;
				for(const auto i : node.statements)
					for(const Node::Abstract *i : i->Accept(*this, componentName))
						block->statements.emplace_back((Node::Statement *)i);

				if(block->statements.size())
					return { block };
				else
					return {};
			}

			std::vector<Node::Abstract *> Transformer::VisitFunction(Node::Function &node, const std::string &componentName)
			{
				//node.prototype = static_cast<Node::FunctionPrototype *>(node.prototype->Accept(*this, componentName));

				if(!componentName.empty())
				{
					m_ComponentInlineData[componentName].functions.emplace_back(&node);
					return {};
				}

				node.body = static_cast<Node::StatementBlock *>(node.body->Accept(*this).front());
				return { &node };
			}

			std::vector<Node::Abstract *> Transformer::VisitComponent(Node::Component &node, const std::string &componentName)
			{
				//Assert component name is empty

				//Group all "normal" types into one constant buffer
				if(m_ComponentData[node.name].constantBufferSize)
				{
					RHI::DescriptorGroupLayout::Binding &binding = m_ComponentData[componentName].bindings.emplace_back();
					binding.type = RHI::DescriptorHeap::Type::ShaderConstantBuffer;

					GM_LOG_CORE_INFO("Component %s | Constant buffer size: %d", node.name.c_str(), m_ComponentData[node.name].constantBufferSize);
				}

				return node.body->Accept(*this, node.name);
			}

			std::vector<Node::Abstract *> Transformer::VisitVariableDeclaration(Node::VariableDeclaration &node, const std::string &componentName)
			{
				if(!componentName.empty())
				{
					switch(node.type)
					{
					case Type::Integer:
					case Type::Float:
						m_ComponentData[componentName].constantBufferSize += 4;
						break;
					case Type::Float2:
						m_ComponentData[componentName].constantBufferSize += 8;
						break;
					case Type::Float3:
						m_ComponentData[componentName].constantBufferSize += 12;
						break;
					case Type::Float4:
						m_ComponentData[componentName].constantBufferSize += 16;
						break;
					case Type::Sampler:
					{
						RHI::DescriptorGroupLayout::Binding &binding = m_ComponentData[componentName].bindings.emplace_back();
						binding.type = RHI::DescriptorHeap::Type::Sampler;
					}
						break;
					default:
						GM_DEBUG_ASSERT(false, "Invalid FlexShader component member variable declaration | Type \"%s\" is invalid | Component: \"%s\" | Name: \"%s\"", GetTypeString(node.type).c_str(), componentName.c_str(), node.name.c_str());
					}

					GM_DEBUG_ASSERT(m_ComponentData.size() < 17, "Invalid FlexShader component member variable declaration");
					node.specifier = (VariableSpecifier)(m_ComponentData.size() - 1);

					m_ComponentInlineData[componentName].variables.emplace_back(&node);
					return {};
				}

				return { &node };
			}

			std::vector<Node::Abstract *> Transformer::VisitComponentInstantiation(Node::ComponentInstantiation &node, const std::string &componentName)
			{
				const ComponentInlineData &inlineData = m_ComponentInlineData[node.typeName];

				std::vector<Node::Abstract *> statements;
				statements.reserve(inlineData.functions.size() + inlineData.variables.size());

				for(Node::Function *i : inlineData.functions)
				{
					Node::Function *function = static_cast<Node::Function *>(statements.emplace_back(new Node::Function));
					function->prototype       = i->prototype;
					function->prototype->name = '$' + node.name + '$' + i->prototype->name;
					function->body            = static_cast<Node::StatementBlock *>(i->body->Accept(*this).front());
				}

				for(Node::VariableDeclaration *i : inlineData.variables)
				{
					Node::VariableDeclaration *variableDeclaration = static_cast<Node::VariableDeclaration *>(statements.emplace_back(new Node::VariableDeclaration));
					variableDeclaration->name      = '$' + node.name + '$' + i->name;
					variableDeclaration->type      = i->type;
					variableDeclaration->specifier = i->specifier;
				}

				m_ComponentInstanceTypeNames[node.name] = node.typeName;
				return statements;
			}

			std::vector<Node::Abstract *> Transformer::VisitIdentifier(Node::Identifier &node, const std::string &componentName)
			{
				//If inside a component member function
				if(!componentName.empty())
					for(const Node::VariableDeclaration *i : m_ComponentInlineData[componentName].variables)
						if(i->name == node.name)
						{
							node.name = '$' + componentName + '$' + node.name;
							break;
						}

				return { &node };
			}

			std::vector<Node::Abstract *> Transformer::VisitVector(Node::Vector &node, const std::string &componentName)
			{
				Node::Vector *list = new Node::Vector;
				for(const auto i : node.expressions)
					for(const Node::Abstract *i : i->Accept(*this, componentName))
						list->expressions.emplace_back((Node::Expression *)i);

				return { list };
			}

			std::vector<Node::Abstract *> Transformer::VisitMemberSelection(Node::MemberSelection &node, const std::string &componentName)
			{
				if(m_ComponentInstanceTypeNames.contains(node.object->name))
					for(const Node::VariableDeclaration *i : m_ComponentInlineData[m_ComponentInstanceTypeNames[node.object->name]].variables)
						if(i->name == node.memberName)
						{
							Node::Identifier *identifier = new Node::Identifier;
							identifier->name = '$' + node.object->name + '$' + node.memberName;
							return { identifier };
						}

				return { &node };
			}

			std::vector<Node::Abstract *> Transformer::VisitBinaryOperation(Node::BinaryOperation &node, const std::string &componentName)
			{
				node.leftOperand  = static_cast<Node::Expression *>(node.leftOperand->Accept(*this, componentName).front());
				node.rightOperand = static_cast<Node::Expression *>(node.rightOperand->Accept(*this, componentName).front());
				return { &node };
			}

			std::vector<Node::Abstract *> Transformer::VisitAssignment(Node::Assignment &node, const std::string &componentName)
			{
				node.lValue = static_cast<Node::Identifier *>(node.lValue->Accept(*this, componentName).front());
				node.rValue = static_cast<Node::Expression *>(node.rValue->Accept(*this, componentName).front());
				return { &node };
			}

			std::vector<Node::Abstract *> Transformer::VisitBranch(Node::Branch &node, const std::string &componentName)
			{
				node.condition    = static_cast<Node::Expression *>(node.condition->Accept(*this, componentName).front());
				node.ifBody       = static_cast<Node::Statement *>(node.ifBody->Accept(*this, componentName).front());
				if(node.elseBody)
					node.elseBody = static_cast<Node::Statement *>(node.elseBody->Accept(*this, componentName).front());
				return { &node };
			}

			std::vector<Node::Abstract *> Transformer::VisitReturn(Node::Return &node, const std::string &componentName)
			{
				if(node.value)
					node.value = static_cast<Node::Expression *>(node.value->Accept(*this, componentName).front());
				return { &node };
			}
		}
	}
}