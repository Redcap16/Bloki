#include <engine/qxml/QXMLReader.hpp>

#include <stack>
#include <string>
#include <vector>
#include <map>

namespace QXML {
	QXMLReader::ContentManager::ContentManager(const std::vector<char>& content, int startIndex) :
		m_Content(content),
		m_CurrentCharIndex(startIndex) {

	}

	QXMLReader::ContentManager QXMLReader::ContentManager::GetSubcontent(int startIndex) {
		return ContentManager(m_Content, startIndex);
	}

	char QXMLReader::ContentManager::GetCharacter(int offset, int move) {
		if (m_CurrentCharIndex + offset >= m_Content.size())
			return 0;
		char result = m_Content[m_CurrentCharIndex + offset];
		Move(move);
		return result;
	}

	void QXMLReader::ContentManager::SkipWhitespaces() {
		while (!End() && std::isspace(m_Content[m_CurrentCharIndex]))
			Move(1);
	}

	void QXMLReader::ContentManager::Move(int count) {
		m_CurrentCharIndex += count;
		if (m_CurrentCharIndex + count >= m_Content.size())
			m_CurrentCharIndex = m_Content.size() - 1;
	}

	void QXMLReader::ContentManager::MoveTo(int position) {
		m_CurrentCharIndex = position < 0 ? 0 : (position >= m_Content.size() ? m_Content.size() - 1 : position);
	}

	bool QXMLReader::ContentManager::End() {
		return m_CurrentCharIndex >= m_Content.size() - 1;
	}

	std::string QXMLReader::ContentManager::ParseString() {
		std::string result;
		while (std::isalnum(GetCharacter())) {
			result += GetCharacter();
			Move(1);
		}

		return result;
	}

	int QXMLReader::ContentManager::ParseInteger() {
		std::string result;
		while (std::isdigit(GetCharacter())) {
			result += GetCharacter();
			Move(1);
		}

		return std::stoi(result);
	}

	int QXMLReader::ContentManager::FindNext(char c) {
		for (int i = m_CurrentCharIndex; i < m_Content.size(); ++i)
			if (m_Content[i] == c)
				return i;
	}

	std::vector<char> QXMLReader::ContentManager::GetSubstring(int from, int length) {
		if (from < 0 || length <= 0)
			return {};
		if (m_Content.size() < from + length)
			return {};
		return std::vector<char>(m_Content.begin() + from, m_Content.begin() + from + length);
	}

	void QXMLReader::invokeError(std::string reason) {
		m_ErrorStack.push_back(reason);
	}

	bool QXMLReader::parseRawContents(ContentManager &content, Element& element) {
		int length = element.GetAttributeValue("raw").m_Value;
		if (!length)
			return false;

		std::vector<char> data = content.GetSubstring(content.GetCurrentIndex(), length);
		element.AddData(data);

		content.Move(length);
		if (content.GetCharacter() != '<') {
			invokeError("Invalid raw content length");
			return false;
		}

		return true;
	}

	bool QXMLReader::parseElementContents(ContentManager &content, Element& element) {
		int endIndex = content.FindNext('<');
		if (endIndex == std::string::npos) {
			invokeError("Cant find closing bracket");
			return false;
		}

		std::vector<char> data = content.GetSubstring(content.GetCurrentIndex(), endIndex - content.GetCurrentIndex());
		element.AddData(data);

		content.MoveTo(endIndex + 1);
		content.SkipWhitespaces();
		while (content.CurrentIsAlpha()) { //Inner element
			ContentManager subContent = content.GetSubcontent(endIndex);
			if (!parseElement(subContent, element))
				return false; //Invalid inner element

			content.MoveTo(subContent.GetCurrentIndex());

			endIndex = content.FindNext('<');
			if (endIndex == std::string::npos) {
				invokeError("Cant find closing bracket");
				return false;
			}
			std::vector<char> data = content.GetSubstring(content.GetCurrentIndex(), endIndex - content.GetCurrentIndex());
			element.AddData(data);

			content.MoveTo(endIndex + 1);
			content.SkipWhitespaces();
		}

		content.MoveTo(endIndex);
		return true;
	}

