#pragma once

#include <memory>
#include <unordered_map>
#include <functional>

template <class TResource>
struct ResourceParams
{

};

template <class TResource>
class Resource
{
public:
	template <typename... Args>
	Resource(Args&&... args);
	~Resource();

	Resource(const Resource& other) = default;
	Resource& operator=(const Resource& other) = default;
	Resource(Resource&& other) noexcept;
	Resource& operator=(Resource&& other) noexcept;

	TResource* Get() { return m_Instance.get(); }
	const TResource* Get() const { return m_Instance.get(); }

	TResource* operator->() { return m_Instance.get(); }
	const TResource* operator->() const { return m_Instance.get(); }

private:
	std::shared_ptr<TResource> m_Instance;
	static std::unordered_map<ResourceParams<TResource>, std::weak_ptr<TResource>> m_Resources;

	void lastInstanceCheck();
};

template <class TResource>
std::unordered_map<ResourceParams<TResource>, std::weak_ptr<TResource>> Resource<TResource>::m_Resources;

template <class TResource>
template <typename... Args>
Resource<TResource>::Resource(Args&&... args)
{
	ResourceParams<TResource> params(std::forward<Args>(args)...);
	auto it = m_Resources.find(params);
	if (it != m_Resources.end())
	{
		m_Instance = it->second.lock();
		if (m_Instance)
			return;
	}

	//Resource doesnt exist or cant be locked
	m_Instance = std::make_shared<TResource>(std::forward<Args>(args)...);
	m_Resources[params] = m_Instance;
}

template <class TResource>
Resource<TResource>::~Resource()
{
	lastInstanceCheck();
}

template <class TResource>
Resource<TResource>::Resource(Resource&& other) noexcept :
	m_Instance(std::move(other.m_Instance))
{

}

template <class TResource>
Resource<TResource>& Resource<TResource>::operator=(Resource&& other) noexcept
{
	if (this == &other)
		return *this;

	lastInstanceCheck();
	m_Instance = std::move(other.m_Instance);

	return *this;
}

template <class TResource>
void Resource<TResource>::lastInstanceCheck()
{
	if (m_Instance.use_count() == 1) //Last resource reference
	{
		m_Instance.reset();

		for (auto it = m_Resources.begin(); it != m_Resources.end(); ++it)
			if (it->second.expired())
			{
				m_Resources.erase(it);
				break;
			}
	}
}