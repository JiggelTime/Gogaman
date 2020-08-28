#include "pch.h"
#include "Visitor.h"

#include "Gogaman/Core/Logging/Log.h"

#include "Type.h"
#include "AbstractSyntaxTree.h"

namespace Gogaman
{
	namespace FlexShader
	{
		namespace AST
		{
			void LogVisitor::VisitStatementBlock(Node::StatementBlock &node)
			{
				std::string spaces;
				spaces.append(m_Depth++ * 2, ' ');

				GM_LOG_CORE_INFO("%sStatement block | Statements:", spaces.c_str());

				for(auto i : node.statements)
					i->Accept(*this);

				m_Depth--;
			}

			void LogVisitor::VisitFunctionPrototype(Node::FunctionPrototype &node)
			{
				std::string spaces;
				spaces.append(m_Depth++ * 2, ' ');

				if(node.parameters.empty())
				{
					GM_LOG_CORE_INFO("%sFunction Prototype | Name: %s | Return type: %s", spaces.c_str(), node.name.c_str(), GetTypeString(node.returnType).c_str());
				}
				else
				{
					GM_LOG_CORE_INFO("%sFunction Prototype | Name: %s | Return type: %s | Parameters:", spaces.c_str(), node.name.c_str(), GetTypeString(node.returnType).c_str());
					for(const auto i : node.parameters)
						i->Accept(*this);
				}

				m_Depth--;
			}

			void LogVisitor::VisitFunction(Node::Function &node)
			{
				std::string spaces;
				spaces.append(m_Depth++ * 2, ' ');

				GM_LOG_CORE_INFO("%sFunction", spaces.c_str());
				node.prototype->Accept(*this);

				m_Depth++;
				if(node.body)
				{
					GM_LOG_CORE_INFO("%s  Body:", spaces.c_str());
					node.body->Accept(*this);
				}

				m_Depth -= 2;
			}

			void LogVisitor::VisitFunctionCall(Node::FunctionCall &node)
			{
				std::string spaces;
				spaces.append(m_Depth++ * 2, ' ');

				if(node.parameters.empty())
				{
					GM_LOG_CORE_INFO("%sFunction call | Name: %s", spaces.c_str(), node.name.c_str());
				}
				else
				{
					GM_LOG_CORE_INFO("%sFunction call | Name: %s | Parameters:", spaces.c_str(), node.name.c_str());
					for(auto i : node.parameters)
						i->Accept(*this);
				}

				m_Depth--;
			}

			void LogVisitor::VisitComponent(Node::Component &node)
			{
				std::string spaces;
				spaces.append(m_Depth++ * 2, ' ');

				if(node.interfaceName.empty())
				{
					GM_LOG_CORE_INFO("%sComponent | Name: %s", spaces.c_str(), node.name.c_str());
				}
				else
					GM_LOG_CORE_INFO("%sComponent | Name: %s | Interface: %s", spaces.c_str(), node.name.c_str(), node.interfaceName.c_str());

				m_Depth++;
				if(node.body)
				{
					GM_LOG_CORE_INFO("%s  Body:", spaces.c_str());
					node.body->Accept(*this);
				}

				m_Depth -= 2;
			}

			void LogVisitor::VisitVariableDeclaration(Node::VariableDeclaration &node)
			{
				std::string spaces;
				spaces.append(m_Depth * 2, ' ');

				GM_LOG_CORE_INFO("%sVariable declaration | Name: %s | Type: %s", spaces.c_str(), node.name.c_str(), GetTypeString(node.type).c_str());
			}

			void LogVisitor::VisitComponentInstantiation(Node::ComponentInstantiation &node)
			{
				std::string spaces;
				spaces.append(m_Depth * 2, ' ');

				GM_LOG_CORE_INFO("%sComponent instantiation | Name: %s | Typename: %s", spaces.c_str(), node.name.c_str(), node.typeName.c_str());
			}

			void LogVisitor::VisitNumericLiteral(Node::NumericLiteral &node)
			{
				std::string spaces;
				spaces.append(m_Depth * 2, ' ');

				GM_LOG_CORE_INFO("%sNumeric literal | Value: %s", spaces.c_str(), std::string(node.value).c_str());
			}

			void LogVisitor::VisitStringLiteral(Node::StringLiteral &node)
			{
				std::string spaces;
				spaces.append(m_Depth * 2, ' ');

				GM_LOG_CORE_INFO("%sString literal | Value: %s", spaces.c_str(), std::string(node.value).c_str());
			}

			void LogVisitor::VisitIdentifier(Node::Identifier &node)
			{
				std::string spaces;
				spaces.append(m_Depth * 2, ' ');

				GM_LOG_CORE_INFO("%sIdentifier | Name: %s", spaces.c_str(), std::string(node.name).c_str());
			}

