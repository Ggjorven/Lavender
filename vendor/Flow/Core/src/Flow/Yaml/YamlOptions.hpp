#pragma once

#include <yaml-cpp/yaml.h>

namespace Flow::Yaml
{

	// From yaml.h
	enum class FileManip
	{
		// General manipulators
		Auto,
		TagByKind,
		Newline,

		// Output character set
		EmitNonAscii,
		EscapeNonAscii,
		EscapeAsJson,

		// String manipulators
		// Auto, // duplicate
		SingleQuoted,
		DoubleQuoted,
		Literal,

		// Null manipulators
		LowerNull,
		UpperNull,
		CamelNull,
		TildeNull,

		// Bool manipulators
		YesNoBool,      // yes, no
		TrueFalseBool,  // true, false
		OnOffBool,      // on, off
		UpperCase,      // TRUE, N
		LowerCase,      // f, yes
		CamelCase,      // No, Off
		LongBool,       // yes, On
		ShortBool,      // y, t

		// Int manipulators
		Dec,
		Hex,
		Oct,

		// Document manipulators
		BeginDoc,
		EndDoc,

		// Sequence manipulators
		BeginSeq,
		EndSeq,
		Flow,
		Block,

		// Map manipulators
		BeginMap,
		EndMap,
		Key,
		Value,
		// Flow, // duplicate
		// Block, // duplicate
		// Auto, // duplicate
		LongKey
	};

}