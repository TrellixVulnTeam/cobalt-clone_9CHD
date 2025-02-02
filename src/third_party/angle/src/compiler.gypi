# Copyright (c) 2013 The ANGLE Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
    'variables':
    {
        # These file lists are shared with the GN build.
        'angle_translator_sources':
        [
            '<(DEPTH)/third_party/angle/include/EGL/egl.h',
            '<(DEPTH)/third_party/angle/include/EGL/eglext.h',
            '<(DEPTH)/third_party/angle/include/EGL/eglplatform.h',
            '<(DEPTH)/third_party/angle/include/GLES2/gl2.h',
            '<(DEPTH)/third_party/angle/include/GLES2/gl2ext.h',
            '<(DEPTH)/third_party/angle/include/GLES2/gl2platform.h',
            '<(DEPTH)/third_party/angle/include/GLES3/gl3.h',
            '<(DEPTH)/third_party/angle/include/GLES3/gl3platform.h',
            '<(DEPTH)/third_party/angle/include/GLES3/gl31.h',
            '<(DEPTH)/third_party/angle/include/GLES3/gl32.h',
            '<(DEPTH)/third_party/angle/include/GLSLANG/ShaderLang.h',
            '<(DEPTH)/third_party/angle/include/GLSLANG/ShaderVars.h',
            '<(DEPTH)/third_party/angle/include/KHR/khrplatform.h',
            '<(DEPTH)/third_party/angle/include/angle_gl.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/AddAndTrueToLoopCondition.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/AddAndTrueToLoopCondition.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/BaseTypes.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/BuiltInFunctionEmulator.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/BuiltInFunctionEmulator.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/BreakVariableAliasingInInnerLoops.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/BreakVariableAliasingInInnerLoops.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/Cache.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/Cache.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/CallDAG.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/CallDAG.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/CodeGen.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/Common.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/Compiler.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/Compiler.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ConstantUnion.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ConstantUnion.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/DeferGlobalInitializers.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/DeferGlobalInitializers.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/Diagnostics.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/Diagnostics.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/DirectiveHandler.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/DirectiveHandler.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/EmulateGLFragColorBroadcast.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/EmulateGLFragColorBroadcast.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/EmulatePrecision.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/EmulatePrecision.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ExpandIntegerPowExpressions.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ExpandIntegerPowExpressions.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ExtensionBehavior.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/FindMain.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/FindMain.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/FindSymbolNode.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/FindSymbolNode.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/FlagStd140Structs.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/FlagStd140Structs.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/HashNames.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/InfoSink.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/InfoSink.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/Initialize.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/Initialize.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/InitializeDll.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/InitializeDll.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/InitializeGlobals.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/InitializeVariables.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/InitializeVariables.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/IntermNode.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/IntermNode.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/IntermTraverse.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/Intermediate.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/Intermediate.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/NodeSearch.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/Operator.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/Operator.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ParseContext.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ParseContext.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/PoolAlloc.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/PoolAlloc.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/Pragma.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/PruneEmptyDeclarations.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/PruneEmptyDeclarations.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/PrunePureLiteralStatements.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/PrunePureLiteralStatements.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/QualifierTypes.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/QualifierTypes.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RecordConstantPrecision.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RecordConstantPrecision.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RegenerateStructNames.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RegenerateStructNames.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RemoveInvariantDeclaration.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RemoveInvariantDeclaration.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RemovePow.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RemovePow.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RewriteDoWhile.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RewriteDoWhile.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RewriteTexelFetchOffset.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RewriteTexelFetchOffset.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RewriteUnaryMinusOperatorFloat.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RewriteUnaryMinusOperatorFloat.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RewriteUnaryMinusOperatorInt.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RewriteUnaryMinusOperatorInt.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ScalarizeVecAndMatConstructorArgs.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ScalarizeVecAndMatConstructorArgs.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/SearchSymbol.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/SearchSymbol.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/Severity.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ShaderLang.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ShaderVars.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/SymbolTable.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/SymbolTable.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/Types.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/Types.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/UnfoldShortCircuitAST.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/UnfoldShortCircuitAST.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/UseInterfaceBlockFields.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/UseInterfaceBlockFields.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ValidateGlobalInitializer.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ValidateGlobalInitializer.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ValidateLimitations.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ValidateLimitations.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ValidateMaxParameters.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ValidateMaxParameters.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ValidateMultiviewWebGL.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ValidateMultiviewWebGL.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ValidateOutputs.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ValidateOutputs.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ValidateSwitch.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ValidateSwitch.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/VariableInfo.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/VariableInfo.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/VariablePacker.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/VariablePacker.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/blocklayout.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/blocklayout.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/glslang.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/glslang.l',
            '<(DEPTH)/third_party/angle/src/compiler/translator/glslang.y',
            '<(DEPTH)/third_party/angle/src/compiler/translator/glslang_lex.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/glslang_tab.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/glslang_tab.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/intermOut.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/length_limits.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/util.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/util.h',
            '<(DEPTH)/third_party/angle/src/third_party/compiler/ArrayBoundsClamper.cpp',
            '<(DEPTH)/third_party/angle/src/third_party/compiler/ArrayBoundsClamper.h',
        ],
        'angle_translator_essl_sources':
        [
            '<(DEPTH)/third_party/angle/src/compiler/translator/OutputESSL.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/OutputESSL.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/TranslatorESSL.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/TranslatorESSL.h',
        ],
        'angle_translator_glsl_sources':
        [
            '<(DEPTH)/third_party/angle/src/compiler/translator/BuiltInFunctionEmulatorGLSL.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/BuiltInFunctionEmulatorGLSL.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ExtensionGLSL.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ExtensionGLSL.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/OutputGLSL.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/OutputGLSL.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/OutputGLSLBase.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/OutputGLSLBase.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/TranslatorGLSL.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/TranslatorGLSL.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/VersionGLSL.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/VersionGLSL.h',
        ],
        'angle_translator_hlsl_sources':
        [
            '<(DEPTH)/third_party/angle/src/compiler/translator/AddDefaultReturnStatements.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/AddDefaultReturnStatements.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ArrayReturnValueToOutParameter.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ArrayReturnValueToOutParameter.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ASTMetadataHLSL.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/ASTMetadataHLSL.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/blocklayoutHLSL.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/blocklayoutHLSL.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/BuiltInFunctionEmulatorHLSL.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/BuiltInFunctionEmulatorHLSL.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/IntermNodePatternMatcher.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/IntermNodePatternMatcher.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/OutputHLSL.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/OutputHLSL.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RemoveDynamicIndexing.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RemoveDynamicIndexing.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RemoveSwitchFallThrough.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RemoveSwitchFallThrough.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RewriteElseBlocks.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/RewriteElseBlocks.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/SeparateArrayInitialization.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/SeparateArrayInitialization.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/SeparateDeclarations.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/SeparateDeclarations.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/SeparateExpressionsReturningArrays.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/SeparateExpressionsReturningArrays.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/SimplifyLoopConditions.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/SimplifyLoopConditions.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/SplitSequenceOperator.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/SplitSequenceOperator.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/StructureHLSL.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/StructureHLSL.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/TextureFunctionHLSL.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/TextureFunctionHLSL.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/TranslatorHLSL.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/TranslatorHLSL.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/UnfoldShortCircuitToIf.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/UnfoldShortCircuitToIf.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/UniformHLSL.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/UniformHLSL.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/UtilsHLSL.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/UtilsHLSL.h',
        ],
        'angle_translator_lib_vulkan_sources':
        [
            '<(DEPTH)/third_party/angle/src/compiler/translator/OutputVulkanGLSL.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/OutputVulkanGLSL.h',
            '<(DEPTH)/third_party/angle/src/compiler/translator/TranslatorVulkan.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/translator/TranslatorVulkan.h',
        ],
        'angle_preprocessor_sources':
        [
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/DiagnosticsBase.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/DiagnosticsBase.h',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/DirectiveHandlerBase.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/DirectiveHandlerBase.h',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/DirectiveParser.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/DirectiveParser.h',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/ExpressionParser.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/ExpressionParser.h',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/ExpressionParser.y',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/Input.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/Input.h',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/Lexer.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/Lexer.h',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/Macro.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/Macro.h',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/MacroExpander.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/MacroExpander.h',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/Preprocessor.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/Preprocessor.h',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/SourceLocation.h',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/Token.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/Token.h',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/Tokenizer.cpp',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/Tokenizer.h',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/Tokenizer.l',
            '<(DEPTH)/third_party/angle/src/compiler/preprocessor/numeric_lex.h',
        ],
    },
    # Everything below this is duplicated in the GN build. If you change
    # anything also change angle/BUILD.gn
    'targets':
    [
        {
            'target_name': 'preprocessor',
            'type': 'static_library',
            'dependencies': [ 'angle_common' ],
            'includes': [ '../gyp/common_defines.gypi', ],
            'sources': [ '<@(angle_preprocessor_sources)', ],
        },
        {
            'target_name': 'translator',
            'type': 'static_library',
            'dependencies': [ 'preprocessor', 'angle_common' ],
            'includes': [ '../gyp/common_defines.gypi', ],
            'include_dirs':
            [
                '<(DEPTH)/third_party/angle/include',
                '<(DEPTH)/third_party/angle/src',
            ],
            'sources':
            [
                '<@(angle_translator_sources)',
            ],
            'msvs_settings':
            {
              'VCLibrarianTool':
              {
                'AdditionalOptions': ['/ignore:4221']
              },
            },
            'conditions':
            [
                ['angle_enable_essl==1',
                {
                    'defines':
                    [
                        'ANGLE_ENABLE_ESSL',
                    ],
                    'direct_dependent_settings':
                    {
                        'defines':
                        [
                            'ANGLE_ENABLE_ESSL',
                        ],
                    },
                    'sources':
                    [
                        '<@(angle_translator_essl_sources)',
                    ],
                }],
                ['angle_enable_glsl==1',
                {
                    'defines':
                    [
                        'ANGLE_ENABLE_GLSL',
                    ],
                    'direct_dependent_settings':
                    {
                        'defines':
                        [
                            'ANGLE_ENABLE_GLSL',
                        ],
                    },
                    'sources':
                    [
                        '<@(angle_translator_glsl_sources)',
                    ],
                }],
                ['angle_enable_hlsl==1',
                {
                    'defines':
                    [
                        'ANGLE_ENABLE_HLSL',
                    ],
                    'direct_dependent_settings':
                    {
                        'defines':
                        [
                            'ANGLE_ENABLE_HLSL',
                        ],
                    },
                    'sources':
                    [
                        '<@(angle_translator_hlsl_sources)',
                    ],
                }],
                ['angle_enable_vulkan==1',
                {
                    'defines':
                    [
                        'ANGLE_ENABLE_VULKAN',
                    ],
                    'direct_dependent_settings':
                    {
                        'defines':
                        [
                            'ANGLE_ENABLE_VULKAN',
                        ],
                    },
                    'sources':
                    [
                        '<@(angle_translator_lib_vulkan_sources)',
                    ],
                }],
            ],
        },
    ],
}
