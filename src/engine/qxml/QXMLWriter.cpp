#include <engine/qxml/QXMLWriter.hpp>

QXML::QXMLWriter::QXMLWriter() {

}

QXML::QXMLWriter::QXMLWriter(std::string filename) :
	m_OutputFile(filename, std::ios_base::out | std::ios_base::binary) {
}

QXML::QXMLWriter::~QXMLWriter() {
	m_OutputFile.close();
}

void QXML::QXMLWriter::AddElement(const Element& element) {
	std::vector<char> data = processElement(element);

	m_OutputData.insert(m_OutputData.end(), data.begin(), data.end());

	if(m_OutputFile.is_open())
		m_OutputFile.write(&data[0], data.size());
}

const std::vector<char>& QXML::QXMLWriter::GetResult() {
	return m_OutputData;
}

std::vector<char> QXML::QXMLWriter::processElement(const Element& element) {
	std::vector<char> result;

	result.push_back('<');
	pushString(result, element.GetTagName());
	for (auto& attribute : element.GetAllAtributes()) {
		result.push_back(' ');
		pushString(result, attribute);
		result.push_back('=');
		pushString(result, std::to_string(element.GetAttributeValue(attribute).m_Value));
	}
	result.push_back('>');

	auto& data = element.GetData();
	result.insert(result.end(), data.begin(), data.end());

	for (auto& innerElement : element.GetAllElements()) {
		std::vector<char> processedElement = processElement(innerElement);
		result.insert(result.end(), processedElement.begin(), processedElement.end());
	}

	result.push_back('<');
	result.push_back('/');
	pushString(result, element.GetTagName());
	result.push_back('>');

	return result;
}

void QXML::QXMLWriter::pushString(std::vector<char>& data, const std::string& str) {
	for (char c : str)
		data.push_back(c);
}