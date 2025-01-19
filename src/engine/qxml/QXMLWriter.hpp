#pragma once

#include <stack>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include "engine/qxml/Element.hpp"

namespace QXML {
	class QXMLWriter {
	public:
		QXMLWriter();
		QXMLWriter(std::string filename);
		~QXMLWriter();

		void AddElement(Element element);
		const std::vector<char>& GetResult();

	private:
		std::ofstream m_OutputFile;
		std::vector<char> m_OutputData;

		std::vector<char> processElement(Element element);
		void pushString(std::vector<char>& data, const std::string& str);
	};
}