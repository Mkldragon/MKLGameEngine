#pragma once

#include "Parse.h"
#include "Lexer.h"
#include "Compiler_DX11.h"

namespace sge
{
	void ShaderParse::LoadFile(StrView filename)
	{
		MemMapFile mm;
		mm.open(filename);

	}

	void ShaderParse::SetshaderPath(StrView filename)
	{
		String path = filename.data();
		int sPos = path.find_last_of("/\\");
		int ePos = path.find_last_of('.');
		int size = ePos - sPos;
		String fn = path.substr(sPos + 1, path.length());

		_shaderData.fileName = fn;
		_shaderData.path = path;
	}
	void ShaderParse::Parse(Span<const u8> src)
	{
		_src = StrView(reinterpret_cast<const char*>(src.data()), src.size());
		_lexer.Read(_src);
		
		_token = _lexer.nextToken();

		if (_token->CheckToken(TokenType::None))
			SGE_LOG_ERROR("TokenType is none");

		else if (_token->CheckToken(TokenType::Identifier, "Shader"))
		{
			_token = _lexer.nextToken();
			if (_token->CheckToken(TokenType::None)) 
				SGE_LOG_ERROR("TokenType is none");

			if (_token->type == TokenType::String)
			{
				_shaderData.shaderName = _token->value.c_str();
				_token = _lexer.nextToken();
			}

			if (_token->CheckToken(TokenType::Operator, "{"))
			{
				_numbracket++;
				_readProperties();
			}
			
		}
	
		SGE_LOG("\nFilePath:{}\nFileName:{}\nShaderName:{}\nPassSize:{}\nPropertySize:{}\n", _shaderData.path,
			_shaderData.fileName,_shaderData.shaderName,
			_shaderData.passes.size(), _shaderData.props.size());

		StrView shaderFilename = Fmt("Assets/Shaders/{}", _shaderData.fileName);
		String outputPath = Fmt("LocalTemp/{}", shaderFilename);
		auto jsonFilename = Fmt("{}/info.json", outputPath);


		outputPath = Fmt("LocalTemp/{}/DX11", shaderFilename);
		SGE_LOG("outputPath : {}", outputPath);
		Directory::create(outputPath);

		JsonUtil::writeFile(jsonFilename, _shaderData, false);

		ShaderCompiler hlslCompiler;
		hlslCompiler.CompilerShader(&_shaderData);


	}

	void ShaderParse::_readProperties()
	{
		while (true)
		{
			_token = _lexer.nextToken();
			if (_token->CheckToken(TokenType::Operator, "}"))				{ break; }
			if (_token->CheckToken(TokenType::Identifier, "Properties"))	{ _readProperty(); }
			if (_token->CheckToken(TokenType::Identifier, "Pass"))			{ _readPass(); }
			if (_token->CheckToken(TokenType::None))						{ break; }
		}
		
	}

	void ShaderParse::_readProperty()
	{
		_token = _lexer.nextToken();
		if (!_token->CheckToken(TokenType::Operator, "{")) SGE_LOG_ERROR("Properties not {");

		while (true)
		{
			_token = _lexer.nextToken();
			
			if (_token->CheckToken(TokenType::Operator, "}")) { break; }
			{
				auto& o = _shaderData.props.emplace_back();
				o.name = _token->value;

				while (!_token->CheckToken(TokenType::Operator, ";"))
				{
					_token = _lexer.nextToken();
					
					if (_token->CheckToken(TokenType::String)) o.displayName = _token->value;;
					if (_token->CheckToken(TokenType::Identifier)) enumTryParse(o.propTypr, _token->value.c_str());
					if (_token->CheckToken(TokenType::Operator, "="))
					{
						_token = _lexer.nextToken();
						while (!_token->CheckToken(TokenType::Operator, ";"))
						{
							if (_token->CheckToken(TokenType::None)) break;
							o.defaultValue += _token->value;
							_token = _lexer.nextToken();
						}
						SGE_LOG("V : {}", o.defaultValue);
					}
					if (_token->CheckToken(TokenType::None)) break;
				}
				
			}
		}
	}

	void ShaderParse::_readPass()
	{
		int _shaderbracket = 0;

		auto& newPass = _shaderData.passes.emplace_back();
		String tokenValue;

		while (true)
		{
			_token = _lexer.nextToken();
			

			if (_token->CheckToken(TokenType::Operator, "{"))
			{
				_shaderbracket++;
			}

			if (_token->CheckToken(TokenType::Operator, "}"))
			{
				if (--_shaderbracket == 0) break;
			}

			if(_token->CheckToken(TokenType::Identifier))
			{

				if (_token->type == TokenType::None) break;

				else if (_token->type == TokenType::Identifier)
				{
					int a = 0;
					if (_token->CheckValue("VsFunc"))
						newPass.vsEntryPt = _getIdfValue();
					else if (_token->CheckValue("PsFunc"))
						newPass.psEntryPt = _getIdfValue();
					else if (_token->CheckValue("Cull"))
						newPass.renderState.cull = _getIdfValue();
					else if (_token->CheckValue("WireFrame"))
						newPass.renderState.wireFrame = _getIdfValue();
					else if (_token->CheckValue("DepthTest"))
						newPass.renderState.depthTest = _getIdfValue();
					else if (_token->CheckValue("DepthWrite"))
						newPass.renderState.depthWrite = _getIdfValue();
					else if (_token->CheckValue("BlendRGB"))
					{
						newPass.renderState.blendRGB.op		= _getIdfValue();
						newPass.renderState.blendRGB.src	= _getIdfValue();
						newPass.renderState.blendRGB.dest	= _getIdfValue();
					}										  
					else if (_token->CheckValue("BlendAlpha"))
					{										  
						newPass.renderState.blendAlpha.op	= _getIdfValue();
						newPass.renderState.blendAlpha.src	= _getIdfValue();
						newPass.renderState.blendAlpha.dest = _getIdfValue();

					}

				}
			}
			if (_token->type == TokenType::None) break;

		}
	}

	String ShaderParse::_getIdfValue()
	{
		String value = "";
		_token = _lexer.nextToken();
		
		if (_token->type == TokenType::None)
		{
			SGE_LOG_ERROR("Txt End");
			return value;
		}

		if (_token->type != TokenType::Identifier)
			SGE_LOG_ERROR("Wrong Value");

		value = _token->value;
		return value;
	}
}