			void LogVisitor::VisitBinaryOperation(Node::BinaryOperation &node)
			{
				std::string spaces;
				spaces.append(m_Depth++ * 2, ' ');

				GM_LOG_CORE_INFO("%sBinary operation | Type: %s", spaces.c_str(), GetTokenTypeString(node.operatorType).c_str());
				node.leftOperand->Accept(*this);
				node.rightOperand->Accept(*this);

				m_Depth--;
			}

			void LogVisitor::VisitAssignment(Node::Assignment &node)
			{
				std::string spaces;
				spaces.append(m_Depth++ * 2, ' ');

				GM_LOG_CORE_INFO("%sAssignment | Operator: %s | Left operand:", spaces.c_str(), GetTokenTypeString(node.operatorType).c_str());
				node.lValue->Accept(*this);

				m_Depth += 2;

				GM_LOG_CORE_INFO("%s  Right operand:", spaces.c_str());
				node.rValue->Accept(*this);

				m_Depth -= 3;
			}

			void LogVisitor::VisitBranch(Node::Branch &node)
			{
				std::string spaces;
				spaces.append(m_Depth++ * 2, ' ');

				GM_LOG_CORE_INFO("%sBranch | Condition:", spaces.c_str());
				node.condition->Accept(*this);

				m_Depth += 2;

				GM_LOG_CORE_INFO("%s  If body:", spaces.c_str());
				node.ifBody->Accept(*this);

				if(node.elseBody)
				{
					GM_LOG_CORE_INFO("%s  Else body:", spaces.c_str());
					node.elseBody->Accept(*this);
				}

				m_Depth -= 3;
			}

			void LogVisitor::VisitReturn(Node::Return &node)
			{
				std::string spaces;
				spaces.append(m_Depth++ * 2, ' ');

				if(node.value)
				{
					GM_LOG_CORE_INFO("%sReturn | Value:", spaces.c_str());
					node.value->Accept(*this);
				}
				else
					GM_LOG_CORE_INFO("%sReturn", spaces.c_str());

				m_Depth--;
			}

			void SpecializationVisitor::VisitStatementBlock(Node::StatementBlock &node)
			{
				for(auto i : node.statements)
					i->Accept(*this);
			}

			void SpecializationVisitor::VisitFunctionPrototype(Node::FunctionPrototype &node)
			{
				for(const auto i : node.parameters)
					i->Accept(*this);
			}

			void SpecializationVisitor::VisitFunction(Node::Function &node)
			{
				node.prototype->Accept(*this);
				node.body->Accept(*this);
			}

			void SpecializationVisitor::VisitFunctionCall(Node::FunctionCall &node)
			{
				const auto periodIndex = node.name.find_first_of('.');
				if(periodIndex != std::string::npos)
					node.name = '$' + node.name.substr(0, periodIndex) + '$' + node.name.substr(periodIndex + 1);

				for(auto i : node.parameters)
					i->Accept(*this);
			}

			void SpecializationVisitor::VisitComponent(Node::Component &node)
			{
				//assert m_InterfaceComponents[node.interfaceName].empty()

				for(const auto &i : m_ComponentNames)
				{
					if(i == node.name)
						m_InterfaceComponents[node.interfaceName] = node.name;

					break;
				}

				node.body->Accept(*this);
			}

			void SpecializationVisitor::VisitVariableDeclaration(Node::VariableDeclaration &node) {}

			void SpecializationVisitor::VisitComponentInstantiation(Node::ComponentInstantiation &node)
			{
				for(const auto &i : m_ComponentNames)
				{
					if(!m_InterfaceComponents[node.typeName].empty() && (m_InterfaceComponents[node.typeName] == i))
						node.typeName = m_InterfaceComponents[node.typeName];

					break;
				}
			}

			void SpecializationVisitor::VisitNumericLiteral(Node::NumericLiteral &node) {}
			void SpecializationVisitor::VisitStringLiteral(Node::StringLiteral &node)   {}
			void SpecializationVisitor::VisitIdentifier(Node::Identifier &node)         {}

			void SpecializationVisitor::VisitBinaryOperation(Node::BinaryOperation &node)
			{
				node.leftOperand->Accept(*this);
				node.rightOperand->Accept(*this);
			}

			void SpecializationVisitor::VisitAssignment(Node::Assignment &node)
			{
				node.lValue->Accept(*this);
				node.rValue->Accept(*this);
			}

			void SpecializationVisitor::VisitBranch(Node::Branch &node)
			{
				node.condition->Accept(*this);
				node.ifBody->Accept(*this);
				if(node.elseBody)
					node.elseBody->Accept(*this);
			}

			void SpecializationVisitor::VisitReturn(Node::Return &node)
			{
				if(node.value)
					node.value->Accept(*this);
			}

			Node::Abstract *ModuleVisitor::VisitAbstract(Node::Abstract &node, const std::string &componentName) { return &node; }

			Node::Abstract *ModuleVisitor::VisitExpression(Node::Expression &node, const std::string &componentName) { return &node; }

			Node::Abstract *ModuleVisitor::VisitStatement(Node::Statement &node, const std::string &componentName) { return &node; }

