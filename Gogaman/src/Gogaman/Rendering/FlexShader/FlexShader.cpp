#include "pch.h"
#include "FlexShader.h"

#include "Lexer.h"

#include "Visitor.h"
#include "AbstractSyntaxTree.h"
#include "SyntacticAnalysis.h"

#include "IntermediateRepresentation.h"

#include "StaticSingleAssignment.h"

#include "Optimizer.h"

#include "CodeGenerator.h"

#define GM_FLEX_SHADER_ENABLE_AST_LOGGING FALSE
#define GM_FLEX_SHADER_ENABLE_IR_LOGGING  TRUE

namespace Gogaman
{
	namespace FlexShader
	{
		Module::Module(const std::string &source)
		{
			m_RootNode = Parse(Lex(source));

			#if GM_FLEX_SHADER_ENABLE_AST_LOGGING
				AST::LogVisitor logger;
				m_RootNode->Accept(logger);
			#endif
		}

		const std::pair<uint32_t, const uint8_t *> Module::CompileKernel(const std::string &entryPointName, const std::vector<std::string> &componentNames)
		{
			AST::SpecializationVisitor specializer(componentNames);
			m_RootNode->Accept(specializer);

			AST::Transformer moduleVisitor;
			AST::Node::Abstract *transformedRootNode = m_RootNode->Accept(moduleVisitor).front();

			AST::SemanticAnalyzer semanticAnalyzer;
			//transformedRootNode->Accept(semanticAnalyzer);

			//(Temporary) log transformed AST
			AST::LogVisitor logger;
			transformedRootNode->Accept(logger);

			//Generate descriptor layouts from component data
			for(const auto i : moduleVisitor.GetComponentData())
			{
				RHI::DescriptorGroupLayout::Binding *bindings = new RHI::DescriptorGroupLayout::Binding;
				memcpy(bindings, i.second.bindings.data(), sizeof(RHI::DescriptorGroupLayout::Binding) * i.second.bindings.size());
				//TODO: Proper bind flag
				//m_ComponentLayouts[i.first] = RHI::DescriptorGroupLayout(i.second.bindings.size(), bindings, RHI::Shader::StageFlag::Pixel);
			}

			Translator translator;
			transformedRootNode->Accept(translator);
			const IR::IR &intermediateRepresentation = translator.IR();

			#if GM_FLEX_SHADER_ENABLE_IR_LOGGING
				LogIR(intermediateRepresentation);
			#endif

			IR::SSA ssa(intermediateRepresentation);

			//IR::Optimize(intermediateRepresentation);

			CodeGenerator *generator = new CodeGenerator(intermediateRepresentation, entryPointName);
			return generator->GetData();
		}
	}
}