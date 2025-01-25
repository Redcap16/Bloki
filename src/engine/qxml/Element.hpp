#pragma once

#include <string>
#include <vector>
#include <map>

namespace QXML {
	struct Attribute {
		std::string m_Name;
		int m_Value;

		Attribute(std::string name, int value) :
			m_Name(name),
			m_Value(value) {

		}
	};

	struct Element {
	private:
		std::string m_Tag;
		std::vector<char> m_Data;
		bool m_Raw = false;
		std::map<std::string, std::vector<Element>> m_InnerElements;
		std::map<std::string, Attribute> m_Attributes;

	public:
		Element(const std::string& tag);
		Element(const std::string& tag, const std::vector<char>& data, const std::map<std::string, std::vector<Element>> innerElements);

		void AddInnerElement(const Element& element) {
			m_InnerElements[element.m_Tag].push_back(element);
		}

		void AddAttribute(const Attribute& attribute) {
			if(HasAtribute(attribute.m_Name))
				m_Attributes.at(attribute.m_Name) = attribute;
			else
				m_Attributes.insert({ attribute.m_Name, attribute });
		}

		void AddData(const std::string& data) {
			m_Data.insert(m_Data.end(), data.begin(), data.end());
			if (m_Raw)
				m_Attributes.at("raw").m_Value = m_Data.size();
		}

		void AddData(const std::vector<char>& data) {
			m_Data.insert(m_Data.end(), data.begin(), data.end());
			if(m_Raw)
				m_Attributes.at("raw").m_Value = m_Data.size();
		}

		const std::vector<Element>* GetElementsByTag(std::string tag) const;
		std::vector<Element> GetAllElements() const;
		std::vector<std::string> GetAllInnerTags() const;

		Attribute GetAttributeValue(std::string attribute) const;
		std::vector<std::string> GetAllAtributes() const;
		bool HasAtribute(std::string attribute) const;
		const std::vector<char>& GetData() const { return m_Data; };
		std::string GetDataAsString() const { 
			return { m_Data.begin(), m_Data.end() };
		};

		const std::string& GetTagName() const { return m_Tag; };

		void SetAsRaw();
	};
}