			Node::Abstract *ModuleVisitor::VisitStatementBlock(Node::StatementBlock &node, const std::string &componentName)
			{
				Node::StatementBlock *block = new Node::StatementBlock;
				for(const auto i : node.statements)
				{
					Node::Statement *statement = static_cast<Node::Statement *>(i->Accept(*this, componentName));
					if(statement)
						block->statements.emplace_back(statement);
				}

				return block->statements.empty() ? nullptr : (block->statements.size() == 1 ? block->statements.front() : block);
			}

			Node::Abstract *ModuleVisitor::VisitFunctionPrototype(Node::FunctionPrototype &node, const std::string &componentName) { return &node; }

			Node::Abstract *ModuleVisitor::VisitFunction(Node::Function &node, const std::string &componentName)
			{
				//node.prototype = static_cast<Node::FunctionPrototype *>(node.prototype->Accept(*this, componentName));

				if(!componentName.empty())
				{
					m_ComponentFunctions[componentName].emplace_back(&node);
					return nullptr;
				}

				return &node;
			}

			Node::Abstract *ModuleVisitor::VisitFunctionCall(Node::FunctionCall &node, const std::string &componentName) { return &node; }

			Node::Abstract *ModuleVisitor::VisitComponent(Node::Component &node, const std::string &componentName)
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

			Node::Abstract *ModuleVisitor::VisitVariableDeclaration(Node::VariableDeclaration &node, const std::string &componentName)
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

					m_ComponentVariableDeclarations[componentName].emplace_back(&node);
					return nullptr;
				}

				return &node;
			}

			Node::Abstract *ModuleVisitor::VisitComponentInstantiation(Node::ComponentInstantiation &node, const std::string &componentName)
			{
				Node::StatementBlock *block = new Node::StatementBlock;

				for(Node::VariableDeclaration *i : m_ComponentVariableDeclarations[node.typeName])
				{
					Node::VariableDeclaration *variableDeclaration = static_cast<Node::VariableDeclaration *>(block->statements.emplace_back(new Node::VariableDeclaration));
					variableDeclaration->name = '$' + node.name + '$' + i->name;
					m_ResolvedComponentVariableNames[i->name] = variableDeclaration->name;

					variableDeclaration->type = i->type;
				}

				for(Node::Function *i : m_ComponentFunctions[node.typeName])
				{
					Node::Function *function = static_cast<Node::Function *>(block->statements.emplace_back(new Node::Function));
					function->prototype = i->prototype;
					function->prototype->name = '$' + node.name + '$' + i->prototype->name;
					function->body = static_cast<Node::StatementBlock *>(i->body->Accept(*this));
				}

				return block;
			}

			Node::Abstract *ModuleVisitor::VisitNumericLiteral(Node::NumericLiteral &node, const std::string &componentName) { return &node; }
			Node::Abstract *ModuleVisitor::VisitStringLiteral(Node::StringLiteral &node, const std::string &componentName)   { return &node; }

			Node::Abstract *ModuleVisitor::VisitIdentifier(Node::Identifier &node, const std::string &componentName)
			{
				if(!componentName.empty())
					for(const Node::VariableDeclaration *i : m_ComponentVariableDeclarations[componentName])
						if(i->name == node.name)
							node.name = '$' + componentName + '$' + node.name;

				if(m_ResolvedComponentVariableNames.contains(node.name))
					node.name = m_ResolvedComponentVariableNames[node.name];

				return &node;
			}

			Node::Abstract *ModuleVisitor::VisitBinaryOperation(Node::BinaryOperation &node, const std::string &componentName)
			{
				node.leftOperand  = static_cast<Node::Expression *>(node.leftOperand->Accept(*this, componentName));
				node.rightOperand = static_cast<Node::Expression *>(node.rightOperand->Accept(*this, componentName));
				return &node;
			}

			Node::Abstract *ModuleVisitor::VisitAssignment(Node::Assignment &node, const std::string &componentName)
			{
				node.lValue = static_cast<Node::Identifier *>(node.lValue->Accept(*this, componentName));
				node.rValue = static_cast<Node::Expression *>(node.rValue->Accept(*this, componentName));
				return &node;
			}

			Node::Abstract *ModuleVisitor::VisitBranch(Node::Branch &node, const std::string &componentName)
			{
				node.condition    = static_cast<Node::Expression *>(node.condition->Accept(*this, componentName));
				node.ifBody       = static_cast<Node::Statement *>(node.ifBody->Accept(*this, componentName));
				if(node.elseBody)
					node.elseBody = static_cast<Node::Statement *>(node.elseBody->Accept(*this, componentName));
				return &node;
			}

			Node::Abstract *ModuleVisitor::VisitReturn(Node::Return &node, const std::string &componentName)
			{
				if(node.value)
					node.value = static_cast<Node::Expression *>(node.value->Accept(*this, componentName));
				return &node;
			}
		}
	}
}