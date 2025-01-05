#pragma once

#include <stack>
#include <string>
#include <vector>
#include <map>
#include "engine/qxml/Element.hpp"

namespace QXML {
	class QXMLReader {
	private:
		Element m_Base;
		std::vector<char> m_Content;
		std::deque<std::string> m_ErrorStack;

		class ContentManager {
		private:
			const std::vector<char>& m_Content;
			int m_CurrentCharIndex;

		public:
			ContentManager(const std::vector<char>& content, int startIndex);
			
			ContentManager GetSubcontent(int startIndex);
			char GetCharacter(int offset = 0, int move = 0);

			void SkipWhitespaces();
			void Move(int count);
			void MoveTo(int position);
			bool End();

			std::string ParseString();
			int ParseInteger();

			bool CurrentIsAlphanumeric() {
				return std::isalnum(GetCharacter());
			}

			bool CurrentIsAlpha() {
				return std::isalpha(GetCharacter());
			}

			int GetCurrentIndex() {
				return m_CurrentCharIndex;
			}

			int FindNext(char c);
			std::vector<char> GetSubstring(int from, int length);
		};

		void invokeError(std::string reason);
		bool parseRawContents(ContentManager &content, Element& element);
		bool parseElementContents(ContentManager &content, Element& element);

		bool parseElement(ContentManager &content, Element& parent);
		void parseDocument();

	public:
		QXMLReader(const std::vector<char>& contents);
		QXMLReader(const std::string& contents);
		
		std::string GetErrorStack() const {
			std::string result;
			for (auto& error : m_ErrorStack)
				result.append(error + "\n");

			return result;
		}

		const Element& GetBase() const {
			return m_Base;
		}
	};
}