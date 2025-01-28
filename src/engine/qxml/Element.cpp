#include "engine/qxml/Element.hpp"

namespace QXML {
	Element::Element(const std::string& tag) :
		m_Tag(tag) {

	}

	Element::Element(const std::string& tag, const std::vector<char>& data, const std::map<std::string, std::vector<Element>> innerElements) :
		m_Tag(tag),
		m_Data(data),
		m_InnerElements(innerElements) {

	}

	const std::vector<Element>* Element::GetElementsByTag(std::string tag) const {
		if (m_InnerElements.find(tag) != m_InnerElements.end())
			return &m_InnerElements.at(tag);
		return nullptr;
	}

	std::vector<Element> Element::GetAllElements() const {
		std::vector<Element> result;
		for (auto& pair : m_InnerElements)
			result.insert(result.end(), pair.second.begin(), pair.second.end());

		return result;
	}

	std::vector<std::string> Element::GetAllInnerTags() const {
		std::vector<std::string> result;
		for (auto& pair : m_InnerElements)
			result.push_back(pair.first);

		return result;
	}

	Attribute Element::GetAttributeValue(std::string attribute) const {
		if (m_Attributes.find(attribute) != m_Attributes.end())
			return m_Attributes.at(attribute);

		return Attribute({ attribute, 0 });
	}

	std::vector<std::string> Element::GetAllAtributes() const {
		std::vector<std::string> result;
		for (auto& pair : m_Attributes)
			result.push_back(pair.first);

		return result;
	}

	bool Element::HasAtribute(std::string attribute) const {
		return m_Attributes.find(attribute) != m_Attributes.end();
	}

	void Element::SetAsRaw() {
		m_Raw = true;
		AddAttribute(Attribute("raw", static_cast<int>(m_Data.size())));
	}
}