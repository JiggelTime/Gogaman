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

				GM_LOG_CORE_INFO("%sVariable declaration | Name: %s | Type: %s | Specifier: %s", spaces.c_str(), node.name.c_str(), GetTypeString(node.type).c_str(), GetVariableSpecifierString(node.specifier).c_str());
			}

			void LogVisitor::VisitComponentInstantiation(Node::ComponentInstantiation &node)
			{
				std::string spaces;
				spaces.append(m_Depth * 2, ' ');

				GM_LOG_CORE_INFO("%sComponent instantiation | Name: %s | Typename: %s", spaces.c_str(), node.name.c_str(), node.typeName.c_str());
			}

			void LogVisitor::VisitBooleanLiteral(Node::BooleanLiteral &node)
			{
				std::string spaces;
				spaces.append(m_Depth * 2, ' ');

				GM_LOG_CORE_INFO("%sBoolean literal | Value: %s", spaces.c_str(), node.value ? "true" : "false");
			}

			void LogVisitor::VisitIntegerLiteral(Node::IntegerLiteral &node)
			{
				std::string spaces;
				spaces.append(m_Depth * 2, ' ');

				GM_LOG_CORE_INFO("%sInteger literal | Value: %d", spaces.c_str(), node.value);
			}

			void LogVisitor::VisitFloatingPointLiteral(Node::FloatingPointLiteral &node)
			{
				std::string spaces;
				spaces.append(m_Depth * 2, ' ');

				GM_LOG_CORE_INFO("%sFloating point literal | Value: %f", spaces.c_str(), node.value);
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

			void LogVisitor::VisitVector(Node::Vector &node)
			{
				std::string spaces;
				spaces.append(m_Depth++ * 2, ' ');

				GM_LOG_CORE_INFO("%sVector | Values:", spaces.c_str());

				for(auto i : node.expressions)
					i->Accept(*this);

				m_Depth--;
			}

			void LogVisitor::VisitMemberSelection(Node::MemberSelection &node)
			{
				std::string spaces;
				spaces.append(m_Depth++ * 2, ' ');

				GM_LOG_CORE_INFO("%sMember access | Member name: %s | Object:", spaces.c_str(), node.memberName.c_str());

				node.object->Accept(*this);

				m_Depth--;
			}

			void LogVisitor::VisitBinaryOperation(Node::BinaryOperation &node)
			{
				std::string spaces;
				spaces.append(m_Depth++ * 2, ' ');

				GM_LOG_CORE_INFO("%sBinary operation | Operator: %s", spaces.c_str(), GetTokenTypeString(node.operatorType).c_str());
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

			void SpecializationVisitor::VisitComponentInstantiation(Node::ComponentInstantiation &node)
			{
				for(const auto &i : m_ComponentNames)
				{
					if(!m_InterfaceComponents[node.typeName].empty() && (m_InterfaceComponents[node.typeName] == i))
						node.typeName = m_InterfaceComponents[node.typeName];

					break;
				}
			}

			void SpecializationVisitor::VisitVector(Node::Vector &node)
			{
				for(auto i : node.expressions)
					i->Accept(*this);
			}

			void SpecializationVisitor::VisitMemberSelection(Node::MemberSelection &node)
			{
				//SUTFF !~!!!!!!!!!!!!!!
			}

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
		}
	}
}