	bool QXMLReader::parseElement(ContentManager &content, Element& parent) {
		content.SkipWhitespaces();
		if (content.GetCharacter(0, 1) != '<')
			return false;

		content.SkipWhitespaces();
		std::string tagName = content.ParseString();
		if (tagName.empty()) {
			invokeError("Invalid element syntax: missing tag name");
			return false;
		}
		content.SkipWhitespaces();

		Element newElement(tagName);

		//Parse attributes
		while (content.CurrentIsAlphanumeric()) {
			std::string attributeName = content.ParseString();
			content.SkipWhitespaces();
			if (content.GetCharacter(0, 1) != '=') {
				invokeError("Invalid attribute syntax: missing equal sign");
				return false;
			}
			content.SkipWhitespaces();

			int attributeValue = content.ParseInteger();
			newElement.AddAttribute({ attributeName, attributeValue });

			content.SkipWhitespaces();
		}

		if (content.GetCharacter(0, 1) != '>') {
			invokeError("Invalid element syntax: missing > sign");
			return false;
		}

		//Parsing contents

		const bool rawContent = newElement.GetAttributeValue("raw").m_Value != 0;
		if (rawContent) {
			if (!parseRawContents(content, newElement))
				return false;
		}
		else {
			if (!parseElementContents(content, newElement))
				return false;
		}


		if (content.GetCharacter(0, 1) != '<') {
			invokeError("Invalid tag syntax: missing closing bracket");
			return false;
		}
		content.SkipWhitespaces();

		if (content.GetCharacter(0, 1) != '/') {
			invokeError("Invalid tag syntax: missing closing tag");
			return false;
		}

		content.SkipWhitespaces();
		std::string closingTag = content.ParseString();
		if (closingTag != tagName) {
			invokeError("Opening and closing tags doesnt match!");
			return false;
		}

		content.SkipWhitespaces();
		if (content.GetCharacter(0, 1) != '>') {
			invokeError("Invalid tag syntax: missing closing bracket");
			return false;
		}

		parent.AddInnerElement(newElement);
		return true;
	}

	void QXMLReader::parseDocument() {
		ContentManager content(m_Content, 0);

		int endIndex = content.FindNext('<');
		if (endIndex == std::string::npos) {
			invokeError("Cant find opening bracket");
			return;
		}

		std::vector<char> data = content.GetSubstring(content.GetCurrentIndex(), endIndex - content.GetCurrentIndex());
		m_Base.AddData(data);

		content.MoveTo(endIndex + 1);
		content.SkipWhitespaces();
		while (content.CurrentIsAlpha()) { //Inner element
			ContentManager subContent = content.GetSubcontent(endIndex);
			if (!parseElement(subContent, m_Base))
				return; //Invalid inner element

			content.MoveTo(subContent.GetCurrentIndex());

			endIndex = content.FindNext('<');
			if (endIndex == std::string::npos) {
				m_Base.AddData(std::vector<char>(m_Content.begin() + content.GetCurrentIndex(), m_Content.end()));
				break;
			}
			std::vector<char> data = content.GetSubstring(content.GetCurrentIndex(), endIndex - content.GetCurrentIndex());
			m_Base.AddData(data);

			content.MoveTo(endIndex + 1);
			content.SkipWhitespaces();
		}

		content.MoveTo(endIndex);
		return;
	}

	QXMLReader::QXMLReader(const std::vector<char>& contents) :
		m_Content(contents),
		m_Base("") {

		parseDocument();
	}

	QXMLReader::QXMLReader(const std::string& contents) :
		m_Content(contents.begin(), contents.end()),
		m_Base("") {

		parseDocument();
	}

	QXMLReader QXML::QXMLReader::OpenFile(std::string filename) {
		std::ifstream file(filename, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
		if (file.is_open()) {
			size_t filesize = file.tellg();
			file.seekg(0, std::ios::beg);
			std::vector<char> filedata(filesize);
			file.read(&filedata[0], filesize);
			file.close();

			return QXMLReader(filedata);
		}
		return QXMLReader("");
	